#include "Window.h"
#include <cassert>
#include <Windowsx.h>
#include "WindowSizeEventsObserver.h"

using namespace ArkanoidEngine;

Window::Window(HINSTANCE hInstance, UINT width, UINT height, std::string windowName, std::string className)
{
	WNDCLASSEX wndClass{};
	wndClass.cbSize = sizeof(WNDCLASSEX);
	wndClass.hInstance = hInstance;
	wndClass.lpfnWndProc = Window::WndProc;
	wndClass.lpszClassName = className.c_str();
	wndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndClass.style = CS_HREDRAW | CS_VREDRAW;

	ATOM registeredClass = RegisterClassEx(&wndClass);
	assert(registeredClass != 0);
	
	m_registeredClass = registeredClass;

	m_windowHandle = CreateWindowEx(
		0,
		className.c_str(),
		windowName.c_str(),
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		static_cast<int>(width),
		static_cast<int>(height),
		0,
		0,
		hInstance,
		this); //this is important, look at the WndProc implementation

	assert(m_windowHandle != NULL);

	RECT windowRect{};
	assert(GetWindowRect(m_windowHandle, static_cast<LPRECT>(&windowRect)) == TRUE);

	m_width = static_cast<UINT>(windowRect.right - windowRect.left);
	m_height = static_cast<UINT>(windowRect.bottom - windowRect.top);

	ShowWindow(m_windowHandle, SW_SHOW);
}

Window::~Window()
{
	if (m_windowHandle != 0)
	{
		DestroyWindow(m_windowHandle);
		DWORD registeredClass = static_cast<DWORD>(m_registeredClass);
		UnregisterClassA(reinterpret_cast<LPCSTR>(&registeredClass), 0);
	}
}

LRESULT CALLBACK Window::WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (uMsg == WM_DESTROY)
	{
		//Send WM_QUIT; this means that as soon as a window is destroyed, the application should terminate.
		PostQuitMessage(0);
		return 0;
	}

	Window* window = nullptr;

	if (uMsg == WM_NCCREATE)
	{
		CREATESTRUCT* pCreate = reinterpret_cast<CREATESTRUCT*>(lParam);
		window = static_cast<Window*>(pCreate->lpCreateParams);
		SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(window));
		//Very important, the CreateWindowEx function sends WM_NCCREATE, WM_NCCALCSIZE, and WM_CREATE messages 
		//to the window being created (before returning to the caller. look at the ctor implementation).
		//When WM_CREATE is sent, the pointer window is valid and the handleMessage method is called.
		//if m_windowHandle is not set here, during the handleMessage execution it could be used while having an invalid value.
		window->m_windowHandle = hWnd;
	}
	else
	{
		window = reinterpret_cast<Window*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
	}

	if (window)
	{
		return window->handleMessage(uMsg, wParam, lParam);
	}
	else
	{
		return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}
}

LRESULT Window::handleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{	
	case WM_SIZE:

		m_width = LOWORD(lParam);
		m_height = HIWORD(lParam);

		if (wParam == SIZE_MINIMIZED)
		{			
			m_eWindowSizeState = EWindowSizeState::MINIMIZED;
			onMinimized();
		}
		else if (wParam == SIZE_MAXIMIZED)
		{			
			m_eWindowSizeState = EWindowSizeState::MAXIMIZED;
			onMaximized();
			onResize();
		}
		else if (wParam == SIZE_RESTORED)
		{
			switch (m_eWindowSizeState)
			{
			case EWindowSizeState::MINIMIZED:
			case EWindowSizeState::MAXIMIZED:				
				m_eWindowSizeState = EWindowSizeState::NONE;
				onResize();
				break;
			case EWindowSizeState::RESIZING:
				break;
			case EWindowSizeState::NONE:
				onResize();
				break;
			}
		}

		return 0;
	case WM_ENTERSIZEMOVE:
		m_eWindowSizeState = EWindowSizeState::RESIZING;
		onStartResizing();
		return 0;
	case WM_EXITSIZEMOVE:
		m_eWindowSizeState = EWindowSizeState::NONE;
		onStopResizing();
		onResize();
		return 0;
	}
		
	LRESULT result = 1;

	for (WindowMessagesHandler* windowMessagesHandler : m_windowMessagesHandlers)
	{
		LRESULT handlerResult = windowMessagesHandler->handleMessage(uMsg, wParam, lParam);
		result *= handlerResult;
	}

	if (result == 0)
	{
		//if handlerResult is 0 for at least on WindowMessagesHandler then result will be 0
		return 0;
	}
	
	return DefWindowProc(m_windowHandle, uMsg, wParam, lParam);
}

void Window::onResize() const
{
	for (WindowSizeEventsObserver* windowSizeEventsObserver : m_windowSizeEventsObservers)
	{
		windowSizeEventsObserver->onResize();
	}
}

void Window::onMinimized() const
{
	for (WindowSizeEventsObserver* windowSizeEventsObserver : m_windowSizeEventsObservers)
	{
		windowSizeEventsObserver->onMinimized();
	}
}

void Window::onMaximized() const
{
	for (WindowSizeEventsObserver* windowSizeEventsObserver : m_windowSizeEventsObservers)
	{
		windowSizeEventsObserver->onMaximized();
	}
}

void Window::onStartResizing() const
{
	for (WindowSizeEventsObserver* windowSizeEventsObserver : m_windowSizeEventsObservers)
	{
		windowSizeEventsObserver->onStartResizing();
	}
}

void Window::onStopResizing() const
{
	for (WindowSizeEventsObserver* windowSizeEventsObserver : m_windowSizeEventsObservers)
	{
		windowSizeEventsObserver->onStopResizing();
	}
}

void Window::addWindowMessagesHandler(WindowMessagesHandler* windowMessagesHandler)
{
	assert(windowMessagesHandler != nullptr);
	m_windowMessagesHandlers.push_back(windowMessagesHandler);
}

void Window::removeWindowMessagesHandler(WindowMessagesHandler* windowMessagesHandler)
{
	assert(windowMessagesHandler != nullptr);
	auto it = std::find(m_windowMessagesHandlers.begin(), m_windowMessagesHandlers.end(), windowMessagesHandler);
	assert(it != m_windowMessagesHandlers.end());
	m_windowMessagesHandlers.erase(it);
}

void Window::addWindowSizeEventsObserver(WindowSizeEventsObserver* windowSizeEventsObserver)
{
	assert(windowSizeEventsObserver != nullptr);
	m_windowSizeEventsObservers.push_back(windowSizeEventsObserver);
}

void Window::removeWindowSizeEventsObserver(WindowSizeEventsObserver* windowSizeEventsObserver)
{
	assert(windowSizeEventsObserver != nullptr);
	auto it = std::find(m_windowSizeEventsObservers.begin(), m_windowSizeEventsObservers.end(), windowSizeEventsObserver);
	assert(it != m_windowSizeEventsObservers.end());
	m_windowSizeEventsObservers.erase(it);
}


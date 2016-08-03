#pragma once
#include "WindowsInclude.h"
#include <string>
#include "WindowMessagesHandler.h"
#include <vector>
#include "WindowsPlatformCheck.h"

namespace ArkanoidEngine
{
	class WindowSizeEventsObserver;

	class Window : public WindowMessagesHandler
	{
	public:
		static constexpr UINT sk_defaultWindowSize = CW_USEDEFAULT;

		//ctors
		explicit Window(HINSTANCE hInstance, UINT width = sk_defaultWindowSize, UINT height = sk_defaultWindowSize,
						std::string windowName = "Window", std::string className = "Default Window Class");

		//dtor
		virtual ~Window();

		//copy
		Window(const Window&) = delete;
		Window& operator=(const Window&) = delete;

		//move
		Window(Window&&) = delete;
		Window& operator=(Window&&) = delete;

		void addWindowMessagesHandler(WindowMessagesHandler* windowMessagesHandler);
		void removeWindowMessagesHandler(WindowMessagesHandler* windowMessagesHandler);

		void addWindowSizeEventsObserver(WindowSizeEventsObserver* windowSizeEventsObserver);
		void removeWindowSizeEventsObserver(WindowSizeEventsObserver* windowSizeEventsObserver);

		HWND windowHandle()const;
		UINT width()const;
		UINT height()const;
		float aspectRatio()const;

	private:
		static LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

		virtual LRESULT handleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) override;
		
		//resize events
		void onResize()const;
		void onMinimized()const;
		void onMaximized()const;
		void onStartResizing()const;
		void onStopResizing()const;
		
		std::vector<WindowMessagesHandler*> m_windowMessagesHandlers{};
		std::vector<WindowSizeEventsObserver*> m_windowSizeEventsObservers{};
		HWND m_windowHandle{ 0 };
		ATOM m_registeredClass{ 0 };
		UINT m_width{ 0 };
		UINT m_height{ 0 };

		enum class EWindowSizeState : char
		{
			NONE,
			MINIMIZED,
			MAXIMIZED,
			RESIZING
		};

		EWindowSizeState m_eWindowSizeState{ EWindowSizeState::NONE };
	};

	inline HWND Window::windowHandle() const
	{
		return m_windowHandle;
	}

	inline UINT Window::width() const
	{
		return m_width;
	}

	inline UINT Window::height() const
	{
		return m_height;
	}

	inline float Window::aspectRatio()const
	{
		return static_cast<float>(m_width) / static_cast<float>(m_height);
	}

}

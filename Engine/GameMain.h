#pragma once
#include "MemoryCommon.h"

#ifdef _DEBUG
#include <memory>
#endif

#include "Window.h"
#include "Application.h"

template<typename GameType>
inline int gameMain(HINSTANCE hInstance, LPSTR, int, const std::string& windowName)
{
#ifdef _DEBUG
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif	

	using namespace ArkanoidEngine;

	Window wnd{ hInstance, Window::sk_defaultWindowSize, Window::sk_defaultWindowSize, windowName };
#ifdef _DEBUG
	//allow crt to detect buffer overruns, etc..
	std::unique_ptr<Application> appPtr{ new Application{ wnd } };
	Application& app = *appPtr;
#else
	Application app{ wnd };
#endif
	return app.run<GameType>();
}
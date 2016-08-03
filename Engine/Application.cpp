#include "Application.h"
#include "Window.h"
#include <cassert>

using namespace ArkanoidEngine;

Application::Application(Window& window) 
	: m_window{window},	  
	  m_renderer{ window.windowHandle(), window.width(), window.height() }
{
	m_window.addWindowSizeEventsObserver(this);
}

Application::~Application()
{
	m_window.removeWindowSizeEventsObserver(this);
}

void Application::onResize()
{
	m_renderer.onResize(m_window.windowHandle(), m_window.width(), m_window.height());
	resume();
}




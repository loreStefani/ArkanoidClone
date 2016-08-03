#pragma once
#include "WindowsInclude.h" 
#include "WindowsPlatformCheck.h"

namespace ArkanoidEngine
{
	class WindowMessagesHandler
	{
	public:
		//dtor
		virtual ~WindowMessagesHandler() = default;

		//returns (LRESULT)0 if the message has been handled
		virtual LRESULT handleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) = 0;

	protected:
		//ctors		
		explicit WindowMessagesHandler() = default;
		
		//copy
		WindowMessagesHandler(const WindowMessagesHandler&) = default;
		WindowMessagesHandler& operator=(const WindowMessagesHandler&) = default;

		//move
		WindowMessagesHandler(WindowMessagesHandler&&) = default;
		WindowMessagesHandler& operator=(WindowMessagesHandler&&) = default;
	};
}

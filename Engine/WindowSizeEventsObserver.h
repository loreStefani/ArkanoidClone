#pragma once

namespace ArkanoidEngine
{
	class WindowSizeEventsObserver
	{
	public:
		//dtor
		virtual ~WindowSizeEventsObserver() = default;

		virtual void onResize() = 0;
		virtual void onMinimized() = 0;
		virtual void onMaximized() = 0;
		virtual void onStartResizing() = 0;
		virtual void onStopResizing() = 0;

	protected:
		//ctors
		explicit WindowSizeEventsObserver() = default;

		//copy
		WindowSizeEventsObserver(const WindowSizeEventsObserver&) = default;
		WindowSizeEventsObserver& operator=(const WindowSizeEventsObserver&) = default;

		//move
		WindowSizeEventsObserver(WindowSizeEventsObserver&&) = default;
		WindowSizeEventsObserver& operator=(WindowSizeEventsObserver&&) = default;
	};
}
#pragma once
#ifdef _WIN32
#include "WindowsInclude.h"
#endif
#include "WindowSizeEventsObserver.h"
#include "Renderer.h"
#include "Timer.h"
#include "PlatformUtils.h"

//#define SHOW_FRAME_STATS

namespace ArkanoidEngine
{
	class Window;

	class Application : public WindowSizeEventsObserver
	{
	public:
		//ctors
		explicit Application(Window& window);

		//dtor
		virtual ~Application();

		//copy
		Application(const Application&) = delete;
		Application& operator=(const Application&) = delete;

		//move
		Application(Application&&) = delete;
		Application& operator=(Application&&) = delete;

		template<typename GameType>
		int run();

		virtual void onResize() override;
		virtual void onMinimized()override;
		virtual void onMaximized()override;
		virtual void onStartResizing()override;
		virtual void onStopResizing()override;

		Renderer& renderer();
		const Renderer& renderer()const;

		Window& window();
		const Window& window()const;

		const Timer& timer()const;

	private:
		void pause();
		void resume();

		Renderer m_renderer;
		Window& m_window;
		Timer m_timer{};
		bool m_paused{ false };
	};

#ifdef _WIN32
	template<typename GameType>
	inline int Application::run()
	{
		GameType game{ *this };

		m_timer.start();

		MSG msg;

		while (true)
		{

			m_timer.tick();

			if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);

				if (msg.message == WM_QUIT)
				{
					break;
				}				
			}
			else
			{
								
				if (!m_paused)
				{

#ifdef SHOW_FRAME_STATS
					SetWindowText(window().windowHandle(),
								 (std::string{ "Frame time: " } +std::to_string(m_timer.deltaTime()) + std::string{ " ms" }).c_str());
#endif

					m_renderer.beginFrame();
					game.render();
					m_renderer.endFrame();
				}
			}
		}

		return static_cast<int>(msg.wParam);
	}
#else
#error UNSUPPORTED_PLATFORM_ERROR_MESSAGE
#endif

	inline void Application::pause()
	{
		if (!m_paused)
		{
			m_paused = true;
			m_timer.stop();
		}		
	}

	inline void Application::resume()
	{
		if (m_paused)
		{
			m_paused = false;
			m_timer.start();
		}		
	}

	inline void Application::onMinimized()
	{
		pause();
	}

	inline void Application::onMaximized()
	{
		resume();
	}

	inline void Application::onStartResizing()
	{
		pause();
	}

	inline void Application::onStopResizing()
	{
		resume();
	}

	inline Renderer& Application::renderer()
	{
		return m_renderer;
	}

	inline const Renderer& Application::renderer()const
	{
		return m_renderer;
	}

	inline Window& Application::window()
	{
		return m_window;
	}

	inline const Window& Application::window()const
	{
		return m_window;
	}

	inline const Timer& Application::timer()const
	{
		return m_timer;
	}

}
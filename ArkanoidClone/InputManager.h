#pragma once
#include "Engine.h"
#include "WindowMessagesHandler.h"

namespace ArkanoidEngine
{
	class Window;
}

namespace ArkanoidGame
{
	class ArkanoidLogic;

	class InputManager : public WindowMessagesHandler
	{
	public:
		//ctors
		explicit InputManager(Window& window, ArkanoidLogic& arkanoid);

		//dtor
		virtual ~InputManager();

		//copy
		InputManager(const InputManager&) = delete;
		InputManager& operator=(const InputManager&) = delete;

		//move
		InputManager(InputManager&&) = delete;
		InputManager& operator=(InputManager&&) = delete;

		virtual LRESULT handleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)override;
		
		bool isFireKeyPressed()const;
		bool isLeftKeyPressed()const;
		bool isRightKeyPressed()const;

	private:
		
		//keyboard events, returns true if consumes the event and false otherwise
		bool onKeyDown(WPARAM keyCode);
		bool onKeyUp(WPARAM keyCode);

		void onFireKeyDown();
		void onFireKeyUp();
		void onLeftKeyDown();
		void onLeftKeyUp();
		void onRightKeyDown();
		void onRightKeyUp();
		void onBrickShuffleKeyDown();
		void onBrickShuffleKeyUp();

		Window& m_window;
		ArkanoidLogic& m_arkanoid;
		bool m_firePressed{ false };
		bool m_leftPressed{ false };
		bool m_rightPressed{ false };
	};

	inline bool InputManager::isFireKeyPressed()const
	{
		return m_firePressed;
	}

	inline bool InputManager::isLeftKeyPressed()const
	{
		return m_leftPressed;
	}

	inline bool InputManager::isRightKeyPressed()const
	{
		return m_rightPressed;
	}
}

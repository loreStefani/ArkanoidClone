#include "InputManager.h"
#include "Window.h"
#include "ArkanoidLogic.h"

using namespace ArkanoidGame;
using namespace ArkanoidEngine;

InputManager::InputManager(Window& window, ArkanoidLogic& arkanoid) : m_window{ window }, m_arkanoid{ arkanoid }
{
	m_window.addWindowMessagesHandler(this);
}

InputManager::~InputManager()
{
	m_window.removeWindowMessagesHandler(this);
}

LRESULT InputManager::handleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_KEYDOWN:
		if (onKeyDown(wParam))
		{
			return 0;
		}
		break;
	case WM_KEYUP:
		if (onKeyUp(wParam))
		{
			return 0;
		}
		break;
	}

	return 1;
}

static constexpr WPARAM gk_leftKeyCode = VK_LEFT;
static constexpr WPARAM gk_rightKeyCode = VK_RIGHT;
static constexpr WPARAM gk_fireKeyCode = VK_CONTROL;
static constexpr WPARAM gk_brickShuffleKeyCode = 0x4E; //'n' key

bool InputManager::onKeyDown(WPARAM keyCode)
{
	switch (keyCode)
	{
	case gk_leftKeyCode:
		onLeftKeyDown();
		return true;
	case gk_rightKeyCode:
		onRightKeyDown();
		return true;
	case gk_fireKeyCode:
		onFireKeyDown();
		return true;
	case gk_brickShuffleKeyCode:
		onBrickShuffleKeyDown();
		return true;
	}

	return false;
}

bool InputManager::onKeyUp(WPARAM keyCode)
{
	switch (keyCode)
	{
	case gk_leftKeyCode:
		onLeftKeyUp();
		return true;
	case gk_rightKeyCode:
		onRightKeyUp();
		return true;
	case gk_fireKeyCode:
		onFireKeyUp();
		return true;
	case gk_brickShuffleKeyCode:
		onBrickShuffleKeyUp();
		return true;
	}

	return false;
}

void InputManager::onFireKeyDown()
{
	m_firePressed = true;
	m_arkanoid.onFireKeyDown();
}

void InputManager::onFireKeyUp()
{
	m_firePressed = false;
	m_arkanoid.onFireKeyUp();
}

void InputManager::onLeftKeyDown()
{
	m_leftPressed = true;
	m_arkanoid.onLeftKeyDown();
}

void InputManager::onLeftKeyUp()
{
	m_leftPressed = false;
	m_arkanoid.onLeftKeyUp();
}

void InputManager::onRightKeyDown()
{
	m_rightPressed = true;
	m_arkanoid.onRightKeyDown();
}

void InputManager::onRightKeyUp()
{
	m_rightPressed = false;
	m_arkanoid.onRightKeyUp();
}

void InputManager::onBrickShuffleKeyDown()
{
	m_arkanoid.onBrickShuffleKeyDown();
}

void InputManager::onBrickShuffleKeyUp()
{
	m_arkanoid.onBrickShuffleKeyUp();
}

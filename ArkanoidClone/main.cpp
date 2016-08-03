#include "GameMain.h"
#include "ArkanoidLogic.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	return gameMain<ArkanoidGame::ArkanoidLogic>(hInstance, lpCmdLine, nCmdShow, "ArkanoidClone");
}
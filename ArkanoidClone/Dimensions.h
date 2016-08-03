#pragma once

namespace ArkanoidGame
{
	constexpr unsigned int gk_entitiesCount = 5; //bricks + ball + player + arena + bonus
	constexpr unsigned int gk_bricksCount = 120;

	//1 ball + 1 player + 1 arena + 1 bonus + gk_bricksCount bricks
	constexpr unsigned int gk_instancesCount = (gk_entitiesCount - 1) + gk_bricksCount; 
}
#pragma once
#include "Engine.h"
#include "MathCommon.h"
#include <algorithm>

namespace ArkanoidGame
{
	constexpr float gk_epsilon = 0.000001f;
				
	inline bool approxEqualf(float a, float b)
	{
		return std::abs(a - b) <= gk_epsilon;
	}

	inline bool lessEqualf(float a, float b)
	{
		return a < b || approxEqualf(a, b);
	}
}
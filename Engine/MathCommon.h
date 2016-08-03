#pragma once
#include <DirectXMath.h>

namespace ArkanoidEngine
{
	using namespace DirectX;

	inline XMFLOAT2 operator+(const XMFLOAT2& v1, const XMFLOAT2& v2)
	{
		return XMFLOAT2{ v1.x + v2.x, v1.y + v2.y };
	}

	inline XMFLOAT2 operator*(const XMFLOAT2& v, float s)
	{
		return XMFLOAT2{ v.x * s, v.y * s };
	}

	inline XMFLOAT2 operator-(const XMFLOAT2& v1, const XMFLOAT2& v2)
	{
		return XMFLOAT2{ v1.x - v2.x, v1.y - v2.y };
	}
}
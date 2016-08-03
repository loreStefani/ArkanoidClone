#pragma once
#include "MathCommon.h"
#include <cassert>

namespace ArkanoidEngine
{
	inline void prepareMatrixForHLSL(XMFLOAT4X4* m)
	{
		assert(m != nullptr);
		XMMATRIX xmmatrix = XMLoadFloat4x4(m);
		XMStoreFloat4x4(m, XMMatrixTranspose(xmmatrix));
	}

}
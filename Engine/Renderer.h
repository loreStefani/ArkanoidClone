#pragma once
#ifdef _WIN32
#include "D3D11/Renderer.h"
#else
#error Unsupported Platform
#endif

namespace ArkanoidEngine
{
#ifdef _WIN32
	using Renderer = D3D11::Renderer;
#endif
}
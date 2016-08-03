#pragma once

struct ID3D11Device;
struct ID3D11DepthStencilState;

namespace ArkanoidEngine
{
	class DepthState;

	namespace D3D11
	{
		ID3D11DepthStencilState* createDepthStencilState(ID3D11Device* device, const DepthState& depthState);
	}
}
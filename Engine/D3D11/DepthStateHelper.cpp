#include "DepthStateHelper.h"
#include "D3DUtils.h"
#include <d3d11.h>
#include <cassert>
#include "../DepthState.h"

using namespace ArkanoidEngine;


ID3D11DepthStencilState* D3D11::createDepthStencilState(ID3D11Device* device, const DepthState& depthState)
{
	assert(device != nullptr);

	D3D11_DEPTH_STENCIL_DESC depthStencilDesc{};
	depthStencilDesc.DepthEnable = depthState.depthTestEnabled();
	depthStencilDesc.StencilEnable = FALSE;
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	

	ID3D11DepthStencilState* depthStencilState = nullptr;
	HRESULT_CHECK(device->CreateDepthStencilState(&depthStencilDesc, &depthStencilState));


	return depthStencilState;
}
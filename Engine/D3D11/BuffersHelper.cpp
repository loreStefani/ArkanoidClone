#include "BuffersHelper.h"
#include "D3DUtils.h"
#include <cassert>

using namespace ArkanoidEngine;

ID3D11Buffer* D3D11::createImmutableBuffer(ID3D11Device* device,
									UINT bindFlags, const void* bufferData, UINT bufferSize)
{
	assert(device != nullptr);

	D3D11_BUFFER_DESC bufferDesc{};
	bufferDesc.BindFlags = bindFlags;
	bufferDesc.ByteWidth = bufferSize;
	bufferDesc.Usage = D3D11_USAGE_IMMUTABLE;

	D3D11_SUBRESOURCE_DATA bufferSubresourceData{};
	bufferSubresourceData.pSysMem = bufferData;

	ID3D11Buffer* buffer = nullptr;
	HRESULT_CHECK(device->CreateBuffer(&bufferDesc, &bufferSubresourceData, &buffer));

	return buffer;
}
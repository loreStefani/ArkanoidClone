#include "InputLayoutHelper.h"
#include "D3DUtils.h"
#include <d3d11.h>
#include <vector>
#include <cassert>

using namespace ArkanoidEngine;

static D3D11_INPUT_ELEMENT_DESC createPositionInputElementDesc()
{
	D3D11_INPUT_ELEMENT_DESC inputElementDesc{};

	inputElementDesc.Format = DXGI_FORMAT_R32G32B32_FLOAT;
	inputElementDesc.SemanticName = "POSITION";
	inputElementDesc.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;

	return inputElementDesc;
}

static D3D11_INPUT_ELEMENT_DESC createNormalInputElementDesc()
{
	D3D11_INPUT_ELEMENT_DESC inputElementDesc{};

	inputElementDesc.Format = DXGI_FORMAT_R32G32B32_FLOAT;
	inputElementDesc.SemanticName = "NORMAL";
	inputElementDesc.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;

	return inputElementDesc;
}

static D3D11_INPUT_ELEMENT_DESC createTextCoordInputElementDesc()
{
	D3D11_INPUT_ELEMENT_DESC inputElementDesc{};
	
	inputElementDesc.Format = DXGI_FORMAT_R32G32_FLOAT;
	inputElementDesc.SemanticName = "TEXTCOORD";
	inputElementDesc.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;

	return inputElementDesc;
}

ID3D11InputLayout* D3D11::createInputLayout(ID3D11Device* device,
										    EVertexType vertexType,
										    ID3DBlob* compiledVertexShaderBlob)
{
	assert(device != nullptr);
	assert(compiledVertexShaderBlob != nullptr);

	std::vector<D3D11_INPUT_ELEMENT_DESC> inputElementsDescs{};
	inputElementsDescs.reserve(3);

	//all available vertex types have position
	inputElementsDescs.push_back(createPositionInputElementDesc());
	
	switch (vertexType)
	{
	case EVertexType::POSITION:
		break;
	case EVertexType::POSITION_NORMAL:
		inputElementsDescs.push_back(createNormalInputElementDesc());
		inputElementsDescs[1].AlignedByteOffset = 3 * sizeof(float);
		break;
	case EVertexType::POSITION_TEXTCOORD:
		inputElementsDescs.push_back(createTextCoordInputElementDesc());
		inputElementsDescs[1].AlignedByteOffset = 3 * sizeof(float);
		break;
	case EVertexType::POSITION_NORMAL_TEXTCOORD:
		inputElementsDescs.push_back(createNormalInputElementDesc());
		inputElementsDescs[1].AlignedByteOffset = 3 * sizeof(float);
		inputElementsDescs.push_back(createTextCoordInputElementDesc());
		inputElementsDescs[2].AlignedByteOffset = 6 * sizeof(float);
		break;
	default:
		assert(false); //invalid EVertexType value
	}
		
	ID3D11InputLayout* inputLayout = nullptr;

	HRESULT_CHECK(device->CreateInputLayout(inputElementsDescs.data(), static_cast<UINT>(inputElementsDescs.size()),
											compiledVertexShaderBlob->GetBufferPointer(),
											compiledVertexShaderBlob->GetBufferSize(),
											&inputLayout));
	
	return inputLayout;
}
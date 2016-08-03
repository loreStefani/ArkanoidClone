#pragma once
#include "../VertexTypes.h"

struct ID3D11InputLayout;
struct ID3D11Device;
struct ID3D10Blob;
using ID3DBlob = ID3D10Blob;

namespace ArkanoidEngine
{
	namespace D3D11
	{
		ID3D11InputLayout* createInputLayout(ID3D11Device* device,
											 EVertexType vertexType,
											 ID3DBlob* compiledVertexShaderBlob);
	}
}
#pragma once
#include <string>
#include <d3dcompiler.h>
#include "EShaderType.h"

namespace ArkanoidEngine
{
	class ShaderCompilationConfig;

	namespace D3D11
	{		
		//shaderCompilationConfig is optional
		ID3DBlob* compileShaderFromFile(EShaderType eShaderType, const std::string& shaderSourceFileName,
										const ShaderCompilationConfig* shaderCompilationConfig = nullptr);
	}
}
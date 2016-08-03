#include "ShaderUtils.h"
#include "../Utils.h"
#include <cassert>
#include "D3DUtils.h"
#include "../Resources.h"
#include <vector>
#include "../ShaderCompilationConfig.h"

using namespace ArkanoidEngine;

static std::vector<D3D_SHADER_MACRO> createDefines(const ShaderCompilationConfig* shaderCompilationConfig)
{
	if (shaderCompilationConfig == nullptr)
	{
		return std::vector<D3D_SHADER_MACRO>{D3D_SHADER_MACRO{ nullptr, nullptr }};
	}

	std::vector<D3D_SHADER_MACRO> defines{};
	defines.reserve(shaderCompilationConfig->definesCount() + 1);

	for (auto& define : *shaderCompilationConfig)
	{
		D3D_SHADER_MACRO shaderMacro;
		shaderMacro.Name = define.first.c_str();
		shaderMacro.Definition = define.second.c_str();

		defines.push_back(std::move(shaderMacro));
	}

	defines.push_back(D3D_SHADER_MACRO{nullptr, nullptr});

	return defines;
}


ID3DBlob* D3D11::compileShaderFromFile(EShaderType eShaderType, const std::string& shaderSourceFileName,
									   const ShaderCompilationConfig* shaderCompilationConfig)
{
	LPCSTR target = nullptr;
	switch (eShaderType)
	{
	case EShaderType::VERTEX:
		target = "vs_5_0";
		break;
	case EShaderType::PIXEL:
		target = "ps_5_0";
		break;
	default:
		OutputDebugString("Invalid Shader type");
		return nullptr;
	}

#ifdef _DEBUG	
	UINT compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#else
	UINT compileFlags = D3DCOMPILE_OPTIMIZATION_LEVEL3;
#endif

	std::vector<D3D_SHADER_MACRO> defines = createDefines(shaderCompilationConfig);

	std::string filePath = gk_shadersPath + shaderSourceFileName;

	ID3DBlob* compiledShader = nullptr;
	ID3DBlob* error = nullptr;

	HRESULT hr = D3DCompileFromFile(Utils::makeWideString(filePath).c_str(),
					   defines.data(),
					   nullptr,
					   "main",
					   target,
					   compileFlags, 0,
					   &compiledShader, &error);

	if (FAILED(hr))
	{
		std::string errorMessage{"D3DCompileFromFile failed: "};
		if (error != nullptr)
		{
			errorMessage += std::string{ reinterpret_cast<const char*>(error->GetBufferPointer()),error->GetBufferSize() };
			COM_RELEASE(error);
		}
		else
		{
			errorMessage += _com_error{ hr }.ErrorMessage();
		}

		OutputDebugString(errorMessage.c_str());
		
		return nullptr;
	}
	else
	{
		COM_RELEASE(error);
		return compiledShader;
	}
}
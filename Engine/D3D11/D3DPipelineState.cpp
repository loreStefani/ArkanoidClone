#include "D3DPipelineState.h"
#include <d3d11.h>
#include <cassert>
#include "D3DUtils.h"

using namespace ArkanoidEngine::D3D11;

D3DPipelineState::D3DPipelineState(ID3D11VertexShader* vertexShader, ID3D11InputLayout* inputLayout,
								   ID3D11PixelShader* pixelShader,
								   std::vector<ID3D11Buffer*> vertexShaderConstantBuffers,
								   std::vector<ID3D11Buffer*> pixelShaderConstantBuffers,
								   ID3D11DepthStencilState* depthStencilState)
	: m_vertexShader{ vertexShader }, m_inputLayout{ inputLayout }, m_pixelShader{ pixelShader },
	  m_vertexShaderConstantBuffers{ vertexShaderConstantBuffers }, m_pixelShaderConstantBuffers{ pixelShaderConstantBuffers },
	  m_depthStencilState{ depthStencilState }
{
	assert(m_vertexShader != nullptr);
	assert(m_inputLayout != nullptr);
	assert(m_pixelShader != nullptr);
	assert(m_depthStencilState != nullptr);

	m_vertexShader->AddRef();
	m_inputLayout->AddRef();
	m_pixelShader->AddRef();
	m_depthStencilState->AddRef();

	auto addRefToConstantBuffers = [](const std::vector<ID3D11Buffer*>& constantBuffers)
	{
		for (auto constantBuffer : constantBuffers)
		{
			assert(constantBuffer != nullptr);
			constantBuffer->AddRef();
		}
	};

	addRefToConstantBuffers(m_vertexShaderConstantBuffers);
	addRefToConstantBuffers(m_pixelShaderConstantBuffers);	
}

D3DPipelineState::~D3DPipelineState()
{
	release();
}

D3DPipelineState::D3DPipelineState(D3DPipelineState&& other)
	: m_vertexShader{ other.m_vertexShader }, 
	  m_inputLayout{ other.m_inputLayout },
	  m_pixelShader{ other.m_pixelShader },
	  m_vertexShaderConstantBuffers{std::move(other.m_vertexShaderConstantBuffers)},
	  m_pixelShaderConstantBuffers{std::move(other.m_pixelShaderConstantBuffers)},
	  m_depthStencilState{ other.m_depthStencilState }
{
	other.m_vertexShader = nullptr;
	other.m_inputLayout = nullptr;
	other.m_pixelShader = nullptr;
	other.m_depthStencilState = nullptr;
}

inline D3DPipelineState& D3DPipelineState::operator=(D3DPipelineState&& rhs)
{
	if (&rhs == this)
	{
		return *this;
	}
	
	release();

	m_vertexShader = rhs.m_vertexShader;
	m_inputLayout = rhs.m_inputLayout;
	m_pixelShader = rhs.m_pixelShader;
	m_vertexShaderConstantBuffers = std::move(rhs.m_vertexShaderConstantBuffers);
	m_pixelShaderConstantBuffers = std::move(rhs.m_pixelShaderConstantBuffers);
	m_depthStencilState = rhs.m_depthStencilState;

	rhs.m_vertexShader = nullptr;
	rhs.m_inputLayout = nullptr;
	rhs.m_pixelShader = nullptr;
	rhs.m_depthStencilState = nullptr;

	return *this;
}

void D3DPipelineState::release()
{
	COM_RELEASE(m_vertexShader);
	COM_RELEASE(m_inputLayout);
	COM_RELEASE(m_pixelShader);
	
	auto releaseConstantBuffers = [](const std::vector<ID3D11Buffer*>& constantBuffers)
	{
		for (auto constantBuffer : constantBuffers)
		{
			COM_RELEASE(constantBuffer);
		}
	};

	releaseConstantBuffers(m_vertexShaderConstantBuffers);
	releaseConstantBuffers(m_pixelShaderConstantBuffers);

	COM_RELEASE(m_depthStencilState);
}

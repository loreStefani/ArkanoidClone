#include "PipelineState.h"
#include <cassert>

using namespace ArkanoidEngine;

PipelineState::PipelineState(IDType vertexShaderID, 
							 IDType pixelShaderID,
							 const std::vector<IDType>& vertexShaderStageConstantBuffers,
							 const std::vector<IDType>& pixelShaderStageConstantBuffers,
							 const DepthState& depthState)
	: m_vertexShaderID{ vertexShaderID },
	  m_pixelShaderID{ pixelShaderID },
	  m_vertexShaderStageConstantBuffers{ vertexShaderStageConstantBuffers },
	  m_pixelShaderStageConstantBuffers{ pixelShaderStageConstantBuffers },
	  m_depthState{ depthState }
{
}

void PipelineState::setStageConstantBuffers(EPipelineStage ePipelineStage, const std::vector<IDType>& constantBuffersIDs)
{
	switch (ePipelineStage)
	{
	case EPipelineStage::VERTEX_SHADER:
		m_vertexShaderStageConstantBuffers = constantBuffersIDs;
		break;
	case EPipelineStage::PIXEL_SHADER:
		m_pixelShaderStageConstantBuffers = constantBuffersIDs;
		break;
	default:
		assert(false);
	}
}

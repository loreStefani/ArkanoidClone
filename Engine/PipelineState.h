#pragma once
#include "IDType.h"
#include <vector>
#include "EPipelineStage.h"
#include "DepthState.h"

namespace ArkanoidEngine
{	
	class PipelineState
	{
	public:
		//ctors
		explicit PipelineState(IDType vertexShaderID = IDType{},
							   IDType pixelShaderID = IDType{},
							   const std::vector<IDType>& vertexShaderStageConstantBuffers = {},
							   const std::vector<IDType>& pixelShaderStageConstantBuffers = {},
							   const DepthState& depthState = DepthState{});
		
		//dtor
		~PipelineState() = default;

		//copy
		PipelineState(const PipelineState&) = default;
		PipelineState& operator=(const PipelineState&) = default;

		//move
		PipelineState(PipelineState&&) = default;
		PipelineState& operator=(PipelineState&&) = default;

		IDType vertexShaderID()const;
		IDType pixelShaderID()const;

		void setVertexShaderID(IDType vertexShaderID);
		void setPixelShaderID(IDType pixelShaderID);

		const std::vector<IDType>& vertexShaderStageConstantBuffers()const;
		const std::vector<IDType>& pixelShaderStageConstantBuffers()const;

		void setStageConstantBuffers(EPipelineStage ePipelineStage, const std::vector<IDType>& constantBuffersIDs);
		
		const DepthState& depthState()const;
		void setDepthState(const DepthState& depthState);

	private:
		std::vector<IDType> m_vertexShaderStageConstantBuffers;
		std::vector<IDType> m_pixelShaderStageConstantBuffers;
		IDType m_vertexShaderID;
		IDType m_pixelShaderID;
		DepthState m_depthState;
	};
	
	inline IDType PipelineState::vertexShaderID()const
	{
		return m_vertexShaderID;
	}

	inline IDType PipelineState::pixelShaderID()const
	{
		return m_pixelShaderID;
	}

	inline void PipelineState::setVertexShaderID(IDType vertexShaderID)
	{
		m_vertexShaderID = vertexShaderID;
	}

	inline void PipelineState::setPixelShaderID(IDType pixelShaderID)
	{
		m_pixelShaderID = pixelShaderID;
	}

	inline const std::vector<IDType>& PipelineState::vertexShaderStageConstantBuffers()const
	{
		return m_vertexShaderStageConstantBuffers;
	}

	inline const std::vector<IDType>& PipelineState::pixelShaderStageConstantBuffers()const
	{
		return m_pixelShaderStageConstantBuffers;
	}

	inline const DepthState& PipelineState::depthState()const
	{
		return m_depthState;
	}

	inline void PipelineState::setDepthState(const DepthState& depthState)
	{
		m_depthState = depthState;
	}
}
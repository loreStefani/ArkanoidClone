#pragma once
#include "IDType.h"

namespace ArkanoidEngine
{
	class PipelineStateData
	{
	public:
		//ctors
		explicit PipelineStateData(IDType texture2DID = IDType{});

		//dtor
		~PipelineStateData() = default;

		//copy
		PipelineStateData(const PipelineStateData&) = default;
		PipelineStateData& operator=(const PipelineStateData&) = default;

		//move
		PipelineStateData(PipelineStateData&&) = default;
		PipelineStateData& operator=(PipelineStateData&&) = default;

		void setTexture2DID(IDType texture2DID);

		IDType texture2DID()const;
		
	private:
		IDType m_texture2DID;
	};

	inline PipelineStateData::PipelineStateData(IDType texture2DID)
		: m_texture2DID{ texture2DID }
	{
	}

	inline IDType PipelineStateData::texture2DID()const
	{
		return m_texture2DID;
	}

	inline void PipelineStateData::setTexture2DID(IDType texture2DID)
	{
		m_texture2DID = texture2DID;
	}
}

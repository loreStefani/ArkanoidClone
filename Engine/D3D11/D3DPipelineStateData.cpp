#include "D3DPipelineStateData.h"
#include <d3d11.h>
#include "D3DUtils.h"
#include <cassert>

using namespace ArkanoidEngine::D3D11;

D3DPipelineStateData::D3DPipelineStateData(ID3D11ShaderResourceView* texture2DSRV)
	: m_texture2DSRV{ texture2DSRV }
{
	assert(m_texture2DSRV != nullptr);	
	m_texture2DSRV->AddRef();
}

D3DPipelineStateData::~D3DPipelineStateData()
{
	COM_RELEASE(m_texture2DSRV);
}

D3DPipelineStateData::D3DPipelineStateData(D3DPipelineStateData&& other)
	: m_texture2DSRV{ other.m_texture2DSRV }
{
	other.m_texture2DSRV = nullptr;
}

D3DPipelineStateData& D3DPipelineStateData::operator=(D3DPipelineStateData&& rhs)
{
	if (&rhs == this)
	{
		return *this;
	}

	COM_RELEASE(m_texture2DSRV);

	m_texture2DSRV = rhs.m_texture2DSRV;
	rhs.m_texture2DSRV = nullptr;

	return *this;
}
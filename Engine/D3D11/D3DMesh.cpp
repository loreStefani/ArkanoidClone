#include "D3DMesh.h"
#include <cassert>
#include <d3d11.h>
#include "D3DUtils.h"
#include <utility>

using namespace ArkanoidEngine::D3D11;


D3DMesh::D3DMesh(ID3D11Buffer* vertexBuffer,
				 ID3D11Buffer* indexBuffer,
				 UINT verticesCount, UINT vertexStride, UINT indicesCount, DXGI_FORMAT indexFormat)
	: m_vertexBuffer{ vertexBuffer }, m_indexBuffer{ indexBuffer },
	  m_verticesCount{ verticesCount }, m_vertexStride{ vertexStride },
	  m_indicesCount{ indicesCount },	m_indexFormat{ indexFormat }
{
	assert(m_vertexBuffer != nullptr);
	assert(m_indexBuffer != nullptr);
	assert(m_verticesCount > 0 && m_vertexStride > 0);
	assert(m_indicesCount > 0);
	
	m_vertexBuffer->AddRef();
	m_indexBuffer->AddRef();
}

D3DMesh::~D3DMesh()
{
	release();
}

D3DMesh::D3DMesh(D3DMesh&& other)
{
	move(std::move(other));
}

D3DMesh& D3DMesh::operator=(D3DMesh&& rhs)
{
	if (&rhs == this)
	{
		return *this;
	}

	release();

	move(std::move(rhs));
	
	return *this;
}

void D3DMesh::move(D3DMesh&& other)
{
	m_vertexBuffer = other.m_vertexBuffer;
	m_indexBuffer = other.m_indexBuffer;
	m_verticesCount = other.m_verticesCount;
	m_vertexStride = other.m_vertexStride;
	m_indicesCount = other.m_indicesCount;
	m_indexFormat = other.m_indexFormat;

	other.m_vertexBuffer = nullptr;
	other.m_indexBuffer = nullptr;
	other.m_verticesCount = 0;
	other.m_vertexStride = 0;
	other.m_indicesCount = 0;
	other.m_indexFormat = DXGI_FORMAT_UNKNOWN;
}

void D3DMesh::release()
{
	COM_RELEASE(m_vertexBuffer);
	COM_RELEASE(m_indexBuffer);
}
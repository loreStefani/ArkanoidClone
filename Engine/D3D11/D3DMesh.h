#pragma once
#include "../WindowsInclude.h"
#include <dxgiformat.h>

struct ID3D11Buffer;

namespace ArkanoidEngine
{
	namespace D3D11
	{
		class D3DMesh
		{
		public:
			//ctors
			explicit D3DMesh(ID3D11Buffer* vertexBuffer, ID3D11Buffer* indexBuffer,
							 UINT verticesCount, UINT vertexStride, UINT indicesCount, DXGI_FORMAT indexFormat);

			//dtor
			~D3DMesh();

			//copy
			D3DMesh(const D3DMesh&) = delete;
			D3DMesh& operator=(const D3DMesh&) = delete;

			//move
			D3DMesh(D3DMesh&&);
			D3DMesh& operator=(D3DMesh&&);

			UINT verticesCount()const;
			UINT vertexStride()const;

			UINT indicesCount()const;

			DXGI_FORMAT indexFormat()const;
			
			ID3D11Buffer* vertexBuffer()const;
			ID3D11Buffer* indexBuffer()const;			

		private:

			void move(D3DMesh&& other);
			void release();

			ID3D11Buffer* m_vertexBuffer{ nullptr };
			ID3D11Buffer* m_indexBuffer{ nullptr };
			UINT m_verticesCount{ 0 };
			UINT m_vertexStride{ 0 };
			UINT m_indicesCount{ 0 };
			DXGI_FORMAT m_indexFormat{ DXGI_FORMAT_UNKNOWN };
		};
						
		inline UINT D3DMesh::verticesCount()const
		{
			return m_verticesCount;
		}

		inline UINT D3DMesh::vertexStride()const
		{
			return m_vertexStride;
		}
		
		inline UINT D3DMesh::indicesCount()const
		{
			return m_indicesCount;
		}

		inline DXGI_FORMAT D3DMesh::indexFormat()const
		{
			return m_indexFormat;
		}

		inline ID3D11Buffer* D3DMesh::vertexBuffer()const
		{
			return m_vertexBuffer;
		}

		inline ID3D11Buffer* D3DMesh::indexBuffer()const
		{
			return m_indexBuffer;
		}
	}
}
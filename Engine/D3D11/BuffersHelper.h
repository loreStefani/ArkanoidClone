#pragma once
#include "../WindowsInclude.h"
#include <vector>
#include "../VertexTypes.h"
#include <d3d11.h>
#include "../Mesh.h"
#include <cassert>

namespace ArkanoidEngine
{
	namespace D3D11
	{
		ID3D11Buffer* createImmutableBuffer(ID3D11Device* device,
											UINT bindFlags, const void* bufferData, UINT bufferSize);

		struct VertexBufferDesc
		{
			ID3D11Buffer* vertexBuffer;
			UINT verticesCount;
			UINT vertexStride;
		};

		template<typename VertexType>
		inline VertexBufferDesc createVertexBuffer(ID3D11Device* device, const Mesh& mesh)
		{
			assert(device != nullptr);

			const std::vector<XMFLOAT3>& meshPositions = mesh.getPositions();
			const std::vector<XMFLOAT3>& meshNormals = mesh.getNormals();
			const std::vector<XMFLOAT2>& meshTextCoords = mesh.getTextCoords();

			const UINT verticesCount = static_cast<UINT>(mesh.vertexCount());

			std::vector<VertexType> vertices(verticesCount);

			fillVertexBuffer<VertexType>(vertices, verticesCount, meshPositions, meshNormals, meshTextCoords);

			constexpr UINT vertexStride = sizeof(VertexType);

			ID3D11Buffer* vertexBuffer = createImmutableBuffer(device,
															   D3D11_BIND_VERTEX_BUFFER,
															   vertices.data(), vertexStride*verticesCount);

			VertexBufferDesc vertexBufferDesc;
			vertexBufferDesc.vertexBuffer = vertexBuffer;
			vertexBufferDesc.vertexStride = vertexStride;
			vertexBufferDesc.verticesCount = verticesCount;

			return vertexBufferDesc;
		}

		struct IndexBufferDesc
		{
			ID3D11Buffer* indexBuffer;
			UINT indicesCount;
		};

		template<typename IndexType>
		inline IndexBufferDesc createIndexBuffer(ID3D11Device* device, const Mesh& mesh)
		{
			assert(device != nullptr);

			const UINT indicesCount = static_cast<UINT>(mesh.indexCount());

			std::vector<IndexType> indices(indicesCount);

			const std::vector<uint64_t>& meshIndices = mesh.getIndices();

			for (UINT index = 0; index < indicesCount; ++index)
			{
				indices[index] = static_cast<IndexType>(meshIndices[index]);
			}

			ID3D11Buffer* indexBuffer = createImmutableBuffer(device,
															  D3D11_BIND_INDEX_BUFFER,
															  indices.data(),
															  sizeof(IndexType) * indicesCount);

			IndexBufferDesc indexBufferDesc;
			indexBufferDesc.indexBuffer = indexBuffer;
			indexBufferDesc.indicesCount = indicesCount;

			return indexBufferDesc;
		}
	}
}
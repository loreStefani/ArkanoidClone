#pragma once
#include "MathCommon.h"
#include <vector>
#include "IDType.h"

namespace ArkanoidEngine
{
	enum class EVertexType : char
	{
		POSITION,
		POSITION_NORMAL,
		POSITION_TEXTCOORD,
		POSITION_NORMAL_TEXTCOORD
	};

	struct VertexPos
	{
		XMFLOAT3 position;
	};

	struct VertexPosNorm : VertexPos
	{
		XMFLOAT3 normal;
	};

	struct VertexPosTextCoord : VertexPos
	{
		XMFLOAT2 textCoord;
	};

	struct VertexPosNormTextCoord : VertexPosNorm
	{
		XMFLOAT2 textCoord;
	};

	//general definition not provided on purpose
	template<typename VertexType>
	inline void fillVertexBuffer(std::vector<VertexType>& vertices,
								 size_t verticesCount,
								 const std::vector<XMFLOAT3>& positions,
								 const std::vector<XMFLOAT3>& normals,
								 const std::vector<XMFLOAT2>& textCoords);	

	template<>
	inline void fillVertexBuffer<VertexPos>(std::vector<VertexPos>& vertices,
											size_t verticesCount,
											const std::vector<XMFLOAT3>& positions,
											const std::vector<XMFLOAT3>& normals,
											const std::vector<XMFLOAT2>& textCoords)
	{
		for (size_t vertexIndex = 0; vertexIndex < verticesCount; ++vertexIndex)
		{
			vertices[vertexIndex].position = positions[vertexIndex];
		}
	}

	template<>
	inline void fillVertexBuffer<VertexPosNorm>(std::vector<VertexPosNorm>& vertices,
												size_t verticesCount,
												const std::vector<XMFLOAT3>& positions,
												const std::vector<XMFLOAT3>& normals,
												const std::vector<XMFLOAT2>& textCoords)
	{
		for (size_t vertexIndex = 0; vertexIndex < verticesCount; ++vertexIndex)
		{
			vertices[vertexIndex].position = positions[vertexIndex];
			vertices[vertexIndex].normal = normals[vertexIndex];
		}
	}

	template<>
	inline void fillVertexBuffer<VertexPosTextCoord>(std::vector<VertexPosTextCoord>& vertices,
													 size_t verticesCount,
													 const std::vector<XMFLOAT3>& positions,
													 const std::vector<XMFLOAT3>& normals,
													 const std::vector<XMFLOAT2>& textCoords)
	{
		for (size_t vertexIndex = 0; vertexIndex < verticesCount; ++vertexIndex)
		{
			vertices[vertexIndex].position = positions[vertexIndex];
			vertices[vertexIndex].textCoord = textCoords[vertexIndex];
		}
	}

	template<>
	inline void fillVertexBuffer<VertexPosNormTextCoord>(std::vector<VertexPosNormTextCoord>& vertices,
														 size_t verticesCount,
														 const std::vector<XMFLOAT3>& positions,
														 const std::vector<XMFLOAT3>& normals,
														 const std::vector<XMFLOAT2>& textCoords)
	{
		for (size_t vertexIndex = 0; vertexIndex < verticesCount; ++vertexIndex)
		{
			vertices[vertexIndex].position = positions[vertexIndex];
			vertices[vertexIndex].normal = normals[vertexIndex];
			vertices[vertexIndex].textCoord = textCoords[vertexIndex];
		}
	}

	IDType getVertexTypeID(EVertexType vertexType);
}
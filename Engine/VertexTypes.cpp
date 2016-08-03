#include "VertexTypes.h"
#include <cassert>

using namespace ArkanoidEngine;

IDType ArkanoidEngine::getVertexTypeID(EVertexType vertexType)
{
	switch (vertexType)
	{
	case EVertexType::POSITION:
		return IDType{ 0 };
	case EVertexType::POSITION_NORMAL:
		return IDType{ 1 };
	case EVertexType::POSITION_TEXTCOORD:
		return IDType{ 2 };
	case EVertexType::POSITION_NORMAL_TEXTCOORD:
		return IDType{ 3 };
	default:
		assert(false);
		return IDType{};
	}
}

#pragma once
#include "MathHelper.h"

namespace ArkanoidGame
{
	inline constexpr unsigned int uiPow(unsigned int base, unsigned int exponent)
	{
		return exponent > 0 ? base*uiPow(base, exponent - 1) : 1;
	}

	inline constexpr unsigned int quadrantsCount(unsigned int depth)
	{
		return (uiPow(4, depth + 1) - 1) / (4 - 1);
	}

	inline void computeChildrenIndices(unsigned int quadrantIndex, unsigned int(&childrenIndices)[4], unsigned int childrenOffset)
	{
		childrenIndices[0] = quadrantIndex + 1;
		for (unsigned int i = 1; i < 4; ++i)
		{
			childrenIndices[i] = childrenIndices[i - 1] + childrenOffset;
		}
	}

	inline unsigned int computeSiblingsOffset(unsigned int depth, unsigned int maxDepth)
	{
		unsigned int siblingOffset = 0;
		for (unsigned int d = depth; d <= maxDepth; ++d)
		{
			siblingOffset += uiPow(4, maxDepth - d);
		}
		return siblingOffset;
	}	
}
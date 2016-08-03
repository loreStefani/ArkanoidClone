#include "AABB.h"
#include <algorithm>
#include "MathHelper.h"

using namespace ArkanoidGame;

AABB AABB::computeFromPoints(const std::vector<XMFLOAT3>& points)
{
	if (points.size() == 0)
	{
		return AABB{};
	}
	
	XMVECTOR xmMin = XMLoadFloat3(&points[0]);
	XMVECTOR xmMax = XMLoadFloat3(&points[0]);

	for (auto& point : points)
	{
		XMVECTOR xmpoint = XMLoadFloat3(&point);
		xmMin = XMVectorMin(xmMin, xmpoint);
		xmMax = XMVectorMax(xmMax, xmpoint);
	}
	
	XMFLOAT2 min;
	XMFLOAT2 max;
	XMStoreFloat2(&min, xmMin);
	XMStoreFloat2(&max, xmMax);
	return AABB::computeFromMinMax(min, max);
}


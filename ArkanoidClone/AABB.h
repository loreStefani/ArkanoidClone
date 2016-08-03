#pragma once
#include "Engine.h"
#include "MathCommon.h"
#include <vector>
#include "MathHelper.h"
#include <cassert>

namespace ArkanoidGame
{
	//axis-aligned 2d bounding box defined on the xy plane
	class AABB
	{
	public:
		//ctors
		explicit AABB() = default;

		//dtor
		~AABB() = default;

		//copy
		AABB(const AABB&) = default;
		AABB& operator=(const AABB&) = default;

		//move
		AABB(AABB&&) = default;
		AABB& operator=(AABB&&) = default;

		static AABB computeFromPoints(const std::vector<XMFLOAT3>& points);
		static AABB computeFromMinMax(const XMFLOAT2& min, const XMFLOAT2& max);
		static AABB computeFromCenterAndHalfExtents(const XMFLOAT2& center, const XMFLOAT2& halfExtents);

		XMFLOAT2& center();
		const XMFLOAT2& center()const;

		XMFLOAT2& halfExtents();
		const XMFLOAT2& halfExtents()const;

		XMFLOAT2 min()const;
		XMFLOAT2 max()const;

		bool intersects(const AABB&)const;

	private:
		//ctors
		explicit AABB(const XMFLOAT2& center, const XMFLOAT2& halfExtents);

		XMFLOAT2 m_center{0.0f, 0.0f};
		XMFLOAT2 m_halfExtents{0.0f, 0.0f};		
	};

	inline AABB AABB::computeFromMinMax(const XMFLOAT2& min, const XMFLOAT2& max)
	{
		assert(lessEqualf(min.x, max.x) && lessEqualf(min.y, max.y));

		const XMFLOAT2 halfExtents = (max - min)*0.5f;
		const XMFLOAT2 center = min + halfExtents;

		return AABB::computeFromCenterAndHalfExtents(center, halfExtents);
	}

	inline AABB AABB::computeFromCenterAndHalfExtents(const XMFLOAT2& center, const XMFLOAT2& halfExtents)
	{
		assert(halfExtents.x > 0.0f && halfExtents.y > 0.0f);
		return AABB{ center, halfExtents };
	}

	inline AABB::AABB(const XMFLOAT2& center, const XMFLOAT2& halfExtents)
		: m_center{ center }, m_halfExtents{ halfExtents }
	{
	}

	inline XMFLOAT2& AABB::center()
	{
		return m_center;
	}

	inline const XMFLOAT2& AABB::center()const
	{
		return m_center;
	}

	inline XMFLOAT2& AABB::halfExtents()
	{
		return m_halfExtents;
	}

	inline const XMFLOAT2& AABB::halfExtents()const
	{
		return m_halfExtents;
	}

	inline XMFLOAT2 AABB::min() const
	{
		return m_center - m_halfExtents;
	}

	inline XMFLOAT2 AABB::max() const
	{
		return m_center + m_halfExtents;
	}

	inline bool AABB::intersects(const AABB& aabb)const
	{
		const bool xIntersects = lessEqualf(std::abs(m_center.x - aabb.m_center.x), (m_halfExtents.x + aabb.m_halfExtents.x));
		const bool yIntersects = lessEqualf(std::abs(m_center.y - aabb.m_center.y), (m_halfExtents.y + aabb.m_halfExtents.y));

		const unsigned int xIntersectsInteger = static_cast<unsigned int>(xIntersects);
		const unsigned int yIntersectsInteger = static_cast<unsigned int>(yIntersects);
		
		return (xIntersectsInteger & yIntersectsInteger) == 1; //& avoids the branch caused by short circuiting	
	}
}

#pragma once
#include "MathHelper.h"

namespace ArkanoidGame
{
	class Quadrant
	{
	public:
		//ctors		
		explicit Quadrant() = default;

		//dtor
		~Quadrant() = default;

		//copy
		Quadrant(const Quadrant&) = default;
		Quadrant& operator=(const Quadrant&) = default;

		//move
		Quadrant(Quadrant&&) = default;
		Quadrant& operator=(Quadrant&&) = default;

		const XMFLOAT2& min()const;
		XMFLOAT2& min();

		bool contains(const XMFLOAT2& aabbMin, const XMFLOAT2& aabbMax, const XMFLOAT2& quadrantSize)const;
		bool outside(const XMFLOAT2& aabbMin, const XMFLOAT2& aabbMax, const XMFLOAT2& quadrantSize)const;

	private:
		XMFLOAT2 m_min;
	};

	inline const XMFLOAT2& Quadrant::min()const
	{
		return m_min;
	}

	inline XMFLOAT2& Quadrant::min()
	{
		return m_min;
	}

	inline bool Quadrant::contains(const XMFLOAT2& aabbMin, const XMFLOAT2& aabbMax, const XMFLOAT2& quadrantSize)const
	{
		const XMFLOAT2 max = m_min + quadrantSize;

		const unsigned int containsXLeft = static_cast<unsigned int>(lessEqualf(m_min.x, aabbMin.x));
		const unsigned int containsXRight = static_cast<unsigned int>(lessEqualf(aabbMax.x, max.x));
		const unsigned int containsX = containsXLeft & containsXRight;

		const unsigned int containsYBottom = static_cast<unsigned int>(lessEqualf(m_min.y, aabbMin.y));
		const unsigned int containsYTop = static_cast<unsigned int>(lessEqualf(aabbMax.y, max.y));
		const unsigned int containsY = containsYBottom & containsYTop;

		return (containsX & containsY) == 1;
	}

	inline bool Quadrant::outside(const XMFLOAT2& aabbMin,
								  const XMFLOAT2& aabbMax,
								  const XMFLOAT2& quadrantSize)const
	{
		XMFLOAT2 max = m_min + quadrantSize;

		const unsigned int outsideXLeft = static_cast<unsigned int>(aabbMax.x < m_min.x);
		const unsigned int outsideXRight = static_cast<unsigned int>(max.x < aabbMin.x);
		const unsigned int outsideX = outsideXLeft | outsideXRight;

		const unsigned int outsideYBottom = static_cast<unsigned int>(aabbMax.y < m_min.y);
		const unsigned int outsideYTop = static_cast<unsigned int>(max.y < aabbMin.y);
		const unsigned int outsideY = outsideYBottom | outsideYTop;
		
		return (outsideX | outsideY) == 1;
	}

}
#pragma once
#include <vector>
#include <cassert>
#include "QuadtreeHelper.h"
#include "Quadrant.h"
#include "MathHelper.h"
#include "AABB.h"

namespace ArkanoidGame
{
	struct DefaultSubdivisionPolicy
	{
		static bool shouldSubdivideQuadrant(unsigned int pointsCount)
		{
			return pointsCount > 0;
		}
	};
	
	template<typename ObjectData = unsigned int, unsigned int MAX_DEPTH = 1, typename SubdivisionPolicy = DefaultSubdivisionPolicy>
	class Quadtree : public SubdivisionPolicy
	{
	public:
		//ctors
		explicit Quadtree(const AABB& quadtreeArea, const XMFLOAT2& objectsHalfExtents);

		//dtor
		~Quadtree() = default;

		//copy
		Quadtree(const Quadtree&) = default;
		Quadtree& operator=(const Quadtree&) = default;

		//move
		Quadtree(Quadtree&&) = default;
		Quadtree& operator=(Quadtree&&) = default;
		
		void insert(const XMFLOAT2& objectCenter, const ObjectData& objectData);

		//foundObjects must point to an array of ObjectData which size is enough to contain all the objects
		//the number of objects actually found is returned
		unsigned int findPotentialColliders(const AABB& objectAABB, ObjectData* foundObjects)const;
		
	private:		
		void buildQuadrant(unsigned int quadrantIndex, unsigned int currDepth);

		bool shouldSubdivide(unsigned int quadrantIndex)const;
		void subdivide(unsigned int quadrantIndex, unsigned int currDepth);

		Quadrant& quadrant(unsigned int quadrantIndex);
		const Quadrant& quadrant(unsigned int quadrantIndex)const;

		std::vector<XMFLOAT2>& quadrantObjectsCenters(unsigned int quadrantIndex);
		const std::vector<XMFLOAT2>& quadrantObjectsCenters(unsigned int quadrantIndex)const;

		std::vector<ObjectData>& quadrantObjectsDatas(unsigned int quadrantIndex);
		const std::vector<ObjectData>& quadrantObjectsDatas(unsigned int quadrantIndex)const;
		
		XMFLOAT2& perDepthQuadrantSize(unsigned int depth);
		const XMFLOAT2& perDepthQuadrantSize(unsigned int depth)const;
		
		unsigned int childrenOffsets(unsigned int depth)const;

		void setQuadrantDepth(unsigned int quadrantIndex, unsigned int quadrantDepth);
		unsigned int perQuadrantDepth(unsigned int quadrantIndex)const;

		void setQuadrantSubdivided(unsigned int quadrantIndex, bool quadrantSubdivided);				
		bool isQuadrantSubdivided(unsigned int quadrantIndex)const;
		

		static constexpr unsigned int sk_maxDepth = MAX_DEPTH;
		static constexpr unsigned int sk_maxQuadrantsCount = quadrantsCount(sk_maxDepth);

		std::vector<XMFLOAT2> m_perQuadrantObjectsCenters[sk_maxQuadrantsCount];
		std::vector<ObjectData> m_perQuadrantObjectsDatas[sk_maxQuadrantsCount];

		/*
		starting from the first quadrant, which cover the entire area,
		a quadrant's first children is allocated on the next element
		*/
		Quadrant m_quadrants[sk_maxQuadrantsCount];

		XMFLOAT2 m_objectsHalfExtents;
		XMFLOAT2 m_perDepthQuadrantSize[sk_maxDepth + 1]; //from depth == 0 (entire area) to sk_maxDepth inclusive
		unsigned int m_perQuadrantDepth[sk_maxQuadrantsCount];
		unsigned int m_childrenOffsets[sk_maxDepth]; //not sk_maxDepth + 1 because leaves haven't children
		bool m_perQuadrantSubdivided[sk_maxQuadrantsCount];
	};
	
	//Quadtree implementation

	template<typename ObjectData, unsigned int MAX_DEPTH, typename SubdivisionPolicy>
	inline 
		Quadtree<ObjectData, MAX_DEPTH, SubdivisionPolicy>::
			Quadtree(const AABB& quadtreeArea, const XMFLOAT2& objectsHalfExtents) : m_objectsHalfExtents{ objectsHalfExtents }
	{
		quadrant(0).min() = quadtreeArea.min();

		const XMFLOAT2 areaSize = quadtreeArea.max() - quadtreeArea.min();

		XMFLOAT2 currQuadrantSize = areaSize;
		for (unsigned int depth = 0; depth <= sk_maxDepth; ++depth)
		{
			perDepthQuadrantSize(depth) = currQuadrantSize;
			currQuadrantSize = currQuadrantSize * 0.5f;
		}

		for (unsigned int depth = 0; depth < sk_maxDepth; ++depth)
		{
			m_childrenOffsets[depth] = computeSiblingsOffset(depth + 1, sk_maxDepth);
		}

		buildQuadrant(0, 0);
	}

	template<typename ObjectData, unsigned int MAX_DEPTH, typename SubdivisionPolicy>
	inline
		void 
			Quadtree<ObjectData, MAX_DEPTH, SubdivisionPolicy>::buildQuadrant(unsigned int quadrantIndex, unsigned int currDepth)
	{
		assert(quadrantIndex < sk_maxQuadrantsCount);
		assert(currDepth <= sk_maxDepth);

		Quadrant& parent = quadrant(quadrantIndex);

		setQuadrantDepth(quadrantIndex, currDepth);
		setQuadrantSubdivided(quadrantIndex, false);

		if (currDepth == sk_maxDepth)
		{
			return;
		}

		unsigned int siblingOffset = childrenOffsets(currDepth);

		unsigned int childrenDepth = currDepth + 1;

		const XMFLOAT2& childrenSize = perDepthQuadrantSize(childrenDepth);
		
		const unsigned int firstQuadrantIndex = quadrantIndex + 1;
		const unsigned int secondQuadrantIndex = firstQuadrantIndex + siblingOffset;
		const unsigned int thirdQuadrantIndex = secondQuadrantIndex + siblingOffset;
		const unsigned int fourthQuadrantIndex = thirdQuadrantIndex + siblingOffset;

		Quadrant& firstQuadrant = quadrant(firstQuadrantIndex);
		firstQuadrant.min() = parent.min();

		Quadrant& secondQuadrant = quadrant(secondQuadrantIndex);
		secondQuadrant.min() = firstQuadrant.min() + XMFLOAT2{ childrenSize.x + gk_epsilon, 0.0f };

		Quadrant& thirdQuadrant = quadrant(thirdQuadrantIndex);
		thirdQuadrant.min() = firstQuadrant.min() + XMFLOAT2{ 0.0f, childrenSize.y + gk_epsilon };

		Quadrant& fourthQuadrant = quadrant(fourthQuadrantIndex);
		fourthQuadrant.min() = firstQuadrant.min() + XMFLOAT2{ childrenSize.x + gk_epsilon, childrenSize.y + gk_epsilon };

		buildQuadrant(firstQuadrantIndex, childrenDepth);
		buildQuadrant(secondQuadrantIndex, childrenDepth);
		buildQuadrant(thirdQuadrantIndex, childrenDepth);
		buildQuadrant(fourthQuadrantIndex, childrenDepth);
	}

	template<typename ObjectData, unsigned int MAX_DEPTH, typename SubdivisionPolicy>
	inline
		void
			Quadtree<ObjectData, MAX_DEPTH, SubdivisionPolicy>::insert(const XMFLOAT2& objectCenter,
																	   const ObjectData& objectData)
	{
		const AABB objectAABB = AABB::computeFromCenterAndHalfExtents(objectCenter, m_objectsHalfExtents);
		const XMFLOAT2 objectAABBMin = objectAABB.min();
		const XMFLOAT2 objectAABBMax = objectAABB.max();

		unsigned int currDepth = 0;
		unsigned int currQuadrantIndex = 0;

		auto addObjectToQuadrant = [this, &currQuadrantIndex, &objectCenter, &objectData]()
		{
			quadrantObjectsCenters(currQuadrantIndex).push_back(objectCenter);
			quadrantObjectsDatas(currQuadrantIndex).push_back(objectData);			
		};
		
		while (true)
		{
			if (!isQuadrantSubdivided(currQuadrantIndex))
			{
				addObjectToQuadrant();
				
				if (currDepth < sk_maxDepth && shouldSubdivide(currQuadrantIndex))
				{
					subdivide(currQuadrantIndex, currDepth);
				}

				break;
			}

			//isQuadrantSubdivided(currQuadrantIndex) == false for each leaf, so the current quadrant is not a leaf

			//continue the insertion in one child
			const unsigned int childrenOffset = childrenOffsets(currDepth);
			++currDepth;
			
			const XMFLOAT2& childQuadrantsSize = perDepthQuadrantSize(currDepth);

			unsigned int childIndex = currQuadrantIndex + 1;

			unsigned int child = 0;
			for (; child < 4; ++child)
			{
				if (quadrant(childIndex).contains(objectAABBMin, objectAABBMax, childQuadrantsSize))
				{
					currQuadrantIndex = childIndex;
					break;
				}

				childIndex += childrenOffset;
			}

			if (child == 4)
			{
				//the object doesn't fit any child, so keep it at this level
				addObjectToQuadrant();
				break;
			}			
		}
	}

	template<typename ObjectData, unsigned int MAX_DEPTH, typename SubdivisionPolicy>
	inline
		void
			Quadtree<ObjectData, MAX_DEPTH, SubdivisionPolicy>::subdivide(unsigned int quadrantIndex, unsigned int currDepth)
	{
		assert(currDepth < sk_maxDepth);
		assert(!isQuadrantSubdivided(quadrantIndex));

		bool done = false;

		while (!done)
		{
			unsigned int childrenOffset = childrenOffsets(currDepth);
			++currDepth;
			const XMFLOAT2& childrenQuadrantSize = perDepthQuadrantSize(currDepth);

			unsigned int childrenIndices[4];
			computeChildrenIndices(quadrantIndex, childrenIndices, childrenOffset);
			
			std::vector<XMFLOAT2>& objectsCenters = quadrantObjectsCenters(quadrantIndex);
			std::vector<ObjectData>& objectsDatas = quadrantObjectsDatas(quadrantIndex);

			//remove from the quadrant's objects the ones belonging to the children
			//and keep the remaining ones

			int objectsCount = static_cast<int>(objectsCenters.size()); 

			for (int objectIndex = 0; objectIndex < objectsCount; ++objectIndex)
			{
				const XMFLOAT2& objectCenter = objectsCenters[objectIndex];

				const AABB objectAABB = AABB::computeFromCenterAndHalfExtents(objectCenter, m_objectsHalfExtents);
				const XMFLOAT2 objectAABBMin = objectAABB.min();
				const XMFLOAT2 objectAABBMax = objectAABB.max();

				for (unsigned int child = 0; child < 4; ++child)
				{
					const unsigned int childrenIndex = childrenIndices[child];
					const Quadrant& childQuadrant = quadrant(childrenIndex);

					if (childQuadrant.contains(objectAABBMin, objectAABBMax, childrenQuadrantSize))
					{
						quadrantObjectsCenters(childrenIndex).push_back(std::move(objectCenter));
						quadrantObjectsDatas(childrenIndex).push_back(std::move(objectsDatas[objectIndex]));

						objectsCenters.erase(objectsCenters.begin() + objectIndex);
						objectsDatas.erase(objectsDatas.begin() + objectIndex);

						--objectIndex;
						--objectsCount;

						break;
					}
				}				
			}

			setQuadrantSubdivided(quadrantIndex, true);

			if (currDepth == sk_maxDepth)
			{
				//all done!
				break;
			}

			//check if a child needs to be subdivided...just one, because we subdivide a quadrant as soon as it satisfy a condition.
			//thus, in the worst case the situation is repeated iff all objects have been transferred entirely to a child.

			unsigned int child = 0;
			for (; child < 4; ++child)
			{
				const unsigned int childrenIndex = childrenIndices[child];
				if (shouldSubdivide(childrenIndex))
				{
					quadrantIndex = childrenIndex;
					break;
				}
			}

			done = (child == 4);
		}
	}
		
	template<typename ObjectData, unsigned int MAX_DEPTH, typename SubdivisionPolicy>
	inline
		unsigned int
			Quadtree<ObjectData, MAX_DEPTH, SubdivisionPolicy>::findPotentialColliders(const AABB& objectAABB,
																					   ObjectData* foundObjects)const
	{
		assert(foundObjects != nullptr);
		
		unsigned int foundObjectsCount = 0;

		const XMFLOAT2 objectAABBMin = objectAABB.min();
		const XMFLOAT2 objectAABBMax = objectAABB.max();

		/*
		quadrantsIndicesStack is a stack used to navigate the quadtree in a depth-first fashion.
		initially, it contains only the root. on each iteration of the next
		while-loop, one element of the stack is popped out and AT MOST four are pushed in.
		this continues until leaves are found, in which case they are popped out until a non-leaf node is found again.
		because of this, we can compute an upper-bound of the stack size:
		(((1 - 1) + 4) - 1 + 4)...etc, until max_depth passes have been done
		*/
		unsigned int quadrantsIndicesStack[3 * sk_maxDepth + 1];
		int stackPointer = -1; //-1 => empty stack

		++stackPointer;
		quadrantsIndicesStack[stackPointer] = 0; //push root
				
		while (stackPointer != -1)
		{
			assert(stackPointer < 3 * sk_maxDepth + 1);
			const unsigned int quadrantIndex = quadrantsIndicesStack[stackPointer];
			--stackPointer;

			const unsigned int quadrantDepth = perQuadrantDepth(quadrantIndex);

			const Quadrant& currQuadrant = quadrant(quadrantIndex);
			
			if (currQuadrant.outside(objectAABBMin, objectAABBMax, perDepthQuadrantSize(quadrantDepth)))
			{
				//the object doesn't touch the current quadrant, so skip its objects
				continue;
			}

			//add all the quadrant objects
			for (auto& quadrantObjectData : quadrantObjectsDatas(quadrantIndex))
			{
				foundObjects[foundObjectsCount++] = quadrantObjectData;
			}

			if (isQuadrantSubdivided(quadrantIndex))
			{
				unsigned int childrenIndices[4];
				computeChildrenIndices(quadrantIndex, childrenIndices, childrenOffsets(quadrantDepth));

				//check the first child before
				for (int child = 3; child >= 0; --child)
				{
					++stackPointer;
					quadrantsIndicesStack[stackPointer] = childrenIndices[child];
				}
			}
		}

		return foundObjectsCount;
	}
	
	template<typename ObjectData, unsigned int MAX_DEPTH, typename SubdivisionPolicy>
	inline
		bool
			Quadtree<ObjectData, MAX_DEPTH, SubdivisionPolicy>::shouldSubdivide(unsigned int quadrantIndex) const
	{
		return SubdivisionPolicy::shouldSubdivideQuadrant(static_cast<unsigned int>(quadrantObjectsCenters(quadrantIndex).size()));		
	}
		
	template<typename ObjectData, unsigned int MAX_DEPTH, typename SubdivisionPolicy>
	inline 
		std::vector<XMFLOAT2>&
			Quadtree<ObjectData, MAX_DEPTH, SubdivisionPolicy>::quadrantObjectsCenters(unsigned int quadrantIndex)
	{
		assert(quadrantIndex < sk_maxQuadrantsCount);
		return m_perQuadrantObjectsCenters[quadrantIndex];
	}

	template<typename ObjectData, unsigned int MAX_DEPTH, typename SubdivisionPolicy>
	inline 
		const std::vector<XMFLOAT2>&
			Quadtree<ObjectData, MAX_DEPTH, SubdivisionPolicy>::quadrantObjectsCenters(unsigned int quadrantIndex) const
	{
		assert(quadrantIndex < sk_maxQuadrantsCount);
		return m_perQuadrantObjectsCenters[quadrantIndex];
	}

	template<typename ObjectData, unsigned int MAX_DEPTH, typename SubdivisionPolicy>
	inline 
		std::vector<ObjectData>&
			Quadtree<ObjectData, MAX_DEPTH, SubdivisionPolicy>::quadrantObjectsDatas(unsigned int quadrantIndex)
	{
		assert(quadrantIndex < sk_maxQuadrantsCount);
		return m_perQuadrantObjectsDatas[quadrantIndex];
	}

	template<typename ObjectData, unsigned int MAX_DEPTH, typename SubdivisionPolicy>
	inline 
		const std::vector<ObjectData>&
			Quadtree<ObjectData, MAX_DEPTH, SubdivisionPolicy>::quadrantObjectsDatas(unsigned int quadrantIndex) const
	{
		assert(quadrantIndex < sk_maxQuadrantsCount);
		return m_perQuadrantObjectsDatas[quadrantIndex];
	}

	template<typename ObjectData, unsigned int MAX_DEPTH, typename SubdivisionPolicy>
	inline 
		Quadrant&
			Quadtree<ObjectData, MAX_DEPTH, SubdivisionPolicy>::quadrant(unsigned int quadrantIndex)
	{
		assert(quadrantIndex < sk_maxQuadrantsCount);
		return m_quadrants[quadrantIndex];
	}

	template<typename ObjectData, unsigned int MAX_DEPTH, typename SubdivisionPolicy>
	inline 
		const Quadrant&
			Quadtree<ObjectData, MAX_DEPTH, SubdivisionPolicy>::quadrant(unsigned int quadrantIndex) const
	{
		assert(quadrantIndex < sk_maxQuadrantsCount);
		return m_quadrants[quadrantIndex];
	}

	template<typename ObjectData, unsigned int MAX_DEPTH, typename SubdivisionPolicy>
	inline 
		XMFLOAT2&
			Quadtree<ObjectData, MAX_DEPTH, SubdivisionPolicy>::perDepthQuadrantSize(unsigned int depth)
	{
		assert(depth <= sk_maxDepth);
		return m_perDepthQuadrantSize[depth];
	}

	template<typename ObjectData, unsigned int MAX_DEPTH, typename SubdivisionPolicy>
	inline 
		const XMFLOAT2&
			Quadtree<ObjectData, MAX_DEPTH, SubdivisionPolicy>::perDepthQuadrantSize(unsigned int depth) const
	{
		assert(depth <= sk_maxDepth);
		return m_perDepthQuadrantSize[depth];
	}

	template<typename ObjectData, unsigned int MAX_DEPTH, typename SubdivisionPolicy>
	inline 
		unsigned int
			Quadtree<ObjectData, MAX_DEPTH, SubdivisionPolicy>::childrenOffsets(unsigned int depth)const
	{
		assert(depth < sk_maxDepth);
		return m_childrenOffsets[depth];
	}

	template<typename ObjectData, unsigned int MAX_DEPTH, typename SubdivisionPolicy>
	inline 
		unsigned int 
			Quadtree<ObjectData, MAX_DEPTH, SubdivisionPolicy>::perQuadrantDepth(unsigned int quadrantIndex) const
	{
		assert(quadrantIndex < sk_maxQuadrantsCount);
		return m_perQuadrantDepth[quadrantIndex];
	}

	template<typename ObjectData, unsigned int MAX_DEPTH, typename SubdivisionPolicy>
	inline 
		void
			Quadtree<ObjectData, MAX_DEPTH, SubdivisionPolicy>::setQuadrantDepth(unsigned int quadrantIndex, unsigned int quadrantDepth)
	{
		assert(quadrantIndex < sk_maxQuadrantsCount);
		assert(quadrantDepth <= sk_maxDepth);
		m_perQuadrantDepth[quadrantIndex] = quadrantDepth;
	}

	template<typename ObjectData, unsigned int MAX_DEPTH, typename SubdivisionPolicy>
	inline 
		void
			Quadtree<ObjectData, MAX_DEPTH, SubdivisionPolicy>::setQuadrantSubdivided(unsigned int quadrantIndex, bool quadrantSubdivided)
	{
		assert(quadrantIndex < sk_maxQuadrantsCount);
		m_perQuadrantSubdivided[quadrantIndex] = quadrantSubdivided;
	}
	
	template<typename ObjectData, unsigned int MAX_DEPTH, typename SubdivisionPolicy>
	inline 
		bool
			Quadtree<ObjectData, MAX_DEPTH, SubdivisionPolicy>::isQuadrantSubdivided(unsigned int quadrantIndex) const
	{
		assert(quadrantIndex < sk_maxQuadrantsCount);
		return m_perQuadrantSubdivided[quadrantIndex];
	}
}
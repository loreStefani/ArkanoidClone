#pragma once
#include "WindowSizeEventsObserver.h"
#include "ArkanoidRenderer.h"
#include "Camera.h"
#include "InputManager.h"
#include "Dimensions.h"
#include "Quadtree.h"
#include <vector>

namespace ArkanoidEngine
{
	class Application;	
}

namespace ArkanoidGame
{	
	class AABB;

	class ArkanoidLogic : public WindowSizeEventsObserver
	{
	public:
		//ctors
		explicit ArkanoidLogic(Application&);

		//dtor
		virtual ~ArkanoidLogic();

		//copy
		ArkanoidLogic(const ArkanoidLogic&) = delete;
		ArkanoidLogic& operator=(const ArkanoidLogic&) = delete;

		//move
		ArkanoidLogic(ArkanoidLogic&&) = delete;
		ArkanoidLogic& operator=(ArkanoidLogic&&) = delete;

		const ArkanoidRenderer::TransformsConstantBuffer* transforms()const;
		const ArkanoidRenderer::ColorsAndUVTransformsConstantBuffer* colorsAndUVTransforms()const;

		Application& application();
		const Camera& camera()const;

		void render();

		void fillUVTransforms(unsigned int atlasWidth, unsigned int atlasHeight);

		virtual void onResize() override;
		virtual void onMinimized()override;
		virtual void onMaximized()override;
		virtual void onStartResizing()override;
		virtual void onStopResizing()override;

		void onLeftKeyDown();
		void onRightKeyDown();
		void onLeftKeyUp();
		void onRightKeyUp();
		void onFireKeyDown();
		void onFireKeyUp();
		void onBrickShuffleKeyDown();
		void onBrickShuffleKeyUp();

		static constexpr unsigned int sk_quadtreeMaxDepth = 2;
		using Quadtree = Quadtree<unsigned int, sk_quadtreeMaxDepth>;

	private:		
				
		void update();

		void setupLevel();
		
		void placeBricks();
		unsigned int placeBricksRowByRow(AABB& bricksAABB);
		unsigned int placeBricksDiamond(AABB& bricksAABB);
		unsigned int placeBricksColumnsByColumns(AABB& bricksAABB);

		void assignBrickType(unsigned int brickIndex, unsigned int brickTypeIndex);

		void buildQuadtree(const AABB& bricksAABB, unsigned int bricksCount);

		void restartLevel();

		void movePlayer(float deltaTime);
		void moveBall(float deltaTime);
		void moveBonus(float deltaTime);

		void checkBounds(const XMFLOAT2& playerAABBMin, const XMFLOAT2& playerAABBMax,
						 const XMFLOAT2& ballAABBMin, const XMFLOAT2& ballAABBMax);

		struct CollisionData
		{
			//the following are either 0 or 1 to indicate the direction from which the ball hits an AABB
			unsigned int fromRight;
			unsigned int fromLeft;
			unsigned int fromTop;
			unsigned int fromBottom;
		};

		CollisionData ballAABBCollisionData(const AABB& aabb,
											const XMFLOAT2& currBallAABBMin, const XMFLOAT2& currBallAABBMax,
											const XMFLOAT2& lastBallAABBMin, const XMFLOAT2& lastBallAABBMax);

		void checkBricksCollision(const AABB& currBallAABB,
								  const XMFLOAT2& currBallAABBMin, const XMFLOAT2& currBallAABBMax,
								  const XMFLOAT2& lastBallAABBMin, const XMFLOAT2& lastBallAABBMax);
				
		void checkBonusCollision(const AABB& playerAABB);

		void handleSpawnBonus(const XMFLOAT2& spawnPosition);
		unsigned int generateNextBonusBricksHitCount();

		void updateCameraProjection();

		void translateOutOfArena(XMFLOAT4& transform);


		XMFLOAT4& ballTransform();
		const XMFLOAT4& ballTransform()const;

		XMFLOAT4& playerTransform();
		const XMFLOAT4& playerTransform()const;

		XMFLOAT4& arenaTransform();
		const XMFLOAT4& arenaTransform()const;

		XMFLOAT4& bonusTransform();
		const XMFLOAT4& bonusTransform()const;

		XMFLOAT4& brickTransform(unsigned int brickIndex);
		const XMFLOAT4& brickTransform(unsigned int brickIndex)const;

		XMFLOAT4& ballColorAndUVTransform();
		const XMFLOAT4& ballColorAndUVTransform()const;

		XMFLOAT4& playerColorAndUVTransform();
		const XMFLOAT4& playerColorAndUVTransform()const;

		XMFLOAT4& arenaColorAndUVTransform();
		const XMFLOAT4& arenaColorAndUVTransform()const;

		XMFLOAT4& bonusColorAndUVTransform();
		const XMFLOAT4& bonusColorAndUVTransform()const;

		XMFLOAT4& brickColorAndUVTransform(unsigned int brickIndex);
		const XMFLOAT4& brickColorAndUVTransform(unsigned int brickIndex)const;

		XMFLOAT4& ballUVTransform();
		const XMFLOAT4& ballUVTransform()const;

		XMFLOAT4& playerUVTransform();
		const XMFLOAT4& playerUVTransform()const;

		XMFLOAT4& arenaUVTransform();
		const XMFLOAT4& arenaUVTransform()const;

		XMFLOAT4& bonusUVTransform();
		const XMFLOAT4& bonusUVTransform()const;

		XMFLOAT4& bricksUVTransform();
		const XMFLOAT4& bricksUVTransform()const;

		unsigned int& brickRemainingHits(unsigned int brickIndex);
		const unsigned int& brickRemainingHits(unsigned int brickIndex)const;
		
		using BrickPlacerFunction = unsigned int (ArkanoidLogic::*)(AABB&);
		static constexpr BrickPlacerFunction sk_brickPlacers[] =
		{
			&ArkanoidLogic::placeBricksRowByRow,
			&ArkanoidLogic::placeBricksDiamond,
			&ArkanoidLogic::placeBricksColumnsByColumns
		};
		static constexpr unsigned int sk_brickPlacersCount = sizeof(sk_brickPlacers) / sizeof(BrickPlacerFunction);

		Application& m_application;
		ArkanoidRenderer* m_renderer{ nullptr };
		
		InputManager m_inputManager;

		ArkanoidRenderer::TransformsConstantBuffer m_transforms{};
		ArkanoidRenderer::ColorsAndUVTransformsConstantBuffer m_colorsAndUVTransforms{};
		
		Camera m_camera{};
		
		Quadtree m_quadtree;
			
		XMFLOAT2 m_ballVelocity{};

		unsigned int m_ballColliders[gk_bricksCount];

		unsigned int m_bricksRemainingHits[gk_bricksCount];

		unsigned int m_bonusBricksHit{ 0 };
		unsigned int m_nextBonusBricksHitCount{ 0 };
		bool m_bonusAlive{ false };
	};

	inline Application& ArkanoidLogic::application()
	{
		return m_application;
	}

	inline const ArkanoidRenderer::TransformsConstantBuffer* ArkanoidLogic::transforms()const
	{
		return &m_transforms;
	}

	inline const ArkanoidRenderer::ColorsAndUVTransformsConstantBuffer* ArkanoidLogic::colorsAndUVTransforms()const
	{
		return &m_colorsAndUVTransforms;
	}

	inline const Camera& ArkanoidLogic::camera()const
	{
		return m_camera;
	}	
		
	inline void ArkanoidLogic::onMinimized()
	{
		//DO NOTHING
	}

	inline void ArkanoidLogic::onMaximized()
	{
		//DO NOTHING
	}

	inline void ArkanoidLogic::onStartResizing()
	{
		//DO NOTHING
	}

	inline void ArkanoidLogic::onStopResizing()
	{	
		//DO NOTHING
	}
		
	inline void ArkanoidLogic::onLeftKeyUp()
	{
		//DO NOTHING
	}

	inline void ArkanoidLogic::onRightKeyUp()
	{
		//DO NOTHING
	}
		
	inline void ArkanoidLogic::onFireKeyUp()
	{
		//DO NOTHING
	}

	inline void ArkanoidLogic::onBrickShuffleKeyDown()
	{
		//DO NOTHING
	}

	inline void ArkanoidLogic::onLeftKeyDown()
	{
		//DO NOTHING
	}

	inline void ArkanoidLogic::onRightKeyDown()
	{
		//DO NOTHING
	}

	inline void ArkanoidLogic::onFireKeyDown()
	{
		//DO NOTHING
	}

}

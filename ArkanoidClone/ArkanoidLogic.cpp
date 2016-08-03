#include "MemoryCommon.h"
#include "ArkanoidLogic.h"
#include "Application.h"
#include "Window.h"
#include "TextureTileInfo.h"
#include <fstream>
#include "Resources.h"
#include <unordered_map>
#include "AABB.h"
#include <ctime>

using namespace ArkanoidGame;

static constexpr unsigned int gk_arenaWidth = 40;
static constexpr unsigned int gk_arenaHeight = 60;
static constexpr unsigned int gk_arenaHalfWidth = gk_arenaWidth / 2;
static constexpr unsigned int gk_arenaHalfHeight = gk_arenaHeight / 2;

static constexpr int gk_arenaMinX = -static_cast<int>(gk_arenaHalfWidth);
static constexpr int gk_arenaMaxX = gk_arenaHalfWidth;
static constexpr int gk_arenaMinY = -static_cast<int>(gk_arenaHalfHeight);
static constexpr int gk_arenaMaxY = gk_arenaHalfHeight;

static constexpr float gk_outOfArenaX = gk_arenaMaxX * 10.0f;
static constexpr float gk_outOfArenaY = gk_arenaMaxY * 10.0f;

static constexpr float gk_bricksWidth = 4.0f;
static constexpr float gk_bricksHeight = 2.0f;
static constexpr float gk_bricksHalfWidth = gk_bricksWidth*0.5f;
static constexpr float gk_bricksHalfHeight = gk_bricksHeight * 0.5f;

static constexpr float gk_playerHalfWidth = 3.0f;
static constexpr float gk_playerHalfHeight = 0.5f;

static constexpr float gk_ballHalfWidth = 1.0f;
static constexpr float gk_ballHalfHeight = 1.0f;

static constexpr float gk_bonusHalfWidth = 1.0f;
static constexpr float gk_bonusHalfHeight = 0.5f;

static constexpr unsigned int gk_arenaIndex = 0;
static constexpr unsigned int gk_bricksStartIndex = gk_arenaIndex + 1;
static constexpr unsigned int gk_bricksEndIndex = gk_bricksStartIndex + gk_bricksCount;
static constexpr unsigned int gk_ballIndex = gk_bricksEndIndex;
static constexpr unsigned int gk_playerIndex = gk_ballIndex + 1;
static constexpr unsigned int gk_bonusIndex = gk_playerIndex + 1;

static constexpr unsigned int gk_arenaUVTransformIndex = 0;
static constexpr unsigned int gk_bricksUVTransformIndex = gk_arenaUVTransformIndex + 1;
static constexpr unsigned int gk_ballUVTransformIndex = gk_bricksUVTransformIndex + 1;
static constexpr unsigned int gk_playerUVTransformIndex = gk_ballUVTransformIndex + 1;
static constexpr unsigned int gk_bonusUVTransformIndex = gk_playerUVTransformIndex + 1;

static constexpr unsigned int gk_maxBonusBricksHitCount = 5;

static constexpr float gk_playerSpeed = 24.0f;
static constexpr float gk_startBallSpeed = 35.0f;
static constexpr float gk_startBallVelocityX = 0.5f;
static constexpr float gk_startBallVelocityY = 1.0f;
static constexpr float gk_bonusSpeedY = 8.0f;

static constexpr float gk_gameOverBallY = gk_arenaMinY - 15.0f;
static constexpr float gk_destroyBonusY = gk_arenaMinY - 1.0f;

static const XMFLOAT2 gk_ballHalfExtents{ gk_ballHalfWidth, gk_ballHalfHeight };
static const XMFLOAT2 gk_playerHalfExtents{ gk_playerHalfWidth, gk_playerHalfHeight };
static const XMFLOAT2 gk_bricksHalfExtents{ gk_bricksHalfWidth, gk_bricksHalfHeight };
static const XMFLOAT2 gk_bonusHalfExtents{ gk_bonusHalfWidth, gk_bonusHalfHeight };

static constexpr unsigned int gk_brickTypesCount = 3;

static const XMFLOAT4 gk_bricksColors[gk_brickTypesCount] =
{
	XMFLOAT4{ 1.0f, 0.0f, 0.0f, static_cast<float>(gk_bricksUVTransformIndex) },
	XMFLOAT4{ 0.0f, 1.0f, 0.0f, static_cast<float>(gk_bricksUVTransformIndex) },
	XMFLOAT4{ 0.0f, 0.0f, 1.0f, static_cast<float>(gk_bricksUVTransformIndex) }
};

static constexpr unsigned int gk_bricksHitsCounts[gk_brickTypesCount] =
{
	3,
	2,
	1
};



static ArkanoidLogic::Quadtree createQuadtree(const AABB& bricksAABB, const XMFLOAT2& bricksHalfExtents)
{
	return ArkanoidLogic::Quadtree{ bricksAABB, bricksHalfExtents };
}

static ArkanoidLogic::Quadtree createQuadtree(const XMFLOAT2& bricksHalfExtents)
{
	const AABB arenaAABB = AABB::computeFromCenterAndHalfExtents(XMFLOAT2{ 0.0f, 0.0f },
																 XMFLOAT2{ static_cast<float>(gk_arenaHalfWidth),
																		   static_cast<float>(gk_arenaHalfHeight) });
	return createQuadtree(arenaAABB, bricksHalfExtents);
}

ArkanoidLogic::ArkanoidLogic(Application& application) : m_application{ application },
														 m_inputManager{ application.window(), *this },
														 m_quadtree{ createQuadtree(XMFLOAT2{ gk_bricksHalfWidth, gk_bricksHalfHeight }) }														 
{
	updateCameraProjection();
	m_camera.lookAt(XMFLOAT3{ 0.0f, 0.0f, -35.0f }, XMFLOAT3{ 0.0f, 0.0f, 0.0f });

	std::srand(static_cast<unsigned int>(std::time(nullptr)));

	setupLevel();

	m_renderer = new ArkanoidRenderer{ *this };

	application.window().addWindowSizeEventsObserver(this);
}

ArkanoidLogic::~ArkanoidLogic()
{
	m_application.window().removeWindowSizeEventsObserver(this);
	delete m_renderer;
}

void ArkanoidLogic::setupLevel()
{
	placeBricks();

	//ball
	constexpr float ballUVIndex = static_cast<float>(gk_ballUVTransformIndex);
	ballTransform() = XMFLOAT4{ 0.0f, static_cast<float>(gk_arenaMinY) + 1.0f + 1.0f, gk_ballHalfWidth, gk_ballHalfHeight };
	ballColorAndUVTransform() = XMFLOAT4{ 1.0f, 1.0f, 1.0f, ballUVIndex };

	XMVECTOR ballVelocity = XMVectorScale(XMVector2Normalize(XMVectorSet(gk_startBallVelocityX, gk_startBallVelocityY, 0.0f, 0.0f)), gk_startBallSpeed);
	XMStoreFloat2(&m_ballVelocity, ballVelocity);

	//player
	constexpr float playerUVIndex = static_cast<float>(gk_playerUVTransformIndex);
	playerTransform() = XMFLOAT4{ 0.0f, static_cast<float>(gk_arenaMinY), gk_playerHalfWidth, gk_playerHalfHeight };
	playerColorAndUVTransform() = XMFLOAT4{ 1.0f, 1.0f, 1.0f, playerUVIndex };

	//arena
	constexpr float arenaUVIndex = static_cast<float>(gk_arenaUVTransformIndex);
	arenaTransform() = XMFLOAT4{ 0.0f, 0.0f, static_cast<float>(gk_arenaMaxX + 1.5f), static_cast<float>(gk_arenaMaxY + 1.5f) };
	arenaColorAndUVTransform() = XMFLOAT4{ 1.0f, 1.0f, 1.0f, arenaUVIndex };

	//bonus
	constexpr float bonusUVIndex = static_cast<float>(gk_bonusUVTransformIndex);
	bonusTransform() = XMFLOAT4{ gk_outOfArenaX, 0.0f, gk_bonusHalfWidth, gk_bonusHalfHeight };
	bonusColorAndUVTransform() = XMFLOAT4{ 1.0f, 1.0f, 1.0f, bonusUVIndex };

	m_bonusAlive = false;
	m_bonusBricksHit = 0;
	m_nextBonusBricksHitCount = generateNextBonusBricksHitCount();
}

void ArkanoidLogic::updateCameraProjection()
{
	m_camera.makePerspective(m_application.window().aspectRatio(), XM_PI*0.5f, 1.0f, 50.0f);
}

void ArkanoidLogic::onResize()
{
	updateCameraProjection();
	m_renderer->onCameraChanged();
}

unsigned int ArkanoidLogic::generateNextBonusBricksHitCount()
{
	return (std::rand() % gk_maxBonusBricksHitCount) + 1;
}

void ArkanoidLogic::placeBricks()
{
	//place bricks picking randomly one of the brick placer functions
	AABB bricksAABB{};
	const unsigned int brickPlacerFunctionIndex = std::rand() % sk_brickPlacersCount;
	const unsigned int placedBricks = (this->*sk_brickPlacers[brickPlacerFunctionIndex])(bricksAABB);

	buildQuadtree(bricksAABB, placedBricks);

	//hide unplaced bricks
	for (unsigned int unPlacedBrickIndex = placedBricks; unPlacedBrickIndex < gk_bricksCount; ++unPlacedBrickIndex)
	{
		translateOutOfArena(brickTransform(unPlacedBrickIndex));
	}
}

void ArkanoidLogic::buildQuadtree(const AABB& bricksAABB, unsigned int bricksCount)
{
	m_quadtree = createQuadtree(bricksAABB, gk_bricksHalfExtents);

	for (unsigned int brickIndex = 0; brickIndex < bricksCount; ++brickIndex)
	{
		const XMFLOAT4& brickTranslationAndScale = brickTransform(brickIndex);
		const XMFLOAT2 brickCenter{ brickTranslationAndScale.x, brickTranslationAndScale.y };
		m_quadtree.insert(brickCenter, brickIndex);
	}
}

void ArkanoidLogic::restartLevel()
{
	setupLevel();
	m_renderer->onColorsOrUVTransformsChanged();
}

void ArkanoidLogic::render()
{
	update();
	m_renderer->render();
}

void ArkanoidLogic::update()
{
	const float deltaTimeMillis = m_application.timer().deltaTime();
	const float deltaTime = static_cast<float>(deltaTimeMillis) / 1000.0f;

	movePlayer(deltaTime);
	moveBonus(deltaTime);
	
	const XMFLOAT4& ballTranslateAndScale = ballTransform();

	const XMFLOAT2 lastBallPosition{ ballTranslateAndScale.x, ballTranslateAndScale.y };

	moveBall(deltaTime); //modifies ballTranslateAndScale

	const XMFLOAT2 ballPosition{ ballTranslateAndScale.x, ballTranslateAndScale.y };

	if (ballPosition.y < gk_gameOverBallY)
	{
		restartLevel();
		return;
	}

	const XMFLOAT4& playerTranslateAndScale = playerTransform();
	const XMFLOAT2 playerPosition{ playerTranslateAndScale.x, playerTranslateAndScale.y };

	const AABB playerAABB = AABB::computeFromCenterAndHalfExtents(playerPosition, gk_playerHalfExtents);
	const AABB currBallAABB = AABB::computeFromCenterAndHalfExtents(ballPosition, gk_ballHalfExtents);

	const XMFLOAT2 playerAABBMin = playerAABB.min();
	const XMFLOAT2 playerAABBMax = playerAABB.max();
	const XMFLOAT2 currBallAABBMin = currBallAABB.min();
	const XMFLOAT2 currBallAABBMax = currBallAABB.max();

	checkBounds(playerAABBMin, playerAABBMax, currBallAABBMin, currBallAABBMax);
	checkBonusCollision(playerAABB);

	const AABB lastBallAABB = AABB::computeFromCenterAndHalfExtents(lastBallPosition, gk_ballHalfExtents);
	const XMFLOAT2 lastBallAABBMin = lastBallAABB.min();
	const XMFLOAT2 lastBallAABBMax = lastBallAABB.max();

	if (playerAABB.intersects(currBallAABB))
	{
		CollisionData collisionData = ballAABBCollisionData(playerAABB, currBallAABBMin, currBallAABBMax, lastBallAABBMin, lastBallAABBMax);

		const unsigned int reverseYVelocity = collisionData.fromTop | collisionData.fromBottom;

		const float ballVelocitiesY[2] = { m_ballVelocity.y, -m_ballVelocity.y };

		m_ballVelocity.y = ballVelocitiesY[reverseYVelocity];

		//find ball AABB's x position nearest to the player
		const unsigned int isBallLeftSide = static_cast<unsigned int>(ballPosition.x < playerPosition.x);
		const float ballXPos[2] = { currBallAABBMin.x, currBallAABBMax.x };

		//make the new velocity angle (wrt the player normal) proportional to the hit distance
		const float ballXPosLocal = ballXPos[isBallLeftSide] - playerPosition.x;
		const float velocityXDir = (ballXPosLocal / gk_playerHalfExtents.x); //normalize
		
		m_ballVelocity.x = gk_startBallSpeed * velocityXDir;
	}
	else
	{
		checkBricksCollision(currBallAABB, currBallAABBMin, currBallAABBMax, lastBallAABBMin, lastBallAABBMax);
	}
}

void ArkanoidLogic::movePlayer(float deltaTime)
{
	XMFLOAT4& playerTranslationAndScale = playerTransform();

	const float displacementValue = gk_playerSpeed*deltaTime;

	const float leftKeyDisplacements[2] = { 0.0f, -displacementValue };
	const float rightKeyDisplacements[2] = { 0.0f, displacementValue };

	playerTranslationAndScale.x += leftKeyDisplacements[static_cast<unsigned int>(m_inputManager.isLeftKeyPressed())];
	playerTranslationAndScale.x += rightKeyDisplacements[static_cast<unsigned int>(m_inputManager.isRightKeyPressed())];
}

void ArkanoidLogic::moveBall(float deltaTime)
{
	XMFLOAT4& ballTranslationAndScale = ballTransform();
	ballTranslationAndScale.x += m_ballVelocity.x * deltaTime;
	ballTranslationAndScale.y += m_ballVelocity.y * deltaTime;
}

void ArkanoidLogic::moveBonus(float deltaTime)
{
	XMFLOAT4& bonusTranslateAndScale = bonusTransform();
	bonusTranslateAndScale.y -= static_cast<unsigned int>(m_bonusAlive) * gk_bonusSpeedY * deltaTime;
}

void ArkanoidLogic::checkBounds(const XMFLOAT2& playerAABBMin, const XMFLOAT2& playerAABBMax,
								const XMFLOAT2& ballAABBMin, const XMFLOAT2& ballAABBMax)
{
	//player

	//adjust player position inside the arena

	XMFLOAT4& playerPos = playerTransform();

	const float playerPosXLeftBounds[2] = { playerPos.x, gk_arenaMinX + gk_playerHalfExtents.x };

	playerPos.x = playerPosXLeftBounds[static_cast<unsigned int>(playerAABBMin.x < gk_arenaMinX)];

	const float playerPosXRightBounds[2] = { playerPos.x, gk_arenaMaxX - gk_playerHalfExtents.x };

	playerPos.x = playerPosXRightBounds[static_cast<unsigned int>(playerAABBMax.x > gk_arenaMaxX)];

	//ball

	const unsigned int outOfArenaXright = static_cast<unsigned int>(ballAABBMax.x > gk_arenaMaxX);
	const unsigned int outOfArenaXleft = static_cast<unsigned int>(ballAABBMin.x < gk_arenaMinX);

	const unsigned int outOfArenaX = outOfArenaXright | outOfArenaXleft;

	const unsigned int outOfArenaYtop = static_cast<unsigned int>(ballAABBMax.y > gk_arenaMaxY);

	const float ballVelocitiesX[2] = { m_ballVelocity.x, -m_ballVelocity.x };
	const float ballVelocitiesY[2] = { m_ballVelocity.y, -m_ballVelocity.y };

	m_ballVelocity.x = ballVelocitiesX[outOfArenaX];
	m_ballVelocity.y = ballVelocitiesY[outOfArenaYtop];

	//adjust ball position inside the arena

	XMFLOAT4& ballPos = ballTransform();

	const float ballPosXLeftBounds[2] = { ballPos.x, gk_arenaMinX + gk_ballHalfExtents.x };

	ballPos.x = ballPosXLeftBounds[outOfArenaXleft];

	const float ballPosXRightBounds[2] = { ballPos.x, gk_arenaMaxX - gk_ballHalfExtents.x };

	ballPos.x = ballPosXRightBounds[outOfArenaXright];

	const float ballPosYTopBounds[2] = { ballPos.y, gk_arenaMaxY - gk_ballHalfExtents.y };

	ballPos.y = ballPosYTopBounds[outOfArenaYtop];
}

void ArkanoidLogic::checkBonusCollision(const AABB& playerAABB)
{
	XMFLOAT4& bonusTranslateAndScale = bonusTransform();

	const unsigned int bonusMissed = static_cast<unsigned int>(bonusTranslateAndScale.y < gk_destroyBonusY);

	const AABB bonusAABB = AABB::computeFromCenterAndHalfExtents(XMFLOAT2{ bonusTranslateAndScale.x, bonusTranslateAndScale.y }, gk_bonusHalfExtents);
	const unsigned int bonusTaken = static_cast<unsigned int>(bonusAABB.intersects(playerAABB));

	const bool destroyBonus = (bonusMissed | bonusTaken) == 1;

	//destroyBonus needs to be checked only if m_bonusAlive == true
	m_bonusAlive = (static_cast<unsigned int>(m_bonusAlive) & static_cast<unsigned int>(!destroyBonus)) == 1;

	const unsigned int bonusDead = static_cast<unsigned int>(!m_bonusAlive);

	//if the bonus is dead, translate it out of the arena, otherwise keep it in the current position
	bonusTranslateAndScale.x = static_cast<float>(bonusDead) * gk_outOfArenaX + static_cast<float>(1 - bonusDead)*bonusTranslateAndScale.x;
}

ArkanoidLogic::CollisionData ArkanoidLogic::ballAABBCollisionData(const AABB& aabb,
																  const XMFLOAT2& currBallAABBMin, const XMFLOAT2& currBallAABBMax,
																  const XMFLOAT2& lastBallAABBMin, const XMFLOAT2& lastBallAABBMax)
{
	auto collideFromTop = [](const XMFLOAT2& aabbMax, const XMFLOAT2& currBallAABBMin, const XMFLOAT2& lastBallAABBMin)
	{
		const float currBottom = currBallAABBMin.y;
		const float lastBottom = lastBallAABBMin.y;
		const unsigned int wasTop = static_cast<unsigned int>(lessEqualf(aabbMax.y, lastBottom));
		const unsigned int isNotTop = static_cast<unsigned int>(lessEqualf(currBottom, aabbMax.y));
		return wasTop & isNotTop;
	};

	auto collideFromBottom = [](const XMFLOAT2& aabbMin, const XMFLOAT2& currBallAABBMax, const XMFLOAT2& lastBallAABBMax)
	{
		const float currTop = currBallAABBMax.y;
		const float lastTop = lastBallAABBMax.y;
		const unsigned int wasBottom = static_cast<unsigned int>(lessEqualf(lastTop, aabbMin.y));
		const unsigned int isNotBottom = static_cast<unsigned int>(lessEqualf(aabbMin.y, currTop));
		return wasBottom & isNotBottom;
	};

	auto collideFromRight = [](const XMFLOAT2& aabbMax, const XMFLOAT2& currBallAABBMin, const XMFLOAT2& lastBallAABBMin)
	{
		const float currLeft = currBallAABBMin.x;
		const float lastLeft = lastBallAABBMin.x;
		const unsigned int wasRight = static_cast<unsigned int>(lessEqualf(aabbMax.x, lastLeft));
		const unsigned int isNotRight = static_cast<unsigned int>(lessEqualf(currLeft, aabbMax.x));
		return wasRight & isNotRight;
	};

	auto collideFromLeft = [](const XMFLOAT2& aabbMin, const XMFLOAT2& currBallAABBMax, const XMFLOAT2& lastBallAABBMax)
	{
		const float currRight = currBallAABBMax.x;
		const float lastRight = lastBallAABBMax.x;
		const unsigned int wasLeft = static_cast<unsigned int>(lessEqualf(lastRight, aabbMin.x));
		const unsigned int isNotLeft = static_cast<unsigned int>(lessEqualf(aabbMin.x, currRight));
		return wasLeft & isNotLeft;
	};

	const XMFLOAT2 aabbMin = aabb.min();
	const XMFLOAT2 aabbMax = aabb.max();

	CollisionData collisionData;
	collisionData.fromRight = collideFromRight(aabbMax, currBallAABBMin, lastBallAABBMin);
	collisionData.fromLeft = collideFromLeft(aabbMin, currBallAABBMax, lastBallAABBMax);
	collisionData.fromTop = collideFromTop(aabbMax, currBallAABBMin, lastBallAABBMin);
	collisionData.fromBottom = collideFromBottom(aabbMin, currBallAABBMax, lastBallAABBMax);

	return collisionData;
}

void ArkanoidLogic::checkBricksCollision(const AABB& currBallAABB,
										 const XMFLOAT2& currBallAABBMin, const XMFLOAT2& currBallAABBMax,
										 const XMFLOAT2& lastBallAABBMin, const XMFLOAT2& lastBallAABBMax)
{

	const unsigned int ballCollidersCount = m_quadtree.findPotentialColliders(currBallAABB, m_ballColliders);

	for (unsigned int colliderIndex = 0; colliderIndex < ballCollidersCount; ++colliderIndex)
	{
		const unsigned int brickIndex = m_ballColliders[colliderIndex];
		assert(brickIndex < gk_bricksCount);

		XMFLOAT4& brickTranslateAndScale = brickTransform(brickIndex);
		const XMFLOAT2 brickAABBCenter{ brickTranslateAndScale.x, brickTranslateAndScale.y };
		const AABB aabb = AABB::computeFromCenterAndHalfExtents(brickAABBCenter, gk_bricksHalfExtents);

		if (currBallAABB.intersects(aabb))
		{
			CollisionData collisionData =
				ballAABBCollisionData(aabb, currBallAABBMin, currBallAABBMax, lastBallAABBMin, lastBallAABBMax);

			//change ball velocity (simple reflection)

			const unsigned int reverseXVelocity = collisionData.fromRight | collisionData.fromLeft;
			const unsigned int reverseYVelocity = collisionData.fromTop | collisionData.fromBottom;

			const float velocitiesX[2] = { m_ballVelocity.x, -m_ballVelocity.x };
			const float velocitiesY[2] = { m_ballVelocity.y, -m_ballVelocity.y };

			m_ballVelocity.x = velocitiesX[reverseXVelocity];
			m_ballVelocity.y = velocitiesY[reverseYVelocity];

			//adjust ball position making it outside the brick

			XMFLOAT4& ballTranslationAndScale = ballTransform();

			const float ballPosXRight[2] = { ballTranslationAndScale.x, brickAABBCenter.x + gk_bricksHalfExtents.x + gk_ballHalfExtents.x };

			ballTranslationAndScale.x = ballPosXRight[collisionData.fromRight];

			const float ballPosXLeft[2] = { ballTranslationAndScale.x, brickAABBCenter.x - gk_bricksHalfExtents.x - gk_ballHalfExtents.x };

			ballTranslationAndScale.x = ballPosXLeft[collisionData.fromLeft];

			const float ballPosYTop[2] = { ballTranslationAndScale.y, brickAABBCenter.y + gk_bricksHalfExtents.y + gk_ballHalfExtents.y };

			ballTranslationAndScale.y = ballPosYTop[collisionData.fromTop];

			const float ballPosYBottom[2] = { ballTranslationAndScale.y, brickAABBCenter.y - gk_bricksHalfExtents.y - gk_ballHalfExtents.y };

			ballTranslationAndScale.y = ballPosYBottom[collisionData.fromBottom];

			if (--brickRemainingHits(brickIndex) == 0)
			{
				translateOutOfArena(brickTranslateAndScale);
				handleSpawnBonus(brickAABBCenter);
			}

			break;
		}
	}
}

void ArkanoidLogic::handleSpawnBonus(const XMFLOAT2& spawnPosition)
{
	++m_bonusBricksHit;

	if (m_bonusBricksHit == m_nextBonusBricksHitCount)
	{
		m_bonusBricksHit = 0;
		m_nextBonusBricksHitCount = generateNextBonusBricksHitCount();

		const unsigned int wasBonusAlive = static_cast<unsigned int>(m_bonusAlive);

		m_bonusAlive = true;

		//if wasBonusAlive the bonus position needs not to be changed to spawnPosition
		XMFLOAT4& bonusTransformAndScale = bonusTransform();
		bonusTransformAndScale.x = static_cast<float>(1 - wasBonusAlive)*spawnPosition.x + static_cast<float>(wasBonusAlive)*bonusTransformAndScale.x;
		bonusTransformAndScale.y = static_cast<float>(1 - wasBonusAlive)*spawnPosition.y + static_cast<float>(wasBonusAlive)*bonusTransformAndScale.y;
	}
}


void ArkanoidLogic::fillUVTransforms(unsigned int atlasWidth, unsigned int atlasHeight)
{
	std::ifstream atlasDescriptorFile{ gk_texturesPath + "atlas.txt" };

	if (!atlasDescriptorFile.is_open())
	{
		assert(false);
		return;
	}
	
	using EntityIndicesPair = std::pair<std::string, unsigned int>;
	std::unordered_map<std::string, unsigned int> entityIndices
	{
		EntityIndicesPair{ "brick", gk_bricksUVTransformIndex },
		EntityIndicesPair{ "ball", gk_ballUVTransformIndex },
		EntityIndicesPair{ "vaus", gk_playerUVTransformIndex },
		EntityIndicesPair{ "arena", gk_arenaUVTransformIndex },
		EntityIndicesPair{ "bonus", gk_bonusUVTransformIndex }
	};
		
	for (unsigned int entity = 0; entity < gk_entitiesCount; ++entity)
	{
		std::string textureName;
		atlasDescriptorFile >> textureName;

		char ignore;
		atlasDescriptorFile >> ignore;

		float x;
		float y;
		float width;
		float height;

		atlasDescriptorFile >> x >> y >> width >> height;
				
		TextureTileInfo textureTileInfo{ static_cast<float>(x),
										 static_cast<float>(y),
										 static_cast<float>(width),
										 static_cast<float>(height) };

		textureTileInfo.transformToUVSpace(atlasWidth, atlasHeight);

		const auto entityIndicesIt = entityIndices.find(textureName);
		assert(entityIndicesIt != entityIndices.end());

		const unsigned int entityIndex = entityIndicesIt->second;

		XMFLOAT4& uvTransform = m_colorsAndUVTransforms.uvTranslationAndScales[entityIndex];
		uvTransform = XMFLOAT4{ textureTileInfo.x(),
								textureTileInfo.y(),
								textureTileInfo.width(),
								textureTileInfo.height() };
	}	
}

void ArkanoidLogic::assignBrickType(unsigned int brickIndex, unsigned int brickTypeIndex)
{
	brickColorAndUVTransform(brickIndex) = gk_bricksColors[brickTypeIndex];
	brickRemainingHits(brickIndex) = gk_bricksHitsCounts[brickTypeIndex];
}

unsigned int ArkanoidLogic::placeBricksRowByRow(AABB& bricksAABB)
{
	constexpr unsigned int columnsCount = gk_arenaWidth / static_cast<unsigned int>(gk_bricksWidth);
	constexpr unsigned int rowsCount = gk_bricksCount / columnsCount;

	XMFLOAT4 currPos{ gk_bricksHalfWidth, gk_arenaMaxY - gk_bricksHeight*2.0f, gk_bricksHalfWidth, gk_bricksHalfHeight };
	
	unsigned int brickIndex = 0;

	for (unsigned int row = 0; row < rowsCount; ++row)
	{
		const unsigned int brickTypeIndex = row % gk_brickTypesCount;

		for (unsigned int column = 0; column < columnsCount / 2; ++column)
		{
			assert(brickIndex < gk_bricksCount);
			unsigned int index = brickIndex++;

			brickTransform(index) = currPos;
			assignBrickType(index, brickTypeIndex);
			
			assert(brickIndex < gk_bricksCount);
			index = brickIndex++;

			brickTransform(index) = currPos;
			//reflect about y axis
			brickTransform(index).x = -currPos.x;
			assignBrickType(index, brickTypeIndex);

			currPos.x += gk_bricksWidth;
		}
		currPos.x = gk_bricksHalfWidth;
		currPos.y -= gk_bricksHeight;
	}

	const XMFLOAT2 aabbMin{ static_cast<float>(gk_arenaMinX), currPos.y };
	const XMFLOAT2 aabbMax{ static_cast<float>(gk_arenaMaxX), gk_arenaMaxY - gk_bricksHeight };

	bricksAABB = AABB::computeFromMinMax(aabbMin, aabbMax);

	return brickIndex;
}

unsigned int ArkanoidLogic::placeBricksDiamond(AABB& bricksAABB)
{
	unsigned int brickIndex = 0;

	auto placeHalfDiamond = [this, &brickIndex](XMFLOAT4& currPos,
												unsigned int rowsCount,
												unsigned int columnsCount,
												float rowIncrementMultiplier)
	{
		unsigned int currColumnsCount = columnsCount;
		for (unsigned int row = 0; currColumnsCount > 0 && row < rowsCount; ++row)
		{
			const unsigned int brickTypeIndex = row % gk_brickTypesCount;

			for (unsigned int column = 0; column < currColumnsCount/2; ++column)
			{
				assert(brickIndex < gk_bricksCount);
				unsigned int index = brickIndex++;

				brickTransform(index) = currPos;
				assignBrickType(index, brickTypeIndex);

				assert(brickIndex < gk_bricksCount);
				index = brickIndex++;

				brickTransform(index) = currPos;
				//reflect about y axis
				brickTransform(index).x = -currPos.x;
				assignBrickType(index, brickTypeIndex);

				currPos.x += gk_bricksWidth;
			}
			currPos.x = gk_bricksHalfWidth;
			currPos.y += rowIncrementMultiplier * gk_bricksHeight;
			currColumnsCount -= 2;
		}
	};

	constexpr unsigned int columnsCount = gk_arenaWidth / static_cast<unsigned int>(gk_bricksWidth);
	constexpr unsigned int rowsCount = gk_bricksCount / columnsCount; //like a box
	constexpr unsigned int halfRowsCount = rowsCount / 2;

	const float startX = gk_bricksHalfWidth;
	const float startY = gk_arenaMaxY - gk_bricksHeight*(halfRowsCount + 2);

	XMFLOAT4 currPos{ startX, startY, gk_bricksHalfWidth, gk_bricksHalfHeight };

	placeHalfDiamond(currPos, halfRowsCount, columnsCount, 1.0f);

	const float maxY = currPos.y;

	currPos.x = startX;
	currPos.y = startY - gk_bricksHeight;

	placeHalfDiamond(currPos, halfRowsCount, columnsCount, -1.0f);
	
	const XMFLOAT2 aabbMin{ static_cast<float>(gk_arenaMinX), currPos.y };
	const XMFLOAT2 aabbMax{ static_cast<float>(gk_arenaMaxX), maxY };

	bricksAABB = AABB::computeFromMinMax(aabbMin, aabbMax);

	return brickIndex;
}

unsigned int ArkanoidLogic::placeBricksColumnsByColumns(AABB& bricksAABB)
{
	constexpr unsigned int availableColumnsCount = gk_arenaWidth / static_cast<unsigned int>(gk_bricksWidth);
	constexpr unsigned int columnsCount = 6;
	constexpr unsigned int rowsCount = gk_bricksCount / columnsCount;

	constexpr float columnsSpacing = gk_bricksWidth *( static_cast<float>(availableColumnsCount - columnsCount)/(columnsCount - 1) + 1.0f);

	constexpr float startX = gk_arenaMinX + gk_bricksHalfWidth;

	XMFLOAT4 currPos{ startX, gk_arenaMaxY - gk_bricksHeight*2.0f, gk_bricksHalfWidth, gk_bricksHalfHeight };
	
	unsigned int brickIndex = 0;

	for (unsigned int row = 0; row < rowsCount; ++row)
	{
		const unsigned int brickTypeIndex = row % gk_brickTypesCount;
		
		for (unsigned int column = 0; column < columnsCount; ++column)
		{
			assert(brickIndex < gk_bricksCount);
			unsigned int index = brickIndex++;

			brickTransform(index) = currPos;
			assignBrickType(index, brickTypeIndex);

			currPos.x += columnsSpacing;
		}
		currPos.x = startX;
		currPos.y -= gk_bricksHeight;
	}

	const XMFLOAT2 aabbMin{ static_cast<float>(gk_arenaMinX), currPos.y };
	const XMFLOAT2 aabbMax{ static_cast<float>(gk_arenaMaxX), static_cast<float>(gk_arenaMaxY) - gk_bricksHeight };

	bricksAABB = AABB::computeFromMinMax(aabbMin, aabbMax);

	return brickIndex;
}

void ArkanoidLogic::onBrickShuffleKeyUp()
{
	restartLevel();	
}

void ArkanoidLogic::translateOutOfArena(XMFLOAT4& transform)
{
	transform.x = gk_outOfArenaX;
}

XMFLOAT4& ArkanoidLogic::ballTransform()
{
	return m_transforms.translationAndScales[gk_ballIndex];
}

XMFLOAT4& ArkanoidLogic::playerTransform()
{
	return m_transforms.translationAndScales[gk_playerIndex];
}

const XMFLOAT4& ArkanoidLogic::ballTransform()const
{
	return m_transforms.translationAndScales[gk_ballIndex];
}

const XMFLOAT4& ArkanoidLogic::playerTransform()const
{
	return m_transforms.translationAndScales[gk_playerIndex];
}

XMFLOAT4& ArkanoidLogic::arenaTransform()
{
	return m_transforms.translationAndScales[gk_arenaIndex];
}

const XMFLOAT4& ArkanoidLogic::arenaTransform()const
{
	return m_transforms.translationAndScales[gk_arenaIndex];
}

XMFLOAT4& ArkanoidLogic::bonusTransform()
{
	return m_transforms.translationAndScales[gk_bonusIndex];
}

const XMFLOAT4& ArkanoidLogic::bonusTransform()const
{
	return m_transforms.translationAndScales[gk_bonusIndex];
}

XMFLOAT4& ArkanoidLogic::brickTransform(unsigned int brickIndex)
{
	return m_transforms.translationAndScales[gk_bricksStartIndex + brickIndex];
}

const XMFLOAT4& ArkanoidLogic::brickTransform(unsigned int brickIndex)const
{
	return m_transforms.translationAndScales[gk_bricksStartIndex + brickIndex];
}

XMFLOAT4& ArkanoidLogic::ballColorAndUVTransform()
{
	return m_colorsAndUVTransforms.colorScaleAndIndex[gk_ballIndex];
}

const XMFLOAT4 & ArkanoidLogic::ballColorAndUVTransform() const
{
	return m_colorsAndUVTransforms.colorScaleAndIndex[gk_ballIndex];
}

XMFLOAT4 & ArkanoidLogic::playerColorAndUVTransform()
{
	return m_colorsAndUVTransforms.colorScaleAndIndex[gk_playerIndex];
}

const XMFLOAT4& ArkanoidLogic::playerColorAndUVTransform() const
{
	return m_colorsAndUVTransforms.colorScaleAndIndex[gk_playerIndex];
}

XMFLOAT4& ArkanoidLogic::arenaColorAndUVTransform()
{
	return m_colorsAndUVTransforms.colorScaleAndIndex[gk_arenaIndex];
}

const XMFLOAT4& ArkanoidLogic::arenaColorAndUVTransform() const
{
	return m_colorsAndUVTransforms.colorScaleAndIndex[gk_arenaIndex];
}

XMFLOAT4& ArkanoidLogic::bonusColorAndUVTransform()
{
	return m_colorsAndUVTransforms.colorScaleAndIndex[gk_bonusIndex];
}

const XMFLOAT4& ArkanoidLogic::bonusColorAndUVTransform()const
{
	return m_colorsAndUVTransforms.colorScaleAndIndex[gk_bonusIndex];
}

XMFLOAT4& ArkanoidLogic::brickColorAndUVTransform(unsigned int brickIndex)
{
	return m_colorsAndUVTransforms.colorScaleAndIndex[gk_bricksStartIndex + brickIndex];
}

const XMFLOAT4& ArkanoidLogic::brickColorAndUVTransform(unsigned int brickIndex)const
{
	return m_colorsAndUVTransforms.colorScaleAndIndex[gk_bricksStartIndex + brickIndex];
}

XMFLOAT4& ArkanoidLogic::ballUVTransform()
{
	return m_colorsAndUVTransforms.uvTranslationAndScales[gk_ballUVTransformIndex];
}

const XMFLOAT4& ArkanoidLogic::ballUVTransform()const
{
	return m_colorsAndUVTransforms.uvTranslationAndScales[gk_ballUVTransformIndex];
}

XMFLOAT4& ArkanoidLogic::playerUVTransform()
{
	return m_colorsAndUVTransforms.uvTranslationAndScales[gk_playerUVTransformIndex];
}

const XMFLOAT4& ArkanoidLogic::playerUVTransform()const
{
	return m_colorsAndUVTransforms.uvTranslationAndScales[gk_playerUVTransformIndex];
}

XMFLOAT4& ArkanoidLogic::arenaUVTransform()
{
	return m_colorsAndUVTransforms.uvTranslationAndScales[gk_arenaUVTransformIndex];
}

const XMFLOAT4& ArkanoidLogic::arenaUVTransform()const
{
	return m_colorsAndUVTransforms.uvTranslationAndScales[gk_arenaUVTransformIndex];
}

XMFLOAT4& ArkanoidLogic::bonusUVTransform()
{
	return m_colorsAndUVTransforms.uvTranslationAndScales[gk_bonusUVTransformIndex];
}

const XMFLOAT4& ArkanoidLogic::bonusUVTransform()const
{
	return m_colorsAndUVTransforms.uvTranslationAndScales[gk_bonusUVTransformIndex];
}

XMFLOAT4& ArkanoidLogic::bricksUVTransform()
{
	return m_colorsAndUVTransforms.uvTranslationAndScales[gk_bricksUVTransformIndex];
}

const XMFLOAT4& ArkanoidLogic::bricksUVTransform()const
{
	return m_colorsAndUVTransforms.uvTranslationAndScales[gk_bricksUVTransformIndex];
}

unsigned int& ArkanoidLogic::brickRemainingHits(unsigned int brickIndex)
{
	return m_bricksRemainingHits[brickIndex];
}

const unsigned int& ArkanoidLogic::brickRemainingHits(unsigned int brickIndex)const
{
	return m_bricksRemainingHits[brickIndex];
}
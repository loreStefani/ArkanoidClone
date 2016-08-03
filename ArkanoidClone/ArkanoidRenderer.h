#pragma once
#include "Engine.h"
#include "MathCommon.h"
#include "IDType.h"
#include "Dimensions.h"

namespace ArkanoidGame
{
	class ArkanoidLogic;

	class ArkanoidRenderer
	{
	public:
		//ctors
		explicit ArkanoidRenderer(ArkanoidLogic&);

		//dtor
		~ArkanoidRenderer();

		//copy
		ArkanoidRenderer(const ArkanoidRenderer&) = delete;
		ArkanoidRenderer& operator=(const ArkanoidRenderer&) = delete;

		//move
		ArkanoidRenderer(ArkanoidRenderer&&) = delete;
		ArkanoidRenderer& operator=(ArkanoidRenderer&&) = delete;
				
		struct TransformsConstantBuffer
		{
			XMFLOAT4 translationAndScales[gk_instancesCount];
		};

		struct ColorsAndUVTransformsConstantBuffer
		{
			//colorScaleAndIndex.w == index of uv transform
			XMFLOAT4 colorScaleAndIndex[gk_instancesCount];
			XMFLOAT4 uvTranslationAndScales[gk_entitiesCount];
		};
		
		void onCameraChanged();
		void onColorsOrUVTransformsChanged();
		void render();
				
	private:		
		void prepareForRendering();

		ArkanoidLogic& m_arkanoid;
		IDType m_meshID{};
		IDType m_pipelineStateID{};
		IDType m_vertexShaderID{};
		IDType m_pixelShaderID{};
		IDType m_sceneConstantBufferID{};
		IDType m_transformsConstantBufferID{};
		IDType m_colorsAndUVTransformsConstantBufferID{};
		IDType m_pipelineStateDataID{};
		IDType m_textureID{};
	};
}
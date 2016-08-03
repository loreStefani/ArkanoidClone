#include "ArkanoidRenderer.h"
#include "ArkanoidLogic.h"
#include "Application.h"
#include "Mesh.h"
#include "PipelineState.h"
#include "PipelineStateData.h"
#include "Camera.h"
#include "Image.h"
#include "HLSLUtils.h"
#include "ShaderCompilationConfig.h"

using namespace ArkanoidGame;

static Mesh createQuadMesh()
{
	Mesh mesh{};

	XMFLOAT3 vertices[]{
		{ -1.0f, -1.0f, 0.0f },
		{ -1.0f, +1.0f, 0.0f },
		{ +1.0f, +1.0f, 0.0f },
		{ +1.0f, -1.0f, 0.0f }
	};

	mesh.setPositions(vertices, sizeof(vertices) / sizeof(XMFLOAT3));

	XMFLOAT2 textCoords[]{
		{ 0.0f, 1.0f },
		{ 0.0f, 0.0f },
		{ 1.0f, 0.0f },
		{ 1.0f, 1.0f }
	};

	mesh.setTextCoords(textCoords, sizeof(textCoords) / sizeof(XMFLOAT2));

	uint64_t indices[]{
		0, 1, 2,
		0, 2, 3
	};

	mesh.setIndices(indices, sizeof(indices) / sizeof(uint64_t));

	return mesh;
}


ArkanoidRenderer::ArkanoidRenderer(ArkanoidLogic& arkanoid) : m_arkanoid{ arkanoid }
{
	Renderer& renderer = m_arkanoid.application().renderer();

	Mesh mesh = createQuadMesh();
	m_meshID = renderer.createMesh(mesh, EVertexType::POSITION_TEXTCOORD);
	
	m_sceneConstantBufferID = renderer.createConstantBuffer(sizeof(XMFLOAT4X4));
	m_transformsConstantBufferID = renderer.createConstantBuffer(sizeof(TransformsConstantBuffer));
	m_colorsAndUVTransformsConstantBufferID = renderer.createConstantBuffer(sizeof(ColorsAndUVTransformsConstantBuffer));

	ShaderCompilationConfig shaderCompilationConfig{};
	shaderCompilationConfig.define("INSTANCES_COUNT", std::to_string(gk_instancesCount));
	shaderCompilationConfig.define("ENTITIES_COUNT", std::to_string(gk_entitiesCount));
	m_vertexShaderID = renderer.createVertexShaderFromSourceFile("everyOneVertexShader.hlsl", EVertexType::POSITION_TEXTCOORD, &shaderCompilationConfig);
	m_pixelShaderID = renderer.createPixelShaderFromSourceFile("everyOnePixelShader.hlsl", &shaderCompilationConfig);

	PipelineState pipelineState{ m_vertexShaderID, m_pixelShaderID };
	pipelineState.setStageConstantBuffers(EPipelineStage::VERTEX_SHADER, {m_sceneConstantBufferID, m_transformsConstantBufferID});
	pipelineState.setStageConstantBuffers(EPipelineStage::PIXEL_SHADER, {m_colorsAndUVTransformsConstantBufferID});

	DepthState noDepthTest{ false };
	pipelineState.setDepthState(noDepthTest);
	
	m_pipelineStateID = renderer.createPipelineState(pipelineState);

	Image atlas = Image::loadImageFromFile("atlas.png");
	m_textureID = renderer.createTextureFromImage(atlas);

	m_arkanoid.fillUVTransforms(atlas.width(), atlas.height());

	PipelineStateData pipelineStateData{ m_textureID };

	m_pipelineStateDataID = renderer.createPipelineStateData(pipelineStateData);
	
	onCameraChanged();
	onColorsOrUVTransformsChanged();

	prepareForRendering();
}

ArkanoidRenderer::~ArkanoidRenderer()
{
	Renderer& renderer = m_arkanoid.application().renderer();

	renderer.destroyPipelineStateData(m_pipelineStateDataID);
	renderer.destroyTexture(m_textureID);

	renderer.destroyPipelineState(m_pipelineStateID);
	renderer.destroyConstantBuffer(m_sceneConstantBufferID);
	renderer.destroyConstantBuffer(m_transformsConstantBufferID);
	renderer.destroyConstantBuffer(m_colorsAndUVTransformsConstantBufferID);
	renderer.destroyShader(m_vertexShaderID);
	renderer.destroyShader(m_pixelShaderID);

	renderer.destroyMesh(m_meshID);
}

void ArkanoidRenderer::prepareForRendering()
{
	Renderer& renderer = m_arkanoid.application().renderer();

	renderer.setPipelineState(m_pipelineStateID);
	renderer.setPipelineStateData(m_pipelineStateDataID);	
}

void ArkanoidRenderer::onCameraChanged()
{
	Renderer& renderer = m_arkanoid.application().renderer();

	XMFLOAT4X4 viewProj = m_arkanoid.camera().viewProj();
	prepareMatrixForHLSL(&viewProj);
	renderer.updateConstantBuffer(m_sceneConstantBufferID, &viewProj, sizeof(XMFLOAT4X4));
}

void ArkanoidRenderer::onColorsOrUVTransformsChanged()
{
	Renderer& renderer = m_arkanoid.application().renderer();

	renderer.updateConstantBuffer(m_colorsAndUVTransformsConstantBufferID,
								  m_arkanoid.colorsAndUVTransforms(),
								  sizeof(ColorsAndUVTransformsConstantBuffer));
}

void ArkanoidRenderer::render()
{
	Renderer& renderer = m_arkanoid.application().renderer();
	renderer.updateConstantBuffer(m_transformsConstantBufferID,
								  m_arkanoid.transforms(),
								  sizeof(TransformsConstantBuffer));

	renderer.renderMeshInstanced(m_meshID, gk_instancesCount);
}
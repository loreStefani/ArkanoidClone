#include "Renderer.h"
#include <cassert>
#include <d3d11.h>
#include "D3DUtils.h"
#include "ShaderUtils.h"
#include "../Resources.h"
#include "../Utils.h"
#include "../Mesh.h"
#include "../PipelineState.h"
#include "../Image.h"
#include "../PipelineStateData.h"
#include "InputLayoutHelper.h"
#include "BuffersHelper.h"
#include "DepthStateHelper.h"


using namespace ArkanoidEngine;
using namespace ArkanoidEngine::D3D11;

static IDType::NativeIDType g_nextID = 0;

static IDType generateMeshID()
{
	return IDType{ g_nextID++ };
}

static IDType generateShaderID()
{
	return generateMeshID();
}

static IDType generateConstantBufferID()
{
	return generateMeshID();
}

static IDType generatePipelineStateID()
{
	return generateMeshID();
}

static IDType generateTextureID()
{
	return generateMeshID();
}

static IDType generatePipelineStateDataID()
{
	return generateMeshID();
}


Renderer::Renderer(HWND windowHandle, UINT windowWidth, UINT windowHeight)
{
	constexpr D3D_FEATURE_LEVEL deviceFeatureLevel = D3D_FEATURE_LEVEL_11_0;

	UINT deviceCreationFlags = 0;
#ifdef _DEBUG
	deviceCreationFlags = D3D11_CREATE_DEVICE_DEBUG;
#endif

	HRESULT_CHECK(D3D11CreateDevice(nullptr, //default adapter
					  D3D_DRIVER_TYPE_HARDWARE,
					  NULL,
					  deviceCreationFlags,
					  &deviceFeatureLevel, 1,
					  D3D11_SDK_VERSION,
					  &m_device,
					  nullptr,
					  &m_deviceContext));
	
	//currently the renderer supports only triangle lists
	m_deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	createSwapChain(windowHandle, windowWidth, windowHeight);
	
	createSamplerState();
		
	onResize(windowHandle, windowWidth, windowHeight);		
}

void Renderer::createSwapChain(HWND windowHandle, UINT windowWidth, UINT windowHeight)
{
	DXGI_SWAP_CHAIN_DESC swapChainDesc{};

	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	swapChainDesc.BufferDesc.Width = windowWidth;
	swapChainDesc.BufferDesc.Height = windowHeight;
	swapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
	swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;

	swapChainDesc.BufferCount = 1;
	swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.Flags = 0;
	swapChainDesc.OutputWindow = windowHandle;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	swapChainDesc.Windowed = true;

	//MSAA disabled
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.SampleDesc.Quality = 0;

	//create swap chain
	IDXGIDevice* dxgiDevice = nullptr;
	HRESULT_CHECK(m_device->QueryInterface(__uuidof(IDXGIDevice), reinterpret_cast<void **>(&dxgiDevice)));

	IDXGIAdapter* dxgiAdapter = nullptr;
	HRESULT_CHECK(dxgiDevice->GetAdapter(&dxgiAdapter));
	COM_RELEASE(dxgiDevice);

	IDXGIFactory* dxgiFactory = nullptr;
	HRESULT_CHECK(dxgiAdapter->GetParent(__uuidof(IDXGIFactory), reinterpret_cast<void **>(&dxgiFactory)));
	COM_RELEASE(dxgiAdapter);

	HRESULT_CHECK(dxgiFactory->CreateSwapChain(m_device, &swapChainDesc, &m_swapChain));
	COM_RELEASE(dxgiFactory);
}

void Renderer::createSamplerState()
{
	D3D11_SAMPLER_DESC samplerDesc{};
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.MinLOD = 0.0f;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;

	HRESULT_CHECK(m_device->CreateSamplerState(&samplerDesc, &m_samplerState));

	m_deviceContext->PSSetSamplers(0, 1, &m_samplerState);
}

Renderer::~Renderer()
{
	auto releaseResourceMapElements = [](auto& map)
	{
		for (auto& pair : map)
		{
			COM_RELEASE(pair.second);
		}
	};

	assert(m_vertexShaders.size() == 0 && "Exist Vertex Shaders not destroyed");
	releaseResourceMapElements(m_vertexShaders);

	assert(m_pixelShaders.size() == 0 && "Exist Pixel Shaders not destroyed");
	releaseResourceMapElements(m_pixelShaders);

	assert(m_constantBuffers.size() == 0 && "Exist Constant Buffers not destroyed");
	releaseResourceMapElements(m_constantBuffers);

	assert(m_texture2DSRVs.size() == 0 && "Exist Texture2Ds not destroyed");
	releaseResourceMapElements(m_texture2DSRVs);

	assert(m_inputLayouts.size() == 0);
	releaseResourceMapElements(m_inputLayouts);

	auto destroyResourceMapElements = [this](auto& map, void (Renderer::*destroyFun)(IDType resourceID))
	{
		while (!map.empty())
		{
			(this->*destroyFun)(map.begin()->first);
		}
	};

	assert(m_meshes.size() == 0 && "Exist Meshes not destroyed");
	destroyResourceMapElements(m_meshes, &Renderer::destroyMesh);

	assert(m_pipelineStates.size() == 0 && "Exist PipelineStates not destroyed");
	destroyResourceMapElements(m_pipelineStates, &Renderer::destroyPipelineState);

	assert(m_pipelineStateDatas.size() == 0 && "Exist PipelineStateDatas not destroyed");
	destroyResourceMapElements(m_pipelineStateDatas, &Renderer::destroyPipelineStateData);

	COM_RELEASE(m_samplerState);

	COM_RELEASE(m_depthStencilView);
	COM_RELEASE(m_depthBuffer);
	COM_RELEASE(m_renderTargetView);
	COM_RELEASE(m_swapChain);
	COM_RELEASE(m_deviceContext);

#ifdef _DEBUG
	ID3D11Debug* debugLayer = nullptr;
	HRESULT hr = m_device->QueryInterface(__uuidof(ID3D11Debug), reinterpret_cast<void**>(&debugLayer));
	COM_RELEASE(m_device);
	debugLayer->ReportLiveDeviceObjects(D3D11_RLDO_DETAIL);
	COM_RELEASE(debugLayer);
#else
	COM_RELEASE(m_device);
#endif
}

void Renderer::onResize(HWND windowHandle, UINT windowWidth, UINT windowHeight)
{
	COM_RELEASE(m_renderTargetView);
	COM_RELEASE(m_depthBuffer);
	COM_RELEASE(m_depthStencilView);
	
	HRESULT_CHECK(m_swapChain->ResizeBuffers(1, windowWidth, windowHeight, DXGI_FORMAT_R8G8B8A8_UNORM, 0));
	
	//create render target and view
	ID3D11Texture2D* backBuffer = nullptr;
	HRESULT_CHECK(m_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&backBuffer)));

	D3D11_RENDER_TARGET_VIEW_DESC rtvDesc{};
	rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	rtvDesc.Texture2D.MipSlice = 0;
	rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;

	HRESULT_CHECK(m_device->CreateRenderTargetView(backBuffer, &rtvDesc, &m_renderTargetView));
	
	//create depth buffer and view
	D3D11_TEXTURE2D_DESC depthBufferDesc{};
	depthBufferDesc.ArraySize = 1;
	depthBufferDesc.Width = windowWidth;
	depthBufferDesc.Height = windowHeight;
	depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthBufferDesc.CPUAccessFlags = 0;
	depthBufferDesc.Format = DXGI_FORMAT_D16_UNORM;
	depthBufferDesc.MipLevels = 1;
	depthBufferDesc.MiscFlags = 0;
	depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;

	//use the same SampleDesc as the back buffer
	D3D11_TEXTURE2D_DESC backBufferDesc;
	backBuffer->GetDesc(&backBufferDesc);

	depthBufferDesc.SampleDesc = backBufferDesc.SampleDesc;
	
	COM_RELEASE(backBuffer);
	
	HRESULT_CHECK(m_device->CreateTexture2D(&depthBufferDesc, nullptr, &m_depthBuffer));
	HRESULT_CHECK(m_device->CreateDepthStencilView(m_depthBuffer, nullptr, &m_depthStencilView));

	m_deviceContext->OMSetRenderTargets(1, &m_renderTargetView, m_depthStencilView);
	
	D3D11_VIEWPORT viewport{};
	viewport.Width = static_cast<FLOAT>(windowWidth);
	viewport.Height = static_cast<FLOAT>(windowHeight);
	viewport.MaxDepth = 1.0f;

	m_deviceContext->RSSetViewports(1, &viewport);
}

void Renderer::beginFrame()const
{
	float clearColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
	m_deviceContext->ClearRenderTargetView(m_renderTargetView, clearColor);
	m_deviceContext->ClearDepthStencilView(m_depthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);
}

void Renderer::endFrame()const
{
#ifdef ENABLE_VSYNC
	HRESULT_CHECK(m_swapChain->Present(1, 0));
#else
	HRESULT_CHECK(m_swapChain->Present(0, 0));
#endif
}

IDType Renderer::createPipelineState(const PipelineState& pipelineState)
{
	assert(pipelineState.vertexShaderID().isValid());
	assert(pipelineState.pixelShaderID().isValid());

	auto vertexShaderIt = findAndCheckResource(m_vertexShaders, pipelineState.vertexShaderID());
	auto inputLayoutIDIt = findAndCheckResource(m_perVertexShaderInputLayoutID, pipelineState.vertexShaderID());
	auto pixelShaderIt = findAndCheckResource(m_pixelShaders, pipelineState.pixelShaderID());

	ID3D11VertexShader* vertexShader = vertexShaderIt->second;
	ID3D11InputLayout* inputLayout = findAndCheckResource(m_inputLayouts, inputLayoutIDIt->second)->second;
	ID3D11PixelShader* pixelShader = pixelShaderIt->second;
			
	auto findConstantBuffers = [this](const std::vector<IDType>& constantBuffersIDs)
	{
		std::vector<ID3D11Buffer*> constantBuffers{};
		constantBuffers.reserve(constantBuffersIDs.size());

		for (auto constantBufferID : constantBuffersIDs)
		{
			assert(constantBufferID.isValid());
			auto constantBufferIt = findAndCheckResource(m_constantBuffers, constantBufferID);
			constantBuffers.push_back(constantBufferIt->second);
		}

		return constantBuffers;
	};

	std::vector<ID3D11Buffer*> vertexShaderConstantBuffers = findConstantBuffers(pipelineState.vertexShaderStageConstantBuffers());
	std::vector<ID3D11Buffer*> pixelShaderConstantBuffers = findConstantBuffers(pipelineState.pixelShaderStageConstantBuffers());

	ID3D11DepthStencilState* depthStencilState = createDepthStencilState(m_device, pipelineState.depthState());

	IDType pipelineStateID = generatePipelineStateID();

	m_pipelineStates.emplace(pipelineStateID,
							 D3DPipelineState{ vertexShader, inputLayout, pixelShader,
											   vertexShaderConstantBuffers, pixelShaderConstantBuffers,
											   depthStencilState });

	COM_RELEASE(depthStencilState);
		
	return pipelineStateID;
}

void Renderer::destroyPipelineState(IDType pipelineStateID)
{
	assert(pipelineStateID.isValid());
	removeResourceMapElement(m_pipelineStates, pipelineStateID);
}

void Renderer::setPipelineState(IDType pipelineStateID)
{
	assert(pipelineStateID.isValid());
	auto pipelineStateIt = findAndCheckResource(m_pipelineStates, pipelineStateID);

	const D3DPipelineState& pipelineState = pipelineStateIt->second;

	m_deviceContext->VSSetShader(pipelineState.vertexShader(), nullptr, 0);
	m_deviceContext->IASetInputLayout(pipelineState.inputLayout());
	m_deviceContext->PSSetShader(pipelineState.pixelShader(), nullptr, 0);

	auto& vertexShaderConstantBuffers = pipelineState.vertexShaderConstantBuffers();
	
	if (!vertexShaderConstantBuffers.empty())
	{
		const UINT vertexShaderConstantBuffersCount = static_cast<UINT>(vertexShaderConstantBuffers.size());
		m_deviceContext->VSSetConstantBuffers(0, vertexShaderConstantBuffersCount, vertexShaderConstantBuffers.data());
	}

	auto& pixelShaderConstantBuffers = pipelineState.pixelShaderConstantBuffers();

	if (!pixelShaderConstantBuffers.empty())
	{
		const UINT pixelShaderConstantBuffersCount = static_cast<UINT>(pixelShaderConstantBuffers.size());
		m_deviceContext->PSSetConstantBuffers(0, pixelShaderConstantBuffersCount, pixelShaderConstantBuffers.data());
	}

	m_deviceContext->OMSetDepthStencilState(pipelineState.depthStencilState(), 0);
}

static UINT adjustConstantBufferSize(UINT requiredSize)
{
	//constant buffer size must be a multiple of 16
	constexpr UINT mask = (16 - 1);
	return (requiredSize + mask) & ~mask;
}

IDType Renderer::createConstantBuffer(unsigned int constantBufferSize)
{
	assert(constantBufferSize > 0);

	D3D11_BUFFER_DESC constantBufferDesc{};
	constantBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	constantBufferDesc.ByteWidth = adjustConstantBufferSize(constantBufferSize);
	constantBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	constantBufferDesc.Usage = D3D11_USAGE_DYNAMIC;

	ID3D11Buffer* constantBuffer = nullptr;
	HRESULT_CHECK(m_device->CreateBuffer(&constantBufferDesc, nullptr, &constantBuffer));

	IDType constantBufferID = generateConstantBufferID();

	m_constantBuffers.emplace(constantBufferID, constantBuffer);

	return constantBufferID;
}

void Renderer::destroyConstantBuffer(IDType constantBufferID)
{
	assert(constantBufferID.isValid());
	
	auto constantBufferIt = findAndCheckResource(m_constantBuffers, constantBufferID);

	constantBufferIt->second->Release();

	m_constantBuffers.erase(constantBufferIt);
}

void Renderer::updateConstantBuffer(IDType constantBufferID, const void* constantBufferData, unsigned int constantBufferDataSize)
{
	assert(constantBufferID.isValid());

	auto constantBufferIt = findAndCheckResource(m_constantBuffers, constantBufferID);
	
	ID3D11Buffer* constantBuffer = constantBufferIt->second;

	D3D11_MAPPED_SUBRESOURCE mappedConstantBuffer;
	HRESULT_CHECK(m_deviceContext->Map(constantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedConstantBuffer));

	std::memcpy(mappedConstantBuffer.pData, constantBufferData, constantBufferDataSize);

	m_deviceContext->Unmap(constantBuffer, 0);
}

void Renderer::renderMesh(IDType meshID)const
{
	assert(meshID.isValid());
	auto meshIt = findAndCheckResource(m_meshes, meshID);
	
	const D3DMesh& mesh = meshIt->second;
	
	bindMeshBuffers(mesh);

	m_deviceContext->DrawIndexed(mesh.indicesCount(), 0, 0);
}

void Renderer::renderMeshInstanced(IDType meshID, unsigned int instancesCount)const
{
	assert(meshID.isValid());
	auto meshIt = findAndCheckResource(m_meshes, meshID);

	const D3DMesh& mesh = meshIt->second;
	
	bindMeshBuffers(mesh);

	m_deviceContext->DrawIndexedInstanced(mesh.indicesCount(), instancesCount, 0, 0, 0);
}

void Renderer::bindMeshBuffers(const D3DMesh& mesh)const
{
	ID3D11Buffer* vertexBuffer = mesh.vertexBuffer();
	ID3D11Buffer* indexBuffer = mesh.indexBuffer();

	UINT offset = 0;
	UINT stride = mesh.vertexStride();
	m_deviceContext->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	m_deviceContext->IASetIndexBuffer(indexBuffer, mesh.indexFormat(), 0);
}


IDType Renderer::createMesh(const Mesh& mesh, EVertexType vertexType)
{
	VertexBufferDesc vertexBufferDesc;

	switch (vertexType)
	{
	case EVertexType::POSITION:
		vertexBufferDesc = createVertexBuffer<VertexPos>(m_device, mesh);
		break;
	case EVertexType::POSITION_NORMAL:
		vertexBufferDesc = createVertexBuffer<VertexPosNorm>(m_device, mesh);
		break;
	case EVertexType::POSITION_TEXTCOORD:
		vertexBufferDesc = createVertexBuffer<VertexPosTextCoord>(m_device, mesh);
		break;
	case EVertexType::POSITION_NORMAL_TEXTCOORD:
		vertexBufferDesc = createVertexBuffer<VertexPosNormTextCoord>(m_device, mesh);
		break;
	default:
		assert(false);
		return IDType{};
	}
	
	IndexBufferDesc indexBufferDesc;
	DXGI_FORMAT indexFormat = DXGI_FORMAT_R16_UINT;

	if (vertexBufferDesc.verticesCount > UINT16_MAX)
	{
		/*
		An index buffer contains a sequential set of 16-bit or 32-bit indices, see
		https://msdn.microsoft.com/en-us/library/windows/desktop/ff476898(v=vs.85).aspx#Index_Buffer
		*/
		assert(vertexBufferDesc.verticesCount - 1 <= UINT32_MAX);
		indexFormat = DXGI_FORMAT_R32_UINT;

		indexBufferDesc = createIndexBuffer<UINT32>(m_device, mesh);
	}
	else
	{
		indexBufferDesc = createIndexBuffer<UINT16>(m_device, mesh);
	}

	
	IDType meshID = generateMeshID();

	m_meshes.emplace(meshID, D3DMesh{ vertexBufferDesc.vertexBuffer,
									  indexBufferDesc.indexBuffer,
									  vertexBufferDesc.verticesCount,
									  vertexBufferDesc.vertexStride,
									  indexBufferDesc.indicesCount,
									  indexFormat });

	COM_RELEASE(vertexBufferDesc.vertexBuffer);
	COM_RELEASE(indexBufferDesc.indexBuffer);

	return meshID;
}

void Renderer::destroyMesh(IDType meshID)
{
	assert(meshID.isValid());
	removeResourceMapElement(m_meshes, meshID);
}

IDType Renderer::createTextureFromImage(const Image& img)
{
	D3D11_TEXTURE2D_DESC textureDesc{};
	textureDesc.ArraySize = 1;
	textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	textureDesc.Width = img.width();
	textureDesc.Height = img.height();
	textureDesc.MipLevels = 1; //TODO: handle mip mapping	
	textureDesc.SampleDesc.Count = 1;
	textureDesc.Usage = D3D11_USAGE_IMMUTABLE;

	D3D11_SUBRESOURCE_DATA textureMostDetailedMipData{};
	textureMostDetailedMipData.pSysMem = img.imageData();
	textureMostDetailedMipData.SysMemPitch = sizeof(unsigned char) * 4 * img.width();
	textureMostDetailedMipData.SysMemSlicePitch = textureMostDetailedMipData.SysMemPitch*img.width();

	ID3D11Texture2D* texture = nullptr;
	HRESULT_CHECK(m_device->CreateTexture2D(&textureDesc, &textureMostDetailedMipData, &texture));
		
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = static_cast<UINT>(-1); //all levels

	ID3D11ShaderResourceView* texture2DSRV = nullptr;
	HRESULT_CHECK(m_device->CreateShaderResourceView(texture, &srvDesc, &texture2DSRV));

	COM_RELEASE(texture);

	IDType textureID = generateTextureID();

	m_texture2DSRVs.emplace(textureID, texture2DSRV);

	return textureID;
}

IDType Renderer::createTextureFromImageFile(const std::string& imageFileName)
{
	return createTextureFromImage(Image::loadImageFromFile(imageFileName));
}

void Renderer::destroyTexture(IDType textureID)
{
	assert(textureID.isValid());

	auto texture2DSRVit = findAndCheckResource(m_texture2DSRVs, textureID);
	texture2DSRVit->second->Release();
	m_texture2DSRVs.erase(texture2DSRVit);
}

IDType Renderer::createPipelineStateData(const PipelineStateData& pipelineStateData)
{
	assert(pipelineStateData.texture2DID().isValid());
	auto texture2DSRVit = findAndCheckResource(m_texture2DSRVs, pipelineStateData.texture2DID());
		
	IDType pipelineStateDataID = generatePipelineStateDataID();
	m_pipelineStateDatas.emplace(pipelineStateDataID, D3DPipelineStateData{ texture2DSRVit->second });

	return pipelineStateDataID;
}

void Renderer::destroyPipelineStateData(IDType pipelineStateDataID)
{
	assert(pipelineStateDataID.isValid());
	removeResourceMapElement(m_pipelineStateDatas, pipelineStateDataID);
}

void Renderer::setPipelineStateData(IDType pipelineStateDataID)
{
	assert(pipelineStateDataID.isValid());
	
	auto pipelineStateDataIt = findAndCheckResource(m_pipelineStateDatas, pipelineStateDataID);
	
	ID3D11ShaderResourceView* texture2DSRV = pipelineStateDataIt->second.texture2DSRV();
	m_deviceContext->PSSetShaderResources(0, 1, &texture2DSRV);
}


IDType Renderer::createShaderFromBinary(EShaderType eShaderType,
										void* shaderBinaryBuffPtr,
										size_t shaderBinaryBuffSize)
{
	assert(shaderBinaryBuffPtr != nullptr);
	assert(shaderBinaryBuffSize > 0);

	IDType shaderID = generateShaderID();

	switch (eShaderType)
	{
	case EShaderType::VERTEX:
		{
			ID3D11VertexShader* vertexShader = nullptr;
			HRESULT_CHECK(m_device->CreateVertexShader(shaderBinaryBuffPtr, shaderBinaryBuffSize, nullptr, &vertexShader));
			m_vertexShaders.emplace(shaderID, vertexShader);
			break;
		}		
	case EShaderType::PIXEL:
		{
			ID3D11PixelShader* pixelShader = nullptr;
			HRESULT_CHECK(m_device->CreatePixelShader(shaderBinaryBuffPtr, shaderBinaryBuffSize, nullptr, &pixelShader));
			m_pixelShaders.emplace(shaderID, pixelShader);
			break;
		}
	default:
		assert(false);
	}
	
	return shaderID;
}

IDType Renderer::createShaderFromSourceFile(EShaderType eShaderType,
											const std::string& shaderSourceFileName,											
											ID3DBlob** compiledShaderBlob,
											const ShaderCompilationConfig* compilationConfig)
{
	ID3DBlob* shaderBinaryBlob = compileShaderFromFile(eShaderType, shaderSourceFileName, compilationConfig);
	assert(shaderBinaryBlob != nullptr);

	IDType id = createShaderFromBinary(eShaderType, shaderBinaryBlob->GetBufferPointer(), shaderBinaryBlob->GetBufferSize());
	
	if (compiledShaderBlob != nullptr)
	{
		*compiledShaderBlob = shaderBinaryBlob;
	}
	else
	{
		COM_RELEASE(shaderBinaryBlob);
	}

	return id;
}

IDType Renderer::createPixelShaderFromSourceFile(const std::string& shaderSourceFileName, const ShaderCompilationConfig* compilationConfig)
{
	return createShaderFromSourceFile(EShaderType::PIXEL, shaderSourceFileName, nullptr, compilationConfig);
}

IDType Renderer::createInputLayout(EVertexType vertexType, ID3DBlob* compiledShaderBlob)
{
	//we create one InputLayout for every EVertyexType once and keep a reference count.
	//this allows to control how may vertex shaders reference it and release it properly

	assert(compiledShaderBlob != nullptr);

	IDType vertexTypeID = getVertexTypeID(vertexType);
		
	auto inputLayoutIt = m_inputLayouts.find(vertexTypeID);

	if (inputLayoutIt == m_inputLayouts.end())
	{		
		ID3D11InputLayout* inputLayout = ::createInputLayout(m_device, vertexType, compiledShaderBlob);
		
		m_inputLayouts.emplace(vertexTypeID, inputLayout);

		//set to 1 the reference count
		m_perInputLayoutUsageCount.emplace(vertexTypeID, 1);

	}
	else
	{
		//add 1 to the reference count
		auto inputLayoutUsageCountIt = findAndCheckResourceToModify(m_perInputLayoutUsageCount, vertexTypeID);
		++inputLayoutUsageCountIt->second;
	}

	return vertexTypeID;
}

void Renderer::destroyInputLayout(IDType inputLayoutID)
{
	assert(inputLayoutID.isValid());

	//decrement input layout reference count and if reaches 0 destroy the former 

	auto inputLayoutUsageCountIt = findAndCheckResourceToModify(m_perInputLayoutUsageCount, inputLayoutID);

	--inputLayoutUsageCountIt->second;

	if (inputLayoutUsageCountIt->second == 0)
	{
		auto inputLayoutIt = findAndCheckResource(m_inputLayouts, inputLayoutID);

		inputLayoutIt->second->Release();

		m_inputLayouts.erase(inputLayoutIt);

		m_perInputLayoutUsageCount.erase(inputLayoutUsageCountIt);
	}
}

IDType Renderer::createVertexShaderFromSourceFile(const std::string& shaderSourceFileName, EVertexType vertexType,
												  const ShaderCompilationConfig* compilationConfig)
{
	ID3DBlob* compiledShaderBlob = nullptr;
	IDType vertexShaderID = createShaderFromSourceFile(EShaderType::VERTEX,
													   shaderSourceFileName, &compiledShaderBlob, compilationConfig);
	
	//check if the input layout has already been created, otherwise create it
	IDType inputLayoutID = createInputLayout(vertexType, compiledShaderBlob);
	
	m_perVertexShaderInputLayoutID.emplace(vertexShaderID, inputLayoutID);

	COM_RELEASE(compiledShaderBlob);

	return vertexShaderID;
}

void Renderer::destroyShader(IDType shaderID)
{
	assert(shaderID.isValid());
	auto findAndRemoveShader = [&shaderID, this](auto& shaderMap)
	{
		auto it = shaderMap.find(shaderID);

		if (it != shaderMap.end())
		{
			it->second->Release();
			shaderMap.erase(it);
			return true;
		}

		return false;		
	};

	if (!findAndRemoveShader(m_vertexShaders))
	{
		assert(findAndRemoveShader(m_pixelShaders));
	}
	else
	{
		//the shader is a vertex shader, destroy the input layout associated with it		
		auto inputLayoutIDIt = findAndCheckResource(m_perVertexShaderInputLayoutID, shaderID);
		
		IDType inputLayoutID = inputLayoutIDIt->second;
		m_perVertexShaderInputLayoutID.erase(inputLayoutIDIt);

		destroyInputLayout(inputLayoutID);		
	}
}
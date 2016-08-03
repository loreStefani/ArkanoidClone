#pragma once
#include <vector>
#include <unordered_map>
#include "D3DMesh.h"
#include "../WindowsInclude.h"
#include "EShaderType.h"
#include "../VertexTypes.h"
#include "../IDType.h"
#include "D3DPipelineState.h"
#include "../EPipelineStage.h"
#include <cassert>
#include "D3DPipelineStateData.h"

//#define ENABLE_VSYNC

struct ID3D11Device;
struct ID3D11DeviceContext;
struct IDXGISwapChain;
struct ID3D11Texture2D;
struct ID3D11ShaderResourceView;
struct ID3D11RenderTargetView;
struct ID3D11DepthStencilView;
struct ID3D11Buffer;
struct ID3D11VertexShader;
struct ID3D11PixelShader;
struct ID3D11InputLayout;
struct ID3D11SamplerState;
struct ID3D10Blob;
using ID3DBlob = ID3D10Blob;

namespace ArkanoidEngine
{
	class Mesh;
	class PipelineState;
	class PipelineStateData;
	class Image;
	class ShaderCompilationConfig;

	namespace D3D11
	{
		class Renderer
		{
		public:
			//ctors
			explicit Renderer(HWND windowHandle, UINT windowWidth, UINT windowHeight);

			//dtor
			~Renderer();

			//copy
			Renderer(const Renderer&) = delete;
			Renderer& operator=(const Renderer&) = delete;

			//move
			Renderer(Renderer&&) = delete;
			Renderer& operator=(Renderer&&) = delete;

			void beginFrame()const;
			void endFrame()const;

			void renderMesh(IDType meshID)const;
			void renderMeshInstanced(IDType meshID, unsigned int instancesCount)const;

			void onResize(HWND windowHandle, UINT windowWidth, UINT windowHeight);
			
			IDType createMesh(const Mesh& mesh, EVertexType vertexType);
			void destroyMesh(IDType meshID);
			
			IDType createVertexShaderFromSourceFile(const std::string& shaderSourceFileName,
													EVertexType vertexType,
													const ShaderCompilationConfig* compilationConfig = nullptr);
			IDType createPixelShaderFromSourceFile(const std::string& shaderSourceFileName,
												   const ShaderCompilationConfig* compilationConfig = nullptr);			
			void destroyShader(IDType shaderID);

			IDType createPipelineState(const PipelineState& pipelineState);
			void destroyPipelineState(IDType pipelineStateID);			
			void setPipelineState(IDType pipelineStateID);
			
			IDType createConstantBuffer(unsigned int constantBufferSize);
			void destroyConstantBuffer(IDType constantBufferID);
			void updateConstantBuffer(IDType constantBufferID, const void* constantBufferData, unsigned int constantBufferDataSize);

			IDType createTextureFromImageFile(const std::string& imageFileName);
			IDType createTextureFromImage(const Image& image);
			void destroyTexture(IDType textureID);
			
			IDType createPipelineStateData(const PipelineStateData& pipelineStateData);
			void destroyPipelineStateData(IDType pipelineStateDataID);
			void setPipelineStateData(IDType pipelineStateDataID);

		private:

			void createSwapChain(HWND windowHandle, UINT windowWidth, UINT windowHeight);
			void createSamplerState();

			IDType createShaderFromSourceFile(EShaderType eShaderType,
											  const std::string& shaderSourceFileName,											  
											  ID3DBlob** compiledShaderBlob,
											  const ShaderCompilationConfig* compilationConfig = nullptr);
			IDType createShaderFromBinary(EShaderType eShaderType,
										  void* shaderBinaryBuffPtr,
										  size_t shaderBinaryBuffSize);
			
			void bindMeshBuffers(const D3DMesh& mesh)const;

			IDType createInputLayout(EVertexType vertexType, ID3DBlob* compiledShaderBlob);
			void destroyInputLayout(IDType inputLayoutID);

			template<typename T>
			using ResourceMap = std::unordered_map<IDType, T>;

			template<typename T>
			using ResourceMapIterator = typename ResourceMap<T>::iterator;

			template<typename T>
			using ResourceMapConstIterator = typename ResourceMap<T>::const_iterator;
			
			template<typename T>
			ResourceMapConstIterator<T> findAndCheckResource(const ResourceMap<T>& resourceMap, IDType resourceID)const;

			template<typename T>
			ResourceMapIterator<T> findAndCheckResourceToModify(ResourceMap<T>& resourceMap, IDType resourceID);

			template<typename T>
			void removeResourceMapElement(ResourceMap<T>& resourceMap, IDType resourceID);

			ID3D11Device* m_device{ nullptr };
			ID3D11DeviceContext* m_deviceContext{ nullptr };
			IDXGISwapChain* m_swapChain{ nullptr };

			ID3D11Texture2D* m_depthBuffer{ nullptr };
			ID3D11RenderTargetView* m_renderTargetView{ nullptr };
			ID3D11DepthStencilView* m_depthStencilView{ nullptr };

			ID3D11SamplerState* m_samplerState{ nullptr };
			
			ResourceMap<ID3D11VertexShader*> m_vertexShaders{};
			ResourceMap<ID3D11PixelShader*> m_pixelShaders{};
			ResourceMap<ID3D11Buffer*> m_constantBuffers{};
			ResourceMap<ID3D11ShaderResourceView*> m_texture2DSRVs{};
			ResourceMap<ID3D11InputLayout*> m_inputLayouts{};
			ResourceMap<IDType> m_perVertexShaderInputLayoutID{};
			ResourceMap<UINT> m_perInputLayoutUsageCount{};

			ResourceMap<D3DMesh> m_meshes{};
			ResourceMap<D3DPipelineState> m_pipelineStates{};
			ResourceMap<D3DPipelineStateData> m_pipelineStateDatas{};
		};

		template<typename T>
		inline 
			Renderer::ResourceMapConstIterator<T> 
				Renderer::findAndCheckResource(const ResourceMap<T>& resourceMap, IDType resourceID)const
		{
			auto it = resourceMap.find(resourceID);
			assert(it != resourceMap.end());
			return it;
		}

		template<typename T>
		inline
			Renderer::ResourceMapIterator<T>
				Renderer::findAndCheckResourceToModify(ResourceMap<T>& resourceMap, IDType resourceID)
		{
			auto it = resourceMap.find(resourceID);
			assert(it != resourceMap.end());
			return it;
		}
						
		template<typename T>
		inline void Renderer::removeResourceMapElement(ResourceMap<T>& resourceMap, IDType resourceID)
		{
			auto it = findAndCheckResource(resourceMap, resourceID);
			resourceMap.erase(it);
		}

	}
}

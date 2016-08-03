#pragma once
#include <vector>

struct ID3D11VertexShader;
struct ID3D11PixelShader;
struct ID3D11Buffer;
struct ID3D11InputLayout;
struct ID3D11DepthStencilState;

namespace ArkanoidEngine
{
	namespace D3D11
	{
		class D3DPipelineState
		{
		public:
			//ctors
			explicit D3DPipelineState(ID3D11VertexShader* vertexShader, ID3D11InputLayout* inputLayout,
									  ID3D11PixelShader* pixelShader,							  
									  std::vector<ID3D11Buffer*> vertexShaderConstantBuffers,
									  std::vector<ID3D11Buffer*> pixelShaderConstantBuffers,
									  ID3D11DepthStencilState* depthStencilState);

			//dtor
			~D3DPipelineState();

			//copy
			D3DPipelineState(const D3DPipelineState&) = delete;
			D3DPipelineState& operator=(const D3DPipelineState&) = delete;

			//move
			D3DPipelineState(D3DPipelineState&&);
			D3DPipelineState& operator=(D3DPipelineState&&);

			ID3D11VertexShader* vertexShader()const;
			ID3D11PixelShader* pixelShader()const;

			const std::vector<ID3D11Buffer*>& vertexShaderConstantBuffers()const;
			const std::vector<ID3D11Buffer*>& pixelShaderConstantBuffers()const;

			ID3D11InputLayout* inputLayout()const;

			ID3D11DepthStencilState* depthStencilState()const;

		private:

			void release();

			std::vector<ID3D11Buffer*> m_vertexShaderConstantBuffers{};
			std::vector<ID3D11Buffer*> m_pixelShaderConstantBuffers{};
			ID3D11VertexShader* m_vertexShader{ nullptr };
			ID3D11InputLayout* m_inputLayout{ nullptr };
			ID3D11PixelShader* m_pixelShader{ nullptr };
			ID3D11DepthStencilState* m_depthStencilState{ nullptr };
		};

		inline ID3D11VertexShader* D3DPipelineState::vertexShader()const
		{
			return m_vertexShader;
		}

		inline ID3D11InputLayout* D3DPipelineState::inputLayout()const
		{
			return m_inputLayout;
		}

		inline ID3D11PixelShader* D3DPipelineState::pixelShader()const
		{
			return m_pixelShader;
		}

		inline const std::vector<ID3D11Buffer*>& D3DPipelineState::vertexShaderConstantBuffers()const
		{
			return m_vertexShaderConstantBuffers;
		}

		inline const std::vector<ID3D11Buffer*>& D3DPipelineState::pixelShaderConstantBuffers()const
		{
			return m_pixelShaderConstantBuffers;
		}

		inline ID3D11DepthStencilState* D3DPipelineState::depthStencilState()const
		{
			return m_depthStencilState;
		}
	}
}
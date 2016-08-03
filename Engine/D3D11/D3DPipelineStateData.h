#pragma once

struct ID3D11ShaderResourceView;

namespace ArkanoidEngine
{
	namespace D3D11
	{
		class D3DPipelineStateData
		{
		public:
			//ctors
			explicit D3DPipelineStateData(ID3D11ShaderResourceView* texture2DSRV);

			//dtor
			~D3DPipelineStateData();

			//copy
			D3DPipelineStateData(const D3DPipelineStateData&) = delete;
			D3DPipelineStateData& operator=(const D3DPipelineStateData&) = delete;

			//move
			D3DPipelineStateData(D3DPipelineStateData&&);
			D3DPipelineStateData& operator=(D3DPipelineStateData&&);

			ID3D11ShaderResourceView* texture2DSRV()const;

		private:
			ID3D11ShaderResourceView* m_texture2DSRV{ nullptr };
		};

		inline ID3D11ShaderResourceView* D3DPipelineStateData::texture2DSRV()const
		{
			return m_texture2DSRV;
		}
	}
}
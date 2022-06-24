#pragma once

#include "../../shader/Shader.h"
#include "Render_DX11_Common.h"

namespace sge {

	class RenderContext_DX11;

	class ShaderVertexStageBase_DX11 : public ShaderVertexStage {
	public:
		void bind(RenderContext_DX11* ctx);
	protected:
		ComPtr<DX11_ID3DInputLayout >  m_d3dInputLayout;
		ComPtr<DX11_ID3DVertexShader> m_d3dVertexShader;

	};
	class ShaderPixelStageBase_DX11  : public ShaderPixelStage {
	public:
		void bind(RenderContext_DX11* ctx);
	protected:
		ComPtr<DX11_ID3DPixelShader> m_d3dPixelShader;
	};

	template<class STAGE_BASE>
	class ShaderStage_DX11_Impl : public STAGE_BASE {
		using Util = DX11Util;
	public:
		void load(StrView passPath, DX11_ID3DDevice* dev);

	friend class ShaderPass_DX11;
	protected:
		void _createShader(DX11_ID3DDevice* dev);
	};

	using ShaderVertexStage_DX11 = ShaderStage_DX11_Impl<ShaderVertexStageBase_DX11>;
	using ShaderPixelStage_DX11	 = ShaderStage_DX11_Impl< ShaderPixelStageBase_DX11>;

	class ShaderPass_DX11 : public ShaderPass {
		using Base = ShaderPass;
	public:
		ShaderPass_DX11(StrView passPath, Info& info);

		SGE_INLINE DX11_ID3DVertexShader* dx11Vs() { return m_dx11VertexStage.m_d3dVertexShader; }
		SGE_INLINE DX11_ID3DPixelShader*  dx11Ps() { return m_dx11PixelStage.m_d3dPixelShader;   }

		ShaderVertexStage_DX11 m_dx11VertexStage;
		ShaderPixelStage_DX11   m_dx11PixelStage;
	};

	class Shader_DX11 : public Shader {
	public:
		virtual void onCreate(StrView compiledPath) override;
	};
}

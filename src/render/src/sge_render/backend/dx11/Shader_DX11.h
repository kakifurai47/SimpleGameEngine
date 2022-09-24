#pragma once

#include "../../shader/Shader.h"
#include "Render_DX11_Common.h"

namespace sge {

	class RenderContext_DX11;

	class ShaderVertexStageBase_DX11 : public ShaderVertexStage {
	protected:
		ComPtr<DX11_ID3DInputLayout >  m_d3dInputLayout;
		ComPtr<DX11_ID3DVertexShader> m_d3dVertexShader;

	};
	class ShaderPixelStageBase_DX11  : public ShaderPixelStage {
	protected:
		ComPtr<DX11_ID3DPixelShader> m_d3dPixelShader;
	};

	template<class STAGE_BASE>
	class ShaderStage_DX11_Impl : public STAGE_BASE {
		using Util = DX11Util;
	public:
		void bind(RenderContext_DX11* ctx);
		void load(StrView passPath, DX11_ID3DDevice* dev);
	protected:
		void _createShader(DX11_ID3DDevice* dev);
	};

	using ShaderVertexStage_DX11 = ShaderStage_DX11_Impl<ShaderVertexStageBase_DX11>;
	using ShaderPixelStage_DX11	 = ShaderStage_DX11_Impl< ShaderPixelStageBase_DX11>;

	class ShaderPass_DX11 : public ShaderPass {
		using This = ShaderPass_DX11;
		using Base = ShaderPass;
	public:
		ShaderPass_DX11(StrView passPath, Info& info);
		ShaderPass_DX11(ShaderPass_DX11&& other);

		ShaderVertexStage_DX11 m_dx11VertexStage;
		ShaderPixelStage_DX11   m_dx11PixelStage;
	};

	class Shader_DX11 : public Shader {
		using Base = Shader;
	public:
		using VertexStage = ShaderVertexStage_DX11;
		using  PixelStage  = ShaderPixelStage_DX11;

		Shader_DX11(StrView filename);

	protected:
		Vector<ShaderPass_DX11, 1> m_dx11ShadPasses;
	};
}

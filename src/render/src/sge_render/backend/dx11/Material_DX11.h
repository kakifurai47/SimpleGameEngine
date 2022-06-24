#pragma once

#include "../../Shader/Material.h"
#include "Shader_DX11.h"

namespace sge {	
	class RenderContext_DX11;

	class MaterialVertexStageBase_DX11 : public MaterialVertexStage {
		using This = MaterialVertexStageBase_DX11;
		using Base = MaterialVertexStage;
	public:
		using ShaderStage_DX11 = ShaderVertexStage_DX11;

		This(Base::ShaderStage* ss) : Base(ss) {}

		void setShader(RenderContext_DX11* ctx, DX11_ID3DVertexShader* vs);

	};

	class  MaterialPixelStageBase_DX11 : public  MaterialPixelStage {
		using This = MaterialPixelStageBase_DX11;
		using Base = MaterialPixelStage;
	public:
		using ShaderStage_DX11 = ShaderPixelStage_DX11;

		This(Base::ShaderStage* ss) : Base(ss) {}

	protected:


	};

	template<class STAGE_BASE>
	class MaterialStage_DX11_Impl : public STAGE_BASE {
		using This = MaterialStage_DX11_Impl;
		using Base = STAGE_BASE;
	public:
		MaterialStage_DX11_Impl(ShaderStage* ss) : Base(ss) {}

		void bind(RenderContext_DX11* ctx, const VertexLayout* layout);

	protected:
	};

	using MaterialVertexStage_DX11 = MaterialStage_DX11_Impl<MaterialVertexStageBase_DX11>;
	using  MaterialPixelStage_DX11 = MaterialStage_DX11_Impl< MaterialPixelStageBase_DX11>;

	class MaterialPass_DX11 : public MaterialPass {
		using Base = MaterialPass;

		using VertexStage_DX11 = MaterialVertexStage_DX11;
		using  PixelStage_DX11 =  MaterialPixelStage_DX11;

	public:
		MaterialPass_DX11::MaterialPass_DX11(Material* material, ShaderPass* shadPass)
			: Base(material, shadPass, m_dx11VertexStage, m_dx11PixelStage)
			, m_dx11VertexStage(shadPass->vertexStage())
			, m_dx11PixelStage (shadPass-> pixelStage())
		{
		}

	protected:		
		virtual void onBind(RenderContext* ctx, const VertexLayout* layout) override;

		VertexStage_DX11  m_dx11VertexStage;
		PixelStage_DX11   m_dx11PixelStage;
	};

	class Material_DX11 : public Material {
	public:
		Shader_DX11* dx11Shad() { return static_cast<Shader_DX11*>(m_shader.ptr()); }

	protected:
		virtual Pass* onCreatePass(ShaderPass*	shadPass) override;
	};
}
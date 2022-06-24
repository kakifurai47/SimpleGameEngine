#include "Renderer_DX11.h"
#include "Material_DX11.h"
#include "RenderContext_DX11.h"

namespace sge 
{
	void MaterialVertexStageBase_DX11::setShader(RenderContext_DX11* ctx, DX11_ID3DVertexShader* vs) {
		
	}



	template<class STAGE_BASE>
	void MaterialStage_DX11_Impl<STAGE_BASE>::bind(RenderContext_DX11* ctx, const VertexLayout* layout) {
		auto* shaderStage = static_cast<ShaderStage_DX11*>(m_shadStage);
		SGE_ASSERT(shaderStage != nullptr);
		shaderStage->bind(ctx);

		auto* dc = ctx->renderer()->d3dDeviceContext();
		for (auto& cb : m_constBufs) {
			SGE_LOG("x");
		}

	}

	template MaterialStage_DX11_Impl<MaterialVertexStageBase_DX11>;
	template MaterialStage_DX11_Impl <MaterialPixelStageBase_DX11>;


	void MaterialPass_DX11::onBind (RenderContext* ctx, const VertexLayout* layout) {
		auto* dx11Ctx = static_cast<RenderContext_DX11*>(ctx);
		m_dx11VertexStage.bind(dx11Ctx, layout);
		 m_dx11PixelStage.bind(dx11Ctx, layout);
	}

	Material::Pass* Material_DX11::onCreatePass(ShaderPass* shadPass) {
		return new MaterialPass_DX11(this, shadPass);
	}

}
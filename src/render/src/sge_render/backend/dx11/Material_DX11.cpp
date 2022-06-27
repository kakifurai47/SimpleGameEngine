#include "Renderer_DX11.h"
#include "Material_DX11.h"
#include "RenderContext_DX11.h"
#include "RenderGpuBuffer_DX11.h"

namespace sge 
{
	void MaterialVertexStageBase_DX11::setConstBuffer(DX11_ID3DDeviceContext* dc, UINT bindPoint, DX11_ID3DBuffer* d3dBuf) {
		dc->VSSetConstantBuffers1(bindPoint, 1, &d3dBuf, nullptr, nullptr);
	}

	void MaterialPixelStageBase_DX11::setConstBuffer(DX11_ID3DDeviceContext* dc, UINT bindPoint, DX11_ID3DBuffer* d3dBuf) {
		dc->PSSetConstantBuffers1(bindPoint, 1, &d3dBuf, nullptr, nullptr);
	}

	template<class STAGE_BASE>
	void MaterialStage_DX11_Impl<STAGE_BASE>::bind(RenderContext_DX11* ctx, const VertexLayout* layout) {
		auto* shaderStage = static_cast<ShaderStage_DX11*>(m_shadStage);
		SGE_ASSERT(shaderStage != nullptr);
		shaderStage->bind(ctx);

		auto* dc = ctx->renderer()->d3dDeviceContext();
		for (auto& cb : m_constBufs) {
			cb.uploadToGpu();

			auto* cbInfo = cb.info();
			UINT bindPoint = cbInfo->bindPoint;

			auto* gpuBuffer = static_cast<RenderGpuBuffer_DX11*>(cb.gpuBuffer.ptr());
			if  (!gpuBuffer) throw SGE_ERROR("const buffer is null");

			auto* d3dBuf = gpuBuffer->d3dBuf();
			if  (!d3dBuf) throw SGE_ERROR("d3dbuffer is null");
			
			setConstBuffer(dc, bindPoint, d3dBuf);
		}
	}

	template MaterialStage_DX11_Impl<MaterialVertexStageBase_DX11>;
	template MaterialStage_DX11_Impl <MaterialPixelStageBase_DX11>;

	void MaterialPass_DX11::onBind (RenderContext* ctx, const VertexLayout* layout) {
		auto* dx11Ctx = static_cast<RenderContext_DX11*>(ctx);
		m_dx11VertexStage.bind(dx11Ctx, layout);
		 m_dx11PixelStage.bind(dx11Ctx, layout);
	}

	MaterialPass_DX11::MaterialPass_DX11(Material* material, ShaderPass* shadPass)
		: Base(material, shadPass, m_dx11VertexStage, m_dx11PixelStage)
		, m_dx11VertexStage(shadPass->vertexStage())
		, m_dx11PixelStage (shadPass-> pixelStage())
	{
	}

	MaterialPass_DX11::MaterialPass_DX11(MaterialPass_DX11&& other)
		: Base(other.m_material, other.m_shadPass, m_dx11VertexStage, m_dx11PixelStage)
		, m_dx11VertexStage(std::move(other.m_dx11VertexStage))
		,  m_dx11PixelStage(std::move( other.m_dx11PixelStage))
	{
	}

	void Material_DX11::onSetShader(Span<ShaderPass*> shadPasses) {
		m_dx11Passes.clear();
		m_dx11Passes.reserve(shadPasses.size());
		for (auto* p : shadPasses) {
			m_dx11Passes.emplace_back(this, p); 
		}
	}

	void Material_DX11::onResetPasses(Vector_<Pass*, 1>& outPasses) {
		outPasses.clear();
		outPasses.reserve(outPasses.size());
		for (auto& p : m_dx11Passes) { outPasses.emplace_back(&p); }
	}
}
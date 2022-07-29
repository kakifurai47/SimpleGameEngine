#include "Material_DX11.h"

#include "Renderer_DX11.h"
#include "RenderContext_DX11.h"
#include "RenderGpuBuffer_DX11.h"

#include "Texture_DX11.h"

namespace sge 
{
	struct MaterialStage_Helper
	{
		template<class STAGE>
		static void _bind(STAGE* matStage, RenderContext_DX11* ctx)
		{
			auto* shadStage = matStage->d3dShadStage();
			SGE_ASSERT(shadStage != nullptr);
			shadStage->bind(ctx);

			auto* dc = ctx->renderer()->d3dDeviceContext();

			for (auto& cb : matStage->constBufs()) {
				cb.uploadToGpu();

				auto* cbInfo	= cb.info();
				UINT  bindPoint	= cbInfo->bindPoint;

				auto* gpuBuffer = static_cast<RenderGpuBuffer_DX11*>(cb.gpuBuffer.ptr());
				if (!gpuBuffer) throw SGE_ERROR("const buffer is null");

				auto* d3dBuf = gpuBuffer->d3dBuf();
				if (!d3dBuf) throw SGE_ERROR("d3dbuffer is null");

				matStage->setConstBuffer(dc, bindPoint, d3dBuf);
			}

			for (auto& texParam : matStage->texParams()) {
				auto* tex = texParam.getUpdatedTexture();

				int  bindPoint = texParam.bindPoint ();
				auto fmt	   = texParam.formatType();

				switch (fmt) {
					case RenderFormatType::Texture2D: 
					{
						auto* tex2d = static_cast<Texture2D_DX11*>(tex);
						auto* rv	= tex2d->resourceView();
						auto* ss	= tex2d->samplerState();

						matStage->_dxSetShaderResource(dc, bindPoint, rv);
						matStage->_dxSetSampler(dc, bindPoint, ss);
					} break;

					default: throw SGE_ERROR("bind unsupported tedxture type: {}", fmt);
				}
			}



		}
	};

	void MaterialVertexStage_DX11::bind(RenderContext_DX11* ctx, const VertexLayout* layout) {
		MaterialStage_Helper::_bind(this, ctx);
		bindInputLayout(ctx, layout);
	}

	void MaterialVertexStage_DX11::bindInputLayout(RenderContext_DX11* ctx, const VertexLayout* layout) {
		if (!layout) throw SGE_ERROR("vertexlayout is null");

		auto* dev = ctx->renderer()->d3dDevice();
		auto* dc  = ctx->renderer()->d3dDeviceContext();

		DX11_ID3DInputLayout* dxLayout = nullptr;

		auto it = m_inputLayoutMap.find(layout);
		if (it != m_inputLayoutMap.end()) {
			dxLayout = it->second;			
		}
		else {
			using Util = DX11Util;

			Vector_<D3D11_INPUT_ELEMENT_DESC, 32> inputDesc;

			auto* vsInfo = info();

			for (auto& input : vsInfo->inputs) {
				auto* e = layout->find(input.semantic);
				if (!e) {
					throw SGE_ERROR("vertex semantic : [{}] is missing", input.semantic);
				}

				auto& dst = inputDesc.emplace_back();

				auto semanticType			= VertexSemanticUtil::getType(e->semantic);
				dst.SemanticName			= Util::getDxSemanticName(semanticType);
				dst.SemanticIndex			= VertexSemanticUtil::getIndex(e->semantic);
				dst.Format					= Util::getDxFormat(e->formatType);
				dst.InputSlot				= 0;
				dst.AlignedByteOffset		= e->offset;
				dst.InputSlotClass			= D3D11_INPUT_PER_VERTEX_DATA;
				dst.InstanceDataStepRate	= 0;
			}

			ComPtr<DX11_ID3DInputLayout> outLayout;
			auto* ss = d3dShadStage();
			auto  bc = ss->byteCode();

			auto hr = dev->CreateInputLayout(inputDesc.data()
											, Util::castUINT(inputDesc.size())
											, bc.data()
											, bc.size()
											, outLayout.ptrForInit());
			Util::throwIfError(hr);

			dxLayout = outLayout;
			m_inputLayoutMap[layout] = std::move(outLayout);

			
		}
		dc->IASetInputLayout(dxLayout);
	}


	void MaterialPixelStage_DX11::bind(RenderContext_DX11* ctx) {
		MaterialStage_Helper::_bind(this, ctx);
	}

	void MaterialPass_DX11::onBind(RenderContext* ctx, const VertexLayout* layout) {
		auto* dx11Ctx = static_cast<RenderContext_DX11*>(ctx);
		m_dx11VertexStage.bind(dx11Ctx, layout);
		 m_dx11PixelStage.bind(dx11Ctx);
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
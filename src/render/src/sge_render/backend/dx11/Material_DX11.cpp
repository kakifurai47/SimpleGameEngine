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
				if  (!gpuBuffer) throw SGE_ERROR("const buffer is null");

				auto* d3dBuf = gpuBuffer->d3dBuf();
				if  (!d3dBuf) throw SGE_ERROR("d3dbuffer is null");

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

			Vector<D3D11_INPUT_ELEMENT_DESC, 32> inputDesc;

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
		_BindStage(dx11Ctx);

	}

	void MaterialPass_DX11::_BindStage(RenderContext_DX11* ctx)
	{
		using Util = DX11Util;

		auto* renderer = ctx->renderer();

		if (!m_rasterizerState) {
			HRESULT hr;
			auto* dev = renderer->d3dDevice();
			auto& rs = info()->renderState;

			D3D11_RASTERIZER_DESC rasterDesc = {};
			rasterDesc.AntialiasedLineEnable = true;
			rasterDesc.CullMode				 = Util::getDxCullMode(rs.cullType);
			rasterDesc.DepthBias			 = 0;
			rasterDesc.DepthBiasClamp		 = 0.0f;
			rasterDesc.DepthClipEnable		 = true;

			rasterDesc.FillMode = rs.isWireFrame ? D3D11_FILL_WIREFRAME : D3D11_FILL_SOLID;
//			rasterDesc.FillMode = D3D11_FILL_WIREFRAME;
			
			rasterDesc.FrontCounterClockwise = true;
			rasterDesc.MultisampleEnable	 = false;
			rasterDesc.ScissorEnable		 = true;
			rasterDesc.SlopeScaledDepthBias  = 0.0f;

			hr = dev->CreateRasterizerState(&rasterDesc, m_rasterizerState.ptrForInit());
			Util::throwIfError(hr);
		}

		if (!m_depthStencilState) {
			HRESULT hr;
			auto* dev = renderer->d3dDevice();
			auto& rs = info()->renderState;

			D3D11_DEPTH_STENCIL_DESC depthStencilDesc = {};

			bool depthTest = rs.depthTest.isEnable();
			if  (depthTest) {
				depthStencilDesc.DepthEnable = true;
				depthStencilDesc.DepthFunc = Util::getDxDepthTestOp(rs.depthTest.op);
			}
			else {
				depthStencilDesc.DepthEnable = false;
				depthStencilDesc.DepthFunc = D3D11_COMPARISON_ALWAYS;
			}

			depthStencilDesc.DepthWriteMask = rs.depthTest.writeMask ? D3D11_DEPTH_WRITE_MASK_ALL : D3D11_DEPTH_WRITE_MASK_ZERO;

			depthStencilDesc.StencilEnable    = false;
			depthStencilDesc.StencilReadMask  = 0xFF;
			depthStencilDesc.StencilWriteMask = 0xFF;

			hr = dev->CreateDepthStencilState(&depthStencilDesc, m_depthStencilState.ptrForInit());
			Util::throwIfError(hr);

		}

		if (!m_blendState) {
			HRESULT hr;
			auto* dev = renderer->d3dDevice();
			auto& rs = info()->renderState;

			D3D11_BLEND_DESC blendStateDesc = {};
			blendStateDesc.AlphaToCoverageEnable = false;
			blendStateDesc.IndependentBlendEnable = false;
			auto& rtDesc = blendStateDesc.RenderTarget[0];

			rtDesc.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
			bool blendEnable = rs.blend.isEnable();
			if (blendEnable) {
				rtDesc.BlendEnable = true;

				if (rs.blend.rgbFunc.op == RenderState_BlendOp::Disable) {
					rtDesc.BlendOp   = D3D11_BLEND_OP_ADD;
					rtDesc.SrcBlend  = D3D11_BLEND_ONE;
					rtDesc.DestBlend = D3D11_BLEND_ZERO;
				}
				else {
					rtDesc.BlendOp	 = Util::getDxBlendOp(rs.blend.rgbFunc.op);
					rtDesc.SrcBlend	 = Util::getDxBlendFactor(rs.blend.rgbFunc.srcFactor);
					rtDesc.DestBlend = Util::getDxBlendFactor(rs.blend.rgbFunc.dstFactor);
				}

				if (rs.blend.alphaFunc.op == RenderState_BlendOp::Disable) {
					rtDesc.BlendOpAlpha		= D3D11_BLEND_OP_ADD;
					rtDesc.SrcBlendAlpha	= D3D11_BLEND_ONE;
					rtDesc.DestBlendAlpha	= D3D11_BLEND_ZERO;
				}
				else {
					rtDesc.BlendOpAlpha		= Util::getDxBlendOp(rs.blend.alphaFunc.op);
					rtDesc.SrcBlendAlpha	= Util::getDxBlendFactor(rs.blend.alphaFunc.srcFactor);
					rtDesc.DestBlendAlpha	= Util::getDxBlendFactor(rs.blend.alphaFunc.dstFactor);
				}
			}
			else {
				rtDesc.BlendEnable = false;
			}

			hr = dev->CreateBlendState(&blendStateDesc, m_blendState.ptrForInit());
			Util::throwIfError(hr);
		}

		auto* d3dCtx = renderer->d3dDeviceContext();

		d3dCtx->RSSetState(m_rasterizerState);
		d3dCtx->OMSetDepthStencilState(m_depthStencilState, 1);
		
		Color4f blendColor(1,1,1,1);
		d3dCtx->OMSetBlendState(m_blendState, blendColor.data, 0xffffffff);

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

	void Material_DX11::onResetPasses(Vector<Pass*, 1>& outPasses) {
		outPasses.clear();
		outPasses.reserve(outPasses.size());
		for (auto& p : m_dx11Passes) { outPasses.emplace_back(&p); }
	}
}
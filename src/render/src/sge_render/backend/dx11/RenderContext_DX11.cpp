
#include "RenderContext_DX11.h"
#include "Renderer_DX11.h"
#include "Shader_DX11.h"
#include "RenderGpuBuffer_DX11.h"

namespace sge
{
	RenderContext_DX11::RenderContext_DX11(CreateDesc& desc)
		: Base(desc)
	{
		m_renderer = Renderer_DX11::current();

		auto* win = static_cast<NativeUIWindow_Win32*>(desc.window);
		auto* dxgiFactory = m_renderer->dxgiFactory();
		auto* d3dDevice   = m_renderer->d3dDevice();

		DXGI_SWAP_CHAIN_DESC swapChainDesc = {};
		swapChainDesc.BufferDesc.Width						= 800;
		swapChainDesc.BufferDesc.Height						= 600;
		swapChainDesc.BufferDesc.RefreshRate.Numerator		= 60;
		swapChainDesc.BufferDesc.RefreshRate.Denominator	= 1;
		swapChainDesc.BufferDesc.Format						= DXGI_FORMAT_R8G8B8A8_UNORM;
		swapChainDesc.BufferDesc.ScanlineOrdering			= DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
		swapChainDesc.BufferDesc.Scaling					= DXGI_MODE_SCALING_UNSPECIFIED;

		swapChainDesc.SampleDesc.Count						= 1;
		swapChainDesc.SampleDesc.Quality					= 0;

		swapChainDesc.BufferUsage							= DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapChainDesc.BufferCount							= 1;
		swapChainDesc.OutputWindow							= win->m_hwmd;
		swapChainDesc.Windowed								= TRUE;
		swapChainDesc.SwapEffect							= DXGI_SWAP_EFFECT_DISCARD;
		swapChainDesc.Flags									= 0;

		auto hr = dxgiFactory->CreateSwapChain( d3dDevice, &swapChainDesc, m_dxgiSwapChain.ptrForInit());
		Util::throwIfError(hr);
	}

	void RenderContext_DX11::_createRenderTarget() {
		auto* dev = m_renderer->d3dDevice();

		HRESULT hr = {};
		ComPtr<ID3D11Texture2D> backBuffer;
		hr = m_dxgiSwapChain->GetBuffer(0, IID_PPV_ARGS(backBuffer.ptrForInit()));
		Util::throwIfError(hr);

		hr = dev->CreateRenderTargetView(backBuffer, nullptr, m_d3dRenderTargetView.ptrForInit());
		Util::throwIfError(hr);
	}

	void RenderContext_DX11::onBeginRender() {
		auto* ctx = m_renderer->d3dDeviceContext();
		if (!m_d3dRenderTargetView) {
			_createRenderTarget();
		}
		DX11_ID3DRenderTargetView* rt[] = { m_d3dRenderTargetView };
		ctx->OMSetRenderTargets(1, rt, nullptr);

		D3D11_VIEWPORT viewport{};
		viewport.TopLeftX = 0;
		viewport.TopLeftY = 0;
		viewport.Width  = 800;
		viewport.Height = 600;
		ctx->RSSetViewports(1, &viewport);
	}

	void RenderContext_DX11::onEndRender()
	{
	}


	void RenderContext_DX11::onCommand_ClearFrameBuffers(RenderCommand_ClearFrameBuffers& cmd) {
		auto* ctx = m_renderer->d3dDeviceContext();
		if (m_d3dRenderTargetView && cmd.color.has_value()) {
			ctx->ClearRenderTargetView(m_d3dRenderTargetView, cmd.color->data);
		}
	}

	void RenderContext_DX11::onCommand_SwapBuffers(RenderCommand_SwapBuffers& cmd) {
		auto hr = m_dxgiSwapChain->Present(0, 0);
		Util::throwIfError(hr);
	}

	void RenderContext_DX11::onCommand_DrawCall(RenderCommand_DrawCall& cmd) {
		SGE_ASSERT(cmd.vertexLayout != nullptr);

		auto* vertexBuffer = static_cast<RenderGpuBuffer_DX11*>(cmd.vertexBuffer.ptr());
		SGE_ASSERT(vertexBuffer != nullptr);

		SGE_ASSERT(cmd.vertexCount > 0);
		SGE_ASSERT(cmd.primitive != RenderPrimitiveType::None);

		RenderGpuBuffer_DX11* indexBuffer = nullptr;
		if (cmd.indexCount > 0) {
			indexBuffer = static_cast<RenderGpuBuffer_DX11*>(cmd.indexBuffer.ptr());
			SGE_ASSERT(indexBuffer != nullptr);
		}

		auto* ctx = m_renderer->d3dDeviceContext();

		_setTestDefaultRenderState();

		if (auto* pass = cmd.getMaterialPass()) {
			pass->bind(this, cmd.vertexLayout);
		}

		auto primitive = Util::getDxPrimitiveTopology(cmd.primitive);
		ctx->IASetPrimitiveTopology(primitive);
		
		UINT offset		 = 0;
		UINT stride		 = static_cast<UINT>(cmd.vertexLayout->stride);
		UINT vertexCount = static_cast<UINT>(cmd.vertexCount);
		UINT indexCount	 = static_cast<UINT>(cmd.indexCount);

		DX11_ID3DBuffer* ppVertexBuffers[]  = { vertexBuffer->d3dBuf() };
		ctx->IASetVertexBuffers(0, 1, ppVertexBuffers, &stride, &offset);
		
		if (indexCount > 0) {
			auto indexFormat = Util::getDxFormat(cmd.indexFormat);
			ctx->IASetIndexBuffer(indexBuffer->d3dBuf(), indexFormat, 0);
			ctx->DrawIndexed(indexCount, 0, 0);
		}
		else {
			ctx->Draw(vertexCount, 0);
		}
	}


	void RenderContext_DX11::_setTestDefaultRenderState()
	{	
		auto* dev = m_renderer->d3dDevice();
		auto* ctx = m_renderer->d3dDeviceContext();
	
		HRESULT hr;
		if (!m_testRasterizerState) {
			D3D11_RASTERIZER_DESC rasterDesc = {};
			rasterDesc.AntialiasedLineEnable = true;
			rasterDesc.CullMode				 = D3D11_CULL_BACK;
			rasterDesc.DepthBias			 = 0;
			rasterDesc.DepthBiasClamp		 = 0.0f;
			rasterDesc.DepthClipEnable		 = true;
	
			bool wireframe	 = false;
			wireframe = true; //test;
			rasterDesc.FillMode = wireframe ? D3D11_FILL_WIREFRAME : D3D11_FILL_SOLID;
	
			rasterDesc.FrontCounterClockwise = true;
			rasterDesc.MultisampleEnable	 = false;
			rasterDesc.ScissorEnable		 = false;
			rasterDesc.SlopeScaledDepthBias  = 0.0f;
	
			hr = dev->CreateRasterizerState(&rasterDesc, m_testRasterizerState.ptrForInit());
			Util::throwIfError(hr);
		}
	
		if (!m_testDepthStencilState) {
			D3D11_DEPTH_STENCIL_DESC depthStencilDesc = {};
	
			bool depthTest = true;
			if (depthTest) {
				depthStencilDesc.DepthEnable	= true;
				depthStencilDesc.DepthFunc		= D3D11_COMPARISON_LESS_EQUAL;
				depthStencilDesc.DepthWriteMask	= D3D11_DEPTH_WRITE_MASK_ALL;
			} else {
				depthStencilDesc.DepthEnable	= false;
				depthStencilDesc.DepthFunc		= D3D11_COMPARISON_ALWAYS;
				depthStencilDesc.DepthWriteMask	= D3D11_DEPTH_WRITE_MASK_ZERO;
			}
	
			depthStencilDesc.StencilEnable		= false;
			depthStencilDesc.StencilReadMask	= 0xFF;
			depthStencilDesc.StencilWriteMask	= 0xFF;
	
			hr = dev->CreateDepthStencilState(&depthStencilDesc, m_testDepthStencilState.ptrForInit());
			Util::throwIfError(hr);
		}
	
		if (!m_testBlendState) {
			D3D11_BLEND_DESC blendStateDesc = {};
			blendStateDesc.AlphaToCoverageEnable  = false;
			blendStateDesc.IndependentBlendEnable = false;
			auto& rtDesc = blendStateDesc.RenderTarget[0];
	
			rtDesc.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
			bool blendEnable = true;
			if (blendEnable) {
				rtDesc.BlendEnable	  = true;
				rtDesc.BlendOp        = D3D11_BLEND_OP_ADD;
				rtDesc.BlendOpAlpha   = D3D11_BLEND_OP_ADD;
				rtDesc.SrcBlend       = D3D11_BLEND_SRC_ALPHA;
				rtDesc.DestBlend      = D3D11_BLEND_INV_SRC_ALPHA;
				rtDesc.SrcBlendAlpha  = D3D11_BLEND_SRC_ALPHA;
				rtDesc.DestBlendAlpha = D3D11_BLEND_INV_SRC_ALPHA;
			}else{
				rtDesc.BlendEnable	  = false;
			}
	
			hr = dev->CreateBlendState(&blendStateDesc, m_testBlendState.ptrForInit());
			Util::throwIfError(hr);
		}
	
		ctx->RSSetState(m_testRasterizerState);
		ctx->OMSetDepthStencilState(m_testDepthStencilState, 1);
		
		Color4f blendColor(1,1,1,1);
		ctx->OMSetBlendState(m_testBlendState, blendColor.data, 0xffffffff);
	}


	void RenderContext_DX11::onCommit(RenderCommandBuffer& cmdBuf) {
		_dispatch(this, cmdBuf);
	}
}
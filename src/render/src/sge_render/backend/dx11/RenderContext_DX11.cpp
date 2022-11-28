
#include "RenderContext_DX11.h"

#include "Renderer_DX11.h"
#include "Shader_DX11.h"
#include "RenderGpuBuffer_DX11.h"

#include <imgui_impl_dx11.h>

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
		swapChainDesc.BufferDesc.Width						= 8;
		swapChainDesc.BufferDesc.Height						= 8;
		swapChainDesc.BufferDesc.RefreshRate.Numerator		= 60;
		swapChainDesc.BufferDesc.RefreshRate.Denominator	= 1;
		swapChainDesc.BufferDesc.Format						= DXGI_FORMAT_R8G8B8A8_UNORM;
		swapChainDesc.BufferDesc.ScanlineOrdering			= DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
		swapChainDesc.BufferDesc.Scaling					= DXGI_MODE_SCALING_UNSPECIFIED;

		swapChainDesc.SampleDesc.Count						= 1;
		swapChainDesc.SampleDesc.Quality					= 0;

		swapChainDesc.BufferUsage							= DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapChainDesc.BufferCount							= 1;
		swapChainDesc.OutputWindow							= win->m_hwnd;
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

		hr = dev->CreateRenderTargetView(backBuffer, nullptr, m_renderTargetView.ptrForInit());
		Util::throwIfError(hr);

		D3D11_TEXTURE2D_DESC backBufferDesc;
		backBuffer->GetDesc(&backBufferDesc);

		// Create depth stencil texture
		D3D11_TEXTURE2D_DESC descDepth	= {};
		descDepth.Width					= backBufferDesc.Width;
		descDepth.Height				= backBufferDesc.Height;
		descDepth.MipLevels				= 1;
		descDepth.ArraySize				= 1;
		descDepth.Format				= DXGI_FORMAT_D24_UNORM_S8_UINT;
		descDepth.SampleDesc.Count		= 1;
		descDepth.SampleDesc.Quality	= 0;
		descDepth.Usage					= D3D11_USAGE_DEFAULT;
		descDepth.BindFlags				= D3D11_BIND_DEPTH_STENCIL;
		descDepth.CPUAccessFlags		= 0;
		descDepth.MiscFlags				= 0;
		hr = dev->CreateTexture2D(&descDepth, nullptr, m_depthStencil.ptrForInit());
		Util::throwIfError(hr);

		// Create the depth stencil view
		D3D11_DEPTH_STENCIL_VIEW_DESC descDSV = {};
		descDSV.Format				= descDepth.Format;
		descDSV.ViewDimension		= D3D11_DSV_DIMENSION_TEXTURE2D;
		descDSV.Texture2D.MipSlice	= 0;
		hr = dev->CreateDepthStencilView(m_depthStencil.ptr(), &descDSV, m_depthStencilView.ptrForInit());
		Util::throwIfError(hr);
	}

	void RenderContext_DX11::onSetFrameBufferSize(const Vec2f& newSize)
	{
		m_renderTargetView.reset(nullptr);
		m_depthStencilView.reset(nullptr);

		auto hr = m_dxgiSwapChain->ResizeBuffers(0 ,
									static_cast<UINT>(Math::max(0.0f, newSize.x)),
									static_cast<UINT>(Math::max(0.0f, newSize.y)),
									DXGI_FORMAT_UNKNOWN,
									0);

		Util::throwIfError(hr);
	}

	void RenderContext_DX11::onBeginRender() {
		auto* ctx = m_renderer->d3dDeviceContext();
		if (!m_renderTargetView) {
			_createRenderTarget();
		}
		DX11_ID3DRenderTargetView* rt[] = { m_renderTargetView };
		ctx->OMSetRenderTargets(1, rt, m_depthStencilView);

		D3D11_VIEWPORT viewport{};
		viewport.TopLeftX	= 0;
		viewport.TopLeftY	= 0;
		viewport.Width		= m_frameBufferSize.x;
		viewport.Height		= m_frameBufferSize.y;
		viewport.MinDepth	= 0;
		viewport.MaxDepth	= 1;

		D3D11_RECT scissorRect = {};
		scissorRect.left	= 0;
		scissorRect.top		= 0;
		scissorRect.right	= static_cast<LONG>(m_frameBufferSize.x);
		scissorRect.bottom	= static_cast<LONG>(m_frameBufferSize.y);

		ctx->RSSetViewports(1, &viewport);
		ctx->RSSetScissorRects(1, &scissorRect);
	}

	void RenderContext_DX11::onEndRender()
	{
	}


	void RenderContext_DX11::onCommand_ClearFrameBuffers(RenderCommand_ClearFrameBuffers& cmd) 
	{
		auto* ctx = m_renderer->d3dDeviceContext();
		if (m_renderTargetView && cmd.color.has_value()) {
			ctx->ClearRenderTargetView(m_renderTargetView, cmd.color->data);
		}
		if (m_depthStencilView && cmd.depth.has_value()) {
			ctx->ClearDepthStencilView(m_depthStencilView, D3D11_CLEAR_DEPTH, *cmd.depth, 0);
		}
	}

	void RenderContext_DX11::onCommand_SwapBuffers(RenderCommand_SwapBuffers& cmd) {
		auto hr = m_dxgiSwapChain->Present(m_renderer->vsync() ? 1 : 0, 0);
		Util::throwIfError(hr);
	}

	void RenderContext_DX11::onCommand_DrawCall(RenderCommand_DrawCall& cmd) {
//		SGE_ASSERT(cmd.vertexLayout);

		auto* vertexBuffer = static_cast<RenderGpuBuffer_DX11*>(cmd.vertexBuffer.ptr());
//		SGE_ASSERT(vertexBuffer); 

//		SGE_ASSERT(cmd.vertexCount > 0);
		SGE_ASSERT(cmd.primitive != RenderPrimitiveType::None);

		RenderGpuBuffer_DX11* indexBuffer = nullptr;
		if (cmd.indexCount > 0) {
			indexBuffer = static_cast<RenderGpuBuffer_DX11*>(cmd.indexBuffer.ptr());
			SGE_ASSERT(indexBuffer);
		}

		auto* ctx = m_renderer->d3dDeviceContext();

		_setTestDefaultRenderState();

		if (auto* pass = cmd.getMaterialPass()) {
			pass->bind(this, cmd.vertexLayout);
		}

		auto primitive = Util::getDxPrimitiveTopology(cmd.primitive);
		ctx->IASetPrimitiveTopology(primitive);
		
		UINT offset		 = 0;
		UINT stride		 = static_cast<UINT>(cmd.vertexLayout ? cmd.vertexLayout->stride : 0);
		UINT vertexCount = static_cast<UINT>(cmd.vertexCount);
		UINT indexCount	 = static_cast<UINT>(cmd.indexCount);
		UINT startIndex  = static_cast<UINT>(cmd.startIndex);
		INT  baseVertex  = static_cast< INT>(cmd.baseVertex);

		DX11_ID3DBuffer* ppVertexBuffers[]  = { vertexBuffer ? vertexBuffer->d3dBuf() : nullptr };
		ctx->IASetVertexBuffers(0, 1, ppVertexBuffers, &stride, &offset);
		
		if (indexCount > 0) {
			auto indexFormat = Util::getDxFormat(cmd.indexFormat);
			ctx->IASetIndexBuffer(indexBuffer->d3dBuf(), indexFormat, 0);
			ctx->DrawIndexed(indexCount, startIndex, baseVertex);
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
//			wireframe = true; //test;
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
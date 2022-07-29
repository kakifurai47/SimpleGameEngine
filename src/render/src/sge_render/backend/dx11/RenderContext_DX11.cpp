
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

	void RenderContext_DX11::onCmd_ClearFrameBuffers(RenderCmd_ClearFrameBuffers& cmd) {
		auto* ctx = m_renderer->d3dDeviceContext();
		if (m_d3dRenderTargetView && cmd.color.has_value()) {
			ctx->ClearRenderTargetView(m_d3dRenderTargetView, cmd.color->data);
		}
	}

	void RenderContext_DX11::onCmd_SwapBuffers(RenderCmd_SwapBuffers& cmd) {
		auto hr = m_dxgiSwapChain->Present(0, 0);
		Util::throwIfError(hr);
	}

	void RenderContext_DX11::onCmd_DrawCall(RenderCmd_DrawCall& cmd) {
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

	void RenderContext_DX11::onCommit(RenderCommandBuffer& cmdBuf) {
		_dispatch(this, cmdBuf);
	}



	//void RenderContext_DX11::onDraw(RenderCmd_Draw* cmd) {
	//}
}
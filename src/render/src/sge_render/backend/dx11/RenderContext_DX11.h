#pragma once

#include "../base/RenderContext.h"
#include "Render_DX11_Common.h"

namespace sge {

	class RenderContext_DX11 : public RenderContext
	{
		using Base = RenderContext;
		using Util = DX11Util;
	public:
		RenderContext_DX11(CreateDesc& desc);

		Renderer_DX11* renderer() { return m_renderer; }

		void onCommand_ClearFrameBuffers(RenderCommand_ClearFrameBuffers& cmd);
		void onCommand_SwapBuffers		(RenderCommand_SwapBuffers&		  cmd);
		void onCommand_DrawCall			(RenderCommand_DrawCall&		  cmd);

	protected:
		Renderer_DX11* m_renderer = nullptr;

		ComPtr<DX11_IDXGISwapChain>			m_dxgiSwapChain;
		ComPtr<DX11_ID3DRenderTargetView>	m_d3dRenderTargetView;

		ComPtr<DX11_ID3DBuffer>				m_testVtxBuffer;
		ComPtr<DX11_ID3DVertexShader>		m_testVtxShader;
		ComPtr<DX11_ID3DBlob>				m_testVtxShaderByteCode;
		ComPtr<DX11_ID3DPixelShader>		m_testPxlShader;

		ComPtr<DX11_ID3DRasterizerState>	m_testRasterizerState;
		ComPtr<DX11_ID3DDepthStencilState>	m_testDepthStencilState;
		ComPtr<DX11_ID3DBlendState>			m_testBlendState;


	protected:
		void _createRenderTarget();


		virtual void onSetFrameBufferSize(const Vec2f& newSize) override;
		virtual void onBeginRender() override;
		virtual void onEndRender  () override;
		

		void _setTestDefaultRenderState();


		virtual void onCommit(RenderCommandBuffer& cmdBuf)override;
	};


}




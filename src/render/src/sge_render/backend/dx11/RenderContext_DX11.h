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
	protected:
		Renderer_DX11* m_renderer = nullptr;

		ComPtr<DX11_IDXGISwapChain>		  m_dxgiSwapChain;
		ComPtr<DX11_ID3DRenderTargetView> m_d3dRenderTargetView;

		ComPtr<DX11_ID3DBuffer>			  m_testVtxBuffer;
		ComPtr<DX11_ID3DVertexShader>	  m_testVtxShader;
		ComPtr<DX11_ID3DBlob>			  m_testVtxShaderByteCode;
		ComPtr<DX11_ID3DPixelShader>	  m_testPxlShader;

	protected:
		void _createRenderTarget();

		virtual void onSetViewport(RenderCmd_SetViewport* cmd) override;
		virtual void onClearBuffer(RenderCmd_Clear*		  cmd) override;
		virtual void onDraw		  (RenderCmd_Draw*		  cmd) override;
		virtual void onSwapBuffer () override;
		virtual void onBeginRender() override;

	};


}




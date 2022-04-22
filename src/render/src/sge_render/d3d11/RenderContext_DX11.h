#pragma once

#include "../base/RenderContext.h"
#include "sge_render_dx11.h"

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

	protected:
		void _createRenderTarget();

		virtual void onSetViewport(RenderCmd_SetViewport* cmd) override;
		virtual void onClearBuffer(RenderCmd_Clear*		  cmd) override;
		virtual void onSwapBuffer () override;
		virtual void onBeginRender() override;

	};


}




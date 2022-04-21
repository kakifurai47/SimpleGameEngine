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
		ComPtr<DX11_IDXGISwapChain> m_dxgiSwapChain;
	};


}




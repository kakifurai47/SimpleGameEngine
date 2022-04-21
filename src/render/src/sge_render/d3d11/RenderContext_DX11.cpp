
#include "RenderContext_DX11.h"
#include "Renderer_DX11.h"

namespace sge
{

	RenderContext_DX11::RenderContext_DX11(CreateDesc& desc)
		: Base(desc)
	{
		auto* win = static_cast<NativeUIWindow_Win32*>(desc.window);
		auto* renderer    = Renderer_DX11::current();
		auto* dxgiFactory = renderer->dxgiFactory();
		auto* d3dDevice   = renderer->d3dDevice();

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
		swapChainDesc.OutputWindow							= win->m_hwmd;
		swapChainDesc.Windowed								= TRUE;
		swapChainDesc.SwapEffect							= DXGI_SWAP_EFFECT_DISCARD;
		swapChainDesc.Flags									= 0;

		auto hr = dxgiFactory->CreateSwapChain( d3dDevice, &swapChainDesc, m_dxgiSwapChain.ptrForInit());
		Util::throwIfError(hr);
	}

}
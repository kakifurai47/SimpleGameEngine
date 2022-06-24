#pragma once

#ifdef SGE_RENDER_COMP_DX11

#include "../base/Renderer.h"
#include "Render_DX11_Common.h"

namespace sge
{
	class RenderContext;

	class Renderer_DX11 : public Renderer
	{
		using Base = Renderer;
		using Util = DX11Util;
	public:
		static Renderer_DX11* current() { return static_cast<Renderer_DX11*>(s_current); }

		Renderer_DX11(CreateDesc& desc);

		virtual Material*	     onCreateMaterial() override;
		virtual Shader*			 onCreateShader   (StrView shadFilename) override;
		virtual RenderContext*   onCreateContext  (RenderContext_CreateDesc& desc) override;
		virtual RenderGpuBuffer* onCreateGpuBuffer(RenderGpuBuffer_CreateDesc& desc) override;

		DX11_IDXGIDevice*		dxgiDevice()		{ return m_dxgiDevice;		 }
		DX11_IDXGIAdapter*		dxgiAdapter()		{ return m_dxgiAdapter;		 }
		DX11_IDXGIFactory*		dxgiFactory()		{ return m_dxgiFactory;		 }

		DX11_ID3DDevice*		d3dDevice()			{ return m_d3dDevice;		 }
		DX11_ID3DDeviceContext* d3dDeviceContext()	{ return m_d3dDeviceContext; }
		DX11_ID3DDebug*			d3dDebug()			{ return m_d3dDebug;		 }

	protected:
		ComPtr<DX11_IDXGIDevice>		m_dxgiDevice;
		ComPtr<DX11_IDXGIAdapter>		m_dxgiAdapter;
		ComPtr<DX11_IDXGIFactory>		m_dxgiFactory;

		ComPtr<DX11_ID3DDevice>			m_d3dDevice;
		ComPtr<DX11_ID3DDeviceContext>	m_d3dDeviceContext;
		ComPtr<DX11_ID3DDebug>			m_d3dDebug;
	};
}

#endif
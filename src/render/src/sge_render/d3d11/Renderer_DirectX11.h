#pragma once
#include "sge_render/base/Renderer_Base.h"

#include <d3d11.h>
#include <d3dcompiler.h>

#pragma comment (lib, "d3d11.lib")
#pragma comment (lib, "d3dcompiler.lib")

namespace sge
{
	class NativeUIWindow;

	class Renderer_DirectX11 : public Renderer_Base
	{
		using Base = Renderer_Base;


	protected:
		virtual void onInit(NativeUIWindow* window) override;
		virtual void onRenderUpdate()  override;
		virtual void onDestroy() override;
		virtual void onDrawTriangle() override;

	private:
		IDXGISwapChain* swapchain;             // the pointer to the swap chain interface
		ID3D11Device* dev;                     // the pointer to our Direct3D device interface
		ID3D11DeviceContext* devcon;           // the pointer to our Direct3D device context
		ID3D11RenderTargetView* backbuffer;    // the pointer to our back buffer

		ID3D11InputLayout* pLayout;            // the pointer to the input layout
		ID3D11VertexShader* pVS;               // the pointer to the vertex shader
		ID3D11PixelShader* pPS;                // the pointer to the pixel shader
		ID3D11Buffer* pVBuffer;                // the pointer to the vertex buffer

	};
}
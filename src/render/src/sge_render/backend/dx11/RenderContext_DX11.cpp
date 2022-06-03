
#include "RenderContext_DX11.h"
#include "Renderer_DX11.h"
#include "Shader_DX11.h"
#include "RenderGpuBuffer_DX11.h"

namespace sge
{
	struct VERTEX {
		FLOAT X, Y, Z;    // position
		Color4f Color;    // color
	};



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
		HRESULT hr;
		//const wchar_t* shaderFile = L"Assets/Shaders/test.hlsl";
		const wchar_t* shaderFile = L"LocalTemp/Imported/Assets/Shaders/test.shader/code.hlsl";

		auto* dev = m_renderer->d3dDevice();
		auto* ctx = m_renderer->d3dDeviceContext();
		
		auto* shad = static_cast<Shader_DX11*>(cmd.material->shader());
		if  (!shad) { SGE_ASSERT(false); return; }
		
		auto* vs = shad->d3dVtxShads()[0];
		auto* ps = shad->d3dPxlShads()[0];
		
		ctx->VSSetShader(vs, 0, 0);
		ctx->PSSetShader(ps, 0, 0);

		auto* buf = static_cast<RenderGpuBuffer_DX11*>( cmd.material->buffers()[0].ptr() );
		DX11_ID3DBuffer* ppConstBuffers[] = { buf->d3dBuf() };
		ctx->PSSetConstantBuffers1(0, 1, ppConstBuffers, nullptr, nullptr);

		//============
		
		//  TODO Vertex Buffer,
		//  IndexBuffer,
		//  Inputlayout,
		//  multiple pass,
		//  setting constant buffer at its corresponding stage

		//============

		if (!m_testVtxShader) {
			ComPtr<ID3DBlob>	byteCode;
			ComPtr<ID3DBlob>	errorMsg;
			
			hr = D3DCompileFromFile(shaderFile, 0, 0, "vs_main", "vs_5_0", 0, 0, byteCode.ptrForInit(), errorMsg.ptrForInit());
			Util::throwIfError(hr);
			
			m_testVtxShaderByteCode.reset(byteCode.ptr());
		}

		

		D3D11_INPUT_ELEMENT_DESC ied[] =
		{
			{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,  0, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
		};
		
		ComPtr<DX11_ID3DInputLayout>	outLayout;
		
		hr = dev->CreateInputLayout(ied, 2,
							   m_testVtxShaderByteCode->GetBufferPointer(),
							   m_testVtxShaderByteCode->GetBufferSize(), outLayout.ptrForInit());
		Util::throwIfError(hr);
		
		ctx->IASetInputLayout(outLayout);
		
		
		if (!m_testVtxBuffer)
		{
			// create a triangle using the VERTEX struct
			VERTEX OurVertices[] =
			{
				{0.0f, 0.5f, 0.0f, Color4f(1.0f, 0.0f, 0.0f, 1.0f)},
				{0.45f, -0.5, 0.0f, Color4f(0.0f, 1.0f, 0.0f, 1.0f)},
				{-0.45f, -0.5f, 0.0f, Color4f(0.0f, 0.0f, 1.0f, 1.0f)}
			};
			
			
			// create the vertex buffer
			D3D11_BUFFER_DESC bd	{};
		
			bd.Usage = D3D11_USAGE_DYNAMIC;                // write access access by CPU and GPU
			bd.ByteWidth = sizeof(VERTEX) * 3;             // size is the VERTEX struct * 3
			bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;       // use as a vertex buffer
			bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;    // allow CPU to write in buffer
			
		
		
			hr = dev->CreateBuffer(&bd, NULL, m_testVtxBuffer.ptrForInit());       // create the buffer
			Util::throwIfError(hr);
			
			
			// copy the vertices into the buffer
			D3D11_MAPPED_SUBRESOURCE ms;
			hr = ctx->Map(m_testVtxBuffer, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &ms);    // map the buffer
			Util::throwIfError(hr);
			memcpy(ms.pData, OurVertices, sizeof(OurVertices));                 // copy the data
			ctx->Unmap(m_testVtxBuffer, NULL);                                      // unmap the buffer			
		}
		
		
		DX11_ID3DBuffer* ppVertexBuffers[] = { m_testVtxBuffer.ptr() };
		
		// select which vertex buffer to display
		UINT stride = sizeof(VERTEX);
		UINT offset = 0;
		ctx->IASetVertexBuffers(0, 1, ppVertexBuffers, &stride, &offset);
		
		// select which primtive type we are using
		ctx->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		
		// draw the vertex buffer to the back buffer
		ctx->Draw(3, 0);
	}

	void RenderContext_DX11::onCommit(RenderCommandBuffer& cmdBuf) {
		_dispatch(this, cmdBuf);
	}



	//void RenderContext_DX11::onDraw(RenderCmd_Draw* cmd) {
	//}
}
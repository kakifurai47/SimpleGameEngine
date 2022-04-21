
#ifdef SGE_RENDER_COMP_DX11

#include "Renderer_DX11.h"
#include "RenderContext_DX11.h"

namespace sge
{

	//#ifdef SGE_RENDER_COMP_DX11
	//
	//	struct VERTEX {
	//		FLOAT X, Y, Z;
	//		float c[4];
	//	};
	//
	//	void Renderer_DirectX11::onInit(NativeUIWindow* window)
	//	{
	//		Base::onInit(window);
	//
	//		if (!window) return;
	//		auto* win32 = static_cast<NativeUIWindow_Win32*>(window);
	//		if (!win32) return;
	//		auto hwnd = win32->m_hmwd;
	//		if (!hwnd) return;
	//
	//		// fill the swap chain description struct
	//		DXGI_SWAP_CHAIN_DESC scd = {};
	//		scd.BufferCount = 1;
	//		scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;     
	//		scd.BufferUsage		  = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	//		scd.OutputWindow	  = hwnd;                           
	//		scd.SampleDesc.Count  = 4;                              
	//		scd.Windowed		  = TRUE;                           
	//
	//		// create a device, device context and swap chain using the information in the scd struct
	//		D3D11CreateDeviceAndSwapChain(NULL,
	//			D3D_DRIVER_TYPE_HARDWARE,
	//			NULL,
	//			NULL,
	//			NULL,
	//			NULL,
	//			D3D11_SDK_VERSION,
	//			&scd,
	//			&swapchain,
	//			&dev,
	//			NULL,
	//			&devcon);
	//
	//		//!how to catch error here?
	//
	//		// get the address of the back buffer
	//		ID3D11Texture2D* pBackBuffer;
	//		swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);
	//
	//		// use the back buffer address to create the render target
	//		dev->CreateRenderTargetView(pBackBuffer, NULL, &backbuffer);
	//		pBackBuffer->Release();
	//
	//		// set the render target as the back buffer
	//		devcon->OMSetRenderTargets(1, &backbuffer, NULL);
	//
	//
	//		// Set the viewport
	//		D3D11_VIEWPORT viewport = {};
	//		viewport.TopLeftX	 = 0;
	//		viewport.TopLeftY	 = 0;
	//		viewport.Width		 = 800;
	//		viewport.Height		 = 600;
	//
	//		devcon->RSSetViewports(1, &viewport);
	//	}
	//
	//	void Renderer_DirectX11::onRenderUpdate()
	//	{
	//		Base::onRenderUpdate();
	//
	//		if (!devcon || !swapchain) {
	//			SGE_LOG("throw exception here");
	//		}		
	//		const float col[4] = { 0.0f, 0.2f, 0.4f, 1.0f };
	//		// clear the back buffer to a deep blue
	//		devcon->ClearRenderTargetView(backbuffer, col);
	//
	//		// select which vertex buffer to display
	//		UINT stride = sizeof(VERTEX);
	//		UINT offset = 0;
	//		devcon->IASetVertexBuffers(0, 1, &pVBuffer, &stride, &offset);
	//
	//		// select which primtive type we are using
	//		devcon->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	//
	//		// draw the vertex buffer to the back buffer
	//		devcon->Draw(3, 0);
	//
	//		// switch the back buffer and the front buffer
	//		swapchain->Present(0, 0);
	//	}
	//
	//	void Renderer_DirectX11::onDestroy()
	//	{
	//		Base::onDestroy();
	//
	//		if (pLayout)    pLayout->Release();
	//		if (pVS)	    pVS->Release();
	//		if (pPS)	    pPS->Release();
	//		if (pVBuffer)   pVBuffer->Release();
	//		if (swapchain)  swapchain->Release();
	//		if (backbuffer) backbuffer->Release();
	//		if (dev)		dev->Release();
	//		if (devcon)		devcon->Release();
	//	}
	//
	//	void Renderer_DirectX11::onDrawTriangle()
	//	{
	//		Base::onDrawTriangle();
	//
	//		{
	//			// create a triangle using the VERTEX struct
	//			VERTEX OurVertices[] =
	//			{
	//				{ 0.0f,   0.5f, 0.0f, { 1.0f, 0.0f, 0.0f, 1.0f }},
	//				{ 0.45f, -0.5f, 0.0f, { 0.0f, 1.0f, 0.0f, 1.0f }},
	//				{-0.45f, -0.5f, 0.0f, { 0.0f, 0.0f, 1.0f, 1.0f }}
	//			};
	//
	//			// create the vertex buffer
	//			D3D11_BUFFER_DESC bd = {};
	//
	//			bd.Usage		  = D3D11_USAGE_DYNAMIC;       // write access access by CPU and GPU
	//			bd.ByteWidth	  = sizeof(VERTEX) * 3;        // size is the VERTEX struct * 3
	//			bd.BindFlags	  = D3D11_BIND_VERTEX_BUFFER;  // use as a vertex buffer
	//			bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;    // allow CPU to write in buffer
	//
	//			dev->CreateBuffer(&bd, NULL, &pVBuffer);       // create the buffer
	//
	//			// copy the vertices into the buffer
	//			D3D11_MAPPED_SUBRESOURCE ms = {};
	//			devcon->Map(pVBuffer, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &ms);    // map the buffer
	//			memcpy(ms.pData, OurVertices, sizeof(OurVertices));                 // copy the data
	//			devcon->Unmap(pVBuffer, NULL);                                      // unmap the buffer
	//		}
	//		{
	//			// load and compile the two shaders
	//			ID3DBlob* vs, * ps, * msg;
	//			D3DCompileFromFile(L"../../../../../shaders.shader", 0, 0, "VShader", "vs_4_0", 0, 0, &vs, &msg);
	//			if (msg) SGE_LOG("throw excpetion here");
	//			D3DCompileFromFile(L"../../../../../shaders.shader", 0, 0, "PShader", "ps_4_0", 0, 0, &ps, &msg);
	//			if (msg) SGE_LOG("throw excpetion here");
	//
	//			//// encapsulate both shaders into shader objects
	//			dev->CreateVertexShader(vs->GetBufferPointer(), vs->GetBufferSize(), NULL, &pVS);
	//			dev->CreatePixelShader (ps->GetBufferPointer(), ps->GetBufferSize(), NULL, &pPS);
	//
	//			// set the shader objects
	//			devcon->VSSetShader(pVS, 0, 0);
	//			devcon->PSSetShader(pPS, 0, 0);
	//
	//			// create the input layout object
	//			D3D11_INPUT_ELEMENT_DESC ied[] =
	//			{
	//				{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,  0, D3D11_INPUT_PER_VERTEX_DATA, 0},
	//				{"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
	//			};
	//			dev->CreateInputLayout(ied, 2, vs->GetBufferPointer(), vs->GetBufferSize(), &pLayout);
	//			devcon->IASetInputLayout(pLayout);
	//		}
	//	}
	//
	//#endif


	Renderer_DX11::Renderer_DX11(CreateDesc& desc)
	{
		D3D_FEATURE_LEVEL featureLevel = {};
		UINT createDeviceFlags = 0;

	#ifdef _DEBUG
		createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
	#endif

		ComPtr<ID3D11Device>			d3dDevice;
		ComPtr<ID3D11DeviceContext>		d3dDeviceContext;

		HRESULT hr = 0;
		hr = D3D11CreateDevice( nullptr,
								D3D_DRIVER_TYPE_HARDWARE,
								nullptr,
								createDeviceFlags,
								nullptr,
								0,
								D3D11_SDK_VERSION,
								d3dDevice.ptrForInit(),
								&featureLevel,
								d3dDeviceContext.ptrForInit());
		//throw error (hr);
		Util::throwIfError(hr);
		//This method returns DXGI_ERROR_SDK_COMPONENT_MISSING if you specify D3D11_CREATE_DEVICE_DEBUG in Flags
		//and the incorrect version of the debug layer is installed on your computer. Install the latest Windows SDK to get the correct version.

		hr = d3dDevice->QueryInterface(IID_PPV_ARGS(m_d3dDevice.ptrForInit()));
		Util::throwIfError(hr);

		hr = d3dDeviceContext->QueryInterface(IID_PPV_ARGS(m_d3dDeviceContext.ptrForInit()));
		Util::throwIfError(hr);

		hr = d3dDevice->QueryInterface(IID_PPV_ARGS(m_dxgiDevice.ptrForInit()));
		Util::throwIfError(hr);

		{
			ComPtr<IDXGIAdapter> dxgiAdapter;
			hr = m_dxgiDevice->GetAdapter(dxgiAdapter.ptrForInit());
			Util::throwIfError(hr);

			hr = dxgiAdapter->QueryInterface(IID_PPV_ARGS(m_dxgiAdapter.ptrForInit()));
			Util::throwIfError(hr);
		}

		hr = m_dxgiAdapter->GetParent(IID_PPV_ARGS(m_dxgiFactory.ptrForInit()));
		Util::throwIfError(hr);

		if (createDeviceFlags & D3D11_CREATE_DEVICE_DEBUG) {
			hr = m_d3dDevice->QueryInterface(IID_PPV_ARGS(m_d3dDebug.ptrForInit()));
			Util::throwIfError(hr);
		}
	}

	RenderContext* Renderer_DX11::onCreateRenderContext(RenderContext_CreateDesc& desc) {
		return new RenderContext_DX11(desc);		
	}

		

}
#endif


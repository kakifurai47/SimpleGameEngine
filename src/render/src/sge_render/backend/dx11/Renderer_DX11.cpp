
#ifdef SGE_RENDER_COMP_DX11

#include "Renderer_DX11.h"
#include "RenderContext_DX11.h"
#include "RenderGpuBuffer_DX11.h"
#include "Material_DX11.h"
#include "Texture_DX11.h"

namespace sge
{
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

	RenderContext* Renderer_DX11::onCreateContext(RenderContext_CreateDesc& desc) {
		return new RenderContext_DX11(desc);		
	}


	RenderGpuBuffer* Renderer_DX11::onCreateGpuBuffer(RenderGpuBuffer_CreateDesc& desc) 
	{
		auto* p = new RenderGpuBuffer_DX11();
		p->create(desc);
		return p;
	}

	Material* Renderer_DX11::onCreateMaterial() {
		return new Material_DX11();
	}

	Shader* Renderer_DX11::onCreateShader(StrView filename) {
		return new Shader_DX11(filename);
	}

	Texture2D* Renderer_DX11::onCreateTexture2D(Texture2D_CreateDesc& desc) {
		return new Texture2D_DX11(desc);
	}

}
#endif


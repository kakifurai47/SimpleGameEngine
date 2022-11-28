#include "Texture_DX11.h"

#include "Renderer_DX11.h"
#include "RenderContext_DX11.h"

namespace sge {

	Texture2D_DX11::Texture2D_DX11(CreateDesc& desc)
		: Base(desc)
	{
		auto* renderer = Renderer_DX11::current();
		auto* dev	   = renderer->d3dDevice();

		D3D11_TEXTURE2D_DESC t2dDesc {};

		t2dDesc.Width				= Util::castUINT(desc.size.x);
		t2dDesc.Height				= Util::castUINT(desc.size.y);
		t2dDesc.MipLevels			= Util::castUINT(desc.mipmapCount);
		t2dDesc.ArraySize			= 1;
		t2dDesc.Format				= Util::getDxColorFormat(desc.colorType);
		t2dDesc.SampleDesc.Count	= 1;
		t2dDesc.SampleDesc.Quality	= 0;
		t2dDesc.Usage				= Util::getDxUsage(desc.usage);
		t2dDesc.BindFlags			= D3D11_BIND_SHADER_RESOURCE; // D3D11_BIND_RENDER_TARGET
		t2dDesc.CPUAccessFlags		= Util::getDxCpuAccessFlags(desc.cpuAcess);
		t2dDesc.MiscFlags			= 0; // D3D11_RESOURCE_MISC_GENERATE_MIPS

		D3D11_SUBRESOURCE_DATA initData {};

		bool hasImage = desc.imageToUpload.has_value();
		if  (hasImage) {
			auto& info = desc.imageToUpload->info();

			initData.pSysMem			= desc.imageToUpload->dataPtr();
			initData.SysMemPitch		= Util::castUINT(info.strideInBytes);
			initData.SysMemSlicePitch	= 0;
		}

		auto hr = dev->CreateTexture2D(&t2dDesc, hasImage ? &initData : nullptr, m_d3dTexture2D.ptrForInit());
		Util::throwIfError(hr);

		//----
		auto& ss = desc.samplerState;

		D3D11_SAMPLER_DESC sDesc {};
		sDesc.Filter			= Util::getDxTextureFilter(ss.filter);
		sDesc.AddressU			= Util::getDxTextureWrap(ss.wrapU);
		sDesc.AddressV			= Util::getDxTextureWrap(ss.wrapV);
		sDesc.AddressW			= Util::getDxTextureWrap(ss.wrapW);
		sDesc.ComparisonFunc	= D3D11_COMPARISON_NEVER;
		sDesc.MinLOD			= ss.minLOD;
		sDesc.MaxLOD			= ss.maxLOD;

		hr = dev->CreateSamplerState(&sDesc, m_d3dSamplerState.ptrForInit());
		Util::throwIfError(hr);
		
		//----
		D3D11_SHADER_RESOURCE_VIEW_DESC rvDesc {};

		rvDesc.Format					 = t2dDesc.Format;
		rvDesc.ViewDimension			 = D3D11_SRV_DIMENSION_TEXTURE2D;
		rvDesc.Texture2D.MipLevels		 = t2dDesc.MipLevels ? t2dDesc.MipLevels : -1;
		rvDesc.Texture2D.MostDetailedMip = 0;

		hr = dev->CreateShaderResourceView(m_d3dTexture2D, &rvDesc, m_d3dResourceView.ptrForInit());
		Util::throwIfError(hr);
	}

	void Texture2D_DX11::onUploadToGpu(const Image& newImage) 
	{
		auto* renderer = Renderer_DX11::current();
		auto* ctx  = renderer->d3dDeviceContext();

		D3D11_MAPPED_SUBRESOURCE mapped {};

		auto hr = ctx->Map(m_d3dTexture2D, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped);
		Util::throwIfError(hr);

		auto data  = newImage.pixelData();
		auto stde  = newImage.info().strideInBytes;

		auto* dst  = reinterpret_cast<u8*>(mapped.pData);
		auto* src  = data.begin();

		for (size_t y = 0; y < newImage.height(); y++) {
			memcpy(dst, src, stde);
			dst += mapped.RowPitch;
			src += stde;
		}

		ctx->Unmap(m_d3dTexture2D, 0);
	}
}

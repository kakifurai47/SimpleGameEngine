#pragma once

#include "Render_DX11_Common.h"
#include "../../texture/Texture.h"

namespace sge {

	class Texture2D_DX11 : public Texture2D {
		using Base = Texture2D;
		using Util = DX11Util;
	public:
		Texture2D_DX11(CreateDesc& desc);

		DX11_ID3DShaderResourceView*	resourceView() { return m_d3dResourceView.ptr(); }
		DX11_ID3DSamplerState*			samplerState() { return m_d3dSamplerState.ptr(); }

	protected:

		virtual void onUploadToGpu(const Image& newImage) override;


	private:		
		ComPtr<DX11_ID3DTexture2D>			m_d3dTexture2D;
		ComPtr<DX11_ID3DSamplerState>		m_d3dSamplerState;
		ComPtr<DX11_ID3DShaderResourceView> m_d3dResourceView;
	};



}


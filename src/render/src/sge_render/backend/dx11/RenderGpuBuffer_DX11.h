#pragma once

#include "Render_DX11_Common.h"
#include "../../buffer/RenderGpuBuffer.h"

namespace sge {

	class RenderGpuBuffer_DX11 : public RenderGpuBuffer {
		using Base = RenderGpuBuffer;
		using Util = DX11Util;
	public :
		virtual void onCreate(CreateDesc& desc) override;

		virtual void onUploadToGpu(ByteSpan data, size_t offset) override;

		virtual u8*  onBeginMapping() override;
		virtual void   onEndMapping() override;


		
		DX11_ID3DBuffer* d3dBuf() { return m_d3dBuf; }
	private:
		ComPtr<DX11_ID3DBuffer> m_d3dBuf;
	};
}
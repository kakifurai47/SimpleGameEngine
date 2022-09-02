#include "RenderGpuBuffer_DX11.h"
#include "Renderer_DX11.h"

namespace sge {

	RenderGpuBuffer_DX11::RenderGpuBuffer_DX11(CreateDesc& desc)
		:Base(desc) 
	{
		if (desc.bufferSize <= 0) throw SGE_ERROR("buffer size == 0");
		if (desc.stride	<= 0)	  throw SGE_ERROR("stride == 0");

		D3D11_BUFFER_DESC bd = {};
		bd.ByteWidth = Util::castUINT(Math::alignTo(desc.bufferSize, 16));
		bd.StructureByteStride = Util::castUINT(desc.stride);

		switch (desc.type) {
			case Type::Vertex: {
				bd.Usage			= D3D11_USAGE_DYNAMIC;
				bd.BindFlags		= D3D11_BIND_VERTEX_BUFFER;
				bd.CPUAccessFlags	= D3D11_CPU_ACCESS_WRITE;
			}break;
			case Type::Index: {
				bd.Usage			= D3D11_USAGE_DYNAMIC;
				bd.BindFlags		= D3D11_BIND_INDEX_BUFFER;
				bd.CPUAccessFlags	= D3D11_CPU_ACCESS_WRITE;
			}break;
			case Type::Const: {			
				bd.Usage			= D3D11_USAGE_DYNAMIC;
				bd.BindFlags		= D3D11_BIND_CONSTANT_BUFFER;
				bd.CPUAccessFlags	= D3D11_CPU_ACCESS_WRITE;
			}break;
			default: throw SGE_ERROR("unsupported gpu buffer type");
		}
		auto* renderer = Renderer_DX11::current();
		auto* dev = renderer->d3dDevice();

		auto hr = dev->CreateBuffer(&bd, nullptr, m_d3dBuf.ptrForInit());
		Util::throwIfError(hr);
	}

	void RenderGpuBuffer_DX11::onUploadToGpu(ByteSpan data, size_t offset) 
	{		
		auto*  dst = onBeginMapping();
		memcpy(dst + offset, data.data(), data.size());
		onEndMapping();
	}

	u8* RenderGpuBuffer_DX11::onBeginMapping() 
	{
		auto* renderer = Renderer_DX11::current();
		auto* ctx  = renderer->d3dDeviceContext();

		D3D11_MAPPED_SUBRESOURCE mapped {};

		auto hr = ctx->Map(m_d3dBuf, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped);
		Util::throwIfError(hr);

		return reinterpret_cast<u8*>(mapped.pData);
	}

	void RenderGpuBuffer_DX11::onEndMapping()
	{
		auto* renderer = Renderer_DX11::current();
		auto* ctx  = renderer->d3dDeviceContext();

		ctx->Unmap(m_d3dBuf, 0);
	}

}

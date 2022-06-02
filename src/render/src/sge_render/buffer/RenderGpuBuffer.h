#pragma once


namespace sge {
	enum class RenderGpuBufferType : u8 {
		None, 
		Vertex,
		Index,
		Const,
	};

	struct RenderGpuBuffer_CreateDesc {
		using Type = RenderGpuBufferType;

		Type	type = Type::None;
		size_t	bufferSize	= 0;
		size_t	stride		= 0;
	};

	class RenderGpuBuffer : public NonCopyable {
	public:
		using Type		 = RenderGpuBufferType;
		using CreateDesc = RenderGpuBuffer_CreateDesc;

		RenderGpuBuffer(CreateDesc& desc);

		void uploadToGpu(ByteSpan data, size_t offset = 0) {
			if (data.size() + offset  > m_desc.bufferSize) {
				throw SGE_ERROR("out of range");
			}
			onUploadToGpu(data, offset);
		}

	protected:
		virtual void onUploadToGpu(ByteSpan data, size_t offset) = 0;

		CreateDesc m_desc;
	};

}
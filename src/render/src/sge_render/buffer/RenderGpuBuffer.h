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
		size_t	stride		= 16;
	};

	class RenderGpuBuffer : public Object {
	public:
		using Type		 = RenderGpuBufferType;
		using CreateDesc = RenderGpuBuffer_CreateDesc;

		class MappedWriter : public NonCopyable {
		public:
			~MappedWriter();

			void create(RenderGpuBuffer* buf);
			void write (ByteSpan data, size_t offset);
			void close ();

			bool isMapping() { return m_mapping; }

		private:
			u8*				 m_mapData = nullptr;
			RenderGpuBuffer* m_buffer  = nullptr;
			bool			 m_mapping = false;
		};

		CreateDesc  desc() const { return m_desc; }
		size_t		size() const { return m_desc.bufferSize; }


		void create(CreateDesc& desc);
		void uploadToGpu(ByteSpan data, size_t offset = 0);

	protected:
		void checkInRange(size_t size);

		virtual void onCreate(CreateDesc& desc) = 0;

		virtual u8*  onBeginMapping() = 0;
		virtual void   onEndMapping() = 0;

		virtual void  onUploadToGpu(ByteSpan data, size_t offset) = 0;

		CreateDesc m_desc;
	};

}
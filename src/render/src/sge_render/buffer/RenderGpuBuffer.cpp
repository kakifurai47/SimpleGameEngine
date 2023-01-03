#include "RenderGpuBuffer.h"

namespace sge {

	void RenderGpuBuffer::create(CreateDesc& desc) {
		m_desc = desc;
		onCreate(desc);
	}

	void RenderGpuBuffer::uploadToGpu(ByteSpan data, size_t offset) {
		checkInRange (data.size() + offset);
		onUploadToGpu(data, offset);
	}

	void RenderGpuBuffer::checkInRange(size_t size) {
		if (size > m_desc.bufferSize) {
			throw SGE_ERROR("render buffer : out of range");
		}
	}

	void RenderGpuBuffer::MappedWriter::create(RenderGpuBuffer* buf)
	{
		close();
		m_mapping = true;
		m_buffer  = buf;
		m_mapData = buf->onBeginMapping();
	}

	void RenderGpuBuffer::MappedWriter::write(ByteSpan data, size_t offset)
	{
		m_buffer->checkInRange(data.size() + offset);
		memcpy(m_mapData + offset, data.data(), data.size());
	}

	RenderGpuBuffer::MappedWriter::~MappedWriter() {
		close();
	}

	void RenderGpuBuffer::MappedWriter::close() {
		if (m_buffer && m_mapping) {
			m_buffer->onEndMapping();
		}
		m_mapping = false;
		m_buffer  = nullptr;
		m_mapData = nullptr;
	}

}


#include "CommandBuffer.h"

namespace sge {

	CommandBuffer::CommandBuffer()		
		: m_buffer(static_cast<u8*>(malloc(size)))
		, m_alloc(m_buffer, size)
	{
	}

	CommandBuffer::~CommandBuffer() {
		sge_delete(m_buffer);
	}

	void CommandBuffer::clear() {
		m_cmds.clear();
		m_alloc.free();
	}

}
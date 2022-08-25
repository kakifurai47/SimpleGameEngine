#include "RenderCommand.h"

namespace sge {


	RenderCommandBuffer::RenderCommandBuffer()
		: m_buffer(static_cast<u8*>(malloc(size)))
		, m_allocator(m_buffer, size)
	{
	}

	RenderCommandBuffer::~RenderCommandBuffer() {
		sge_delete(m_buffer);
	}

	void RenderCommandBuffer::reset() {
		for (auto& c : m_commands) { c->~RenderCommand(); }
		m_commands.clear();
		m_allocator.free();
	}


}
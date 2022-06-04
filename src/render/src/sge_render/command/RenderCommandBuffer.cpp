#include "RenderCommandBuffer.h"

namespace sge {

	RenderCommandBuffer::RenderCommandBuffer()		
		: m_buffer(static_cast<u8*>(malloc(size)))
		, m_alloc(m_buffer, size)
	{
	}

	RenderCommandBuffer::~RenderCommandBuffer() {
		sge_delete(m_buffer);
	}

	void RenderCommandBuffer::drawMesh(const SrcLoc& debugLoc, Material& mat) {
		auto* cmd = addCmd<RenderCmd_DrawCall>();
#if _DEBUG
		cmd->debugLoc = debugLoc;
#endif
		cmd->material = &mat;
	}

	void RenderCommandBuffer::reset() {
		for (auto& c : m_cmds) { c->~RenderCmd(); }
		m_cmds.clear();
		m_alloc.free();
	}
}
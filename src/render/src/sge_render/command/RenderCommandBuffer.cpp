#include "RenderCommandBuffer.h"
#include "../mesh/RenderMesh.h"

namespace sge {

	RenderCommandBuffer::RenderCommandBuffer()		
		: m_buffer(static_cast<u8*>(malloc(size)))
		, m_alloc(m_buffer, size)
	{
	}

	RenderCommandBuffer::~RenderCommandBuffer() {
		sge_delete(m_buffer);
	}


	void RenderCommandBuffer::drawSubMesh(const SrcLoc& debugLoc, RenderSubMesh& mesh, Material& mat) {
		auto* cmd = addCmd<RenderCmd_DrawCall>();
#if _DEBUG
		cmd->debugLoc = debugLoc;
#endif
		cmd->vertexLayout	= mesh.vertexLayout();
		cmd->indexFormat	= mesh.indexFormat ();

		cmd->indexCount		= mesh.indexCount ();
		cmd->vertexCount	= mesh.vertexCount();

		cmd->indexBuffer	= mesh.indexBuffer ();
		cmd->vertexBuffer	= mesh.vertexBuffer();
		cmd->material = &mat;
	}

	void RenderCommandBuffer::drawMesh(const SrcLoc& debugLoc, RenderMesh& mesh, Material& mat) {
		auto submeshes = mesh.subMeshes();
		for (auto& m : submeshes) {
			drawSubMesh(debugLoc, m, mat);
		}
	}

	void RenderCommandBuffer::reset() {
		for (auto& c : m_cmds) { c->~RenderCmd(); }
		m_cmds.clear();
		m_alloc.free();
	}
}
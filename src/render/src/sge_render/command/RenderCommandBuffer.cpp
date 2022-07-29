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


	void RenderCommandBuffer::drawSubMesh(const SrcLoc& debugLoc, RenderSubMesh& subMesh, Material* mat) {
		SGE_ASSERT(mat != nullptr);

		auto passes = mat->passes();

		for (size_t i = 0; i < passes.size(); i++) {
			auto* cmd = addCmd<RenderCmd_DrawCall>();
	#if _DEBUG
			cmd->debugLoc = debugLoc;
	#endif
			cmd->primitive			= subMesh.primitive();

			cmd->vertexLayout		= subMesh.vertexLayout();
			cmd->indexFormat		= subMesh.indexFormat ();
	
			cmd->indexCount			= subMesh.indexCount ();
			cmd->vertexCount		= subMesh.vertexCount();
	
			cmd->indexBuffer		= subMesh.indexBuffer ();
			cmd->vertexBuffer		= subMesh.vertexBuffer();

			cmd->material			= mat;
			cmd->materialPassIndex	= i;
		}
	}

	void RenderCommandBuffer::drawMesh(const SrcLoc& debugLoc, RenderMesh& mesh, Material* mat) {
		auto submeshes = mesh.subMeshes();
		for (auto& s : submeshes) {
			drawSubMesh(debugLoc, s, mat);
		}
	}

	void RenderCommandBuffer::reset() {
		for (auto& c : m_cmds) { c->~RenderCmd(); }
		m_cmds.clear();
		m_alloc.free();
	}
}
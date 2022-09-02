#include "RenderRequest.h"

namespace sge
{

	void RenderRequest::reset() { commandBuffer.reset(); }

	void RenderRequest::setMaterialCommonParams(Material* mtl)
	{
		if (!mtl) return;

		mtl->setParam("SGE_MVP", proj * view * model);
	}

	void RenderRequest::drawMesh(const SrcLoc& debugLoc, RenderMesh& mesh, Material* mtl)
	{
		setMaterialCommonParams(mtl);

		auto submeshes = mesh.subMeshes();
		for (auto& s : submeshes) {
			drawSubMesh(debugLoc, s, mtl);
		}
	}

	void RenderRequest::drawSubMesh(const SrcLoc& debugLoc, RenderSubMesh& subMesh, Material* mtl)
	{
		SGE_ASSERT(mtl != nullptr);

		auto passes = mtl->passes();

		for (size_t i = 0; i < passes.size(); i++) {
			auto* cmd = commandBuffer.newCommand<RenderCommand_DrawCall>();
	#if _DEBUG
			cmd->debugLoc = debugLoc;
	#endif
			cmd->primitive			= subMesh.primitive	  ();

			cmd->vertexLayout		= subMesh.vertexLayout();
			cmd->indexFormat		= subMesh.indexFormat ();
	
			cmd->indexCount			= subMesh.indexCount  ();
			cmd->vertexCount		= subMesh.vertexCount ();

			cmd->startIndex			= subMesh.startIndex  ();
			cmd->baseVertex			= subMesh.baseVertex  ();
	
			cmd->indexBuffer		= subMesh.indexBuffer ();
			cmd->vertexBuffer		= subMesh.vertexBuffer();

			cmd->material			= mtl;
			cmd->materialPassIndex	= i;
		}
	}

}



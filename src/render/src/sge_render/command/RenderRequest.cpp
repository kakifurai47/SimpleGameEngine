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
		if (!mtl) { SGE_ASSERT(false); return; }

		auto passes = mtl->passes();

		for (size_t i = 0; i < passes.size(); i++) {
			auto* cmd = addDrawCall();
	#if _DEBUG
			cmd->debugLoc			= debugLoc;
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

	void RenderRequest::drawFullScreenTriangle(const SrcLoc& debugLoc, Material* mtl)
	{	
		if (!mtl) { SGE_ASSERT(false); return; }

		setMaterialCommonParams(mtl);
		auto passes  = mtl->passes();

		for (size_t i = 0; i < passes.size(); i++)
		{
			auto* cmd = addDrawCall();
	#if _DEBUG
			cmd->debugLoc			= debugLoc;
	#endif
			cmd->primitive			= RenderPrimitiveType::Triangles;
			cmd->vertexLayout		= nullptr;
			cmd->indexFormat		= RenderFormatType::None;
			cmd->vertexCount		= 3;
			cmd->indexCount			= 0;
	
			cmd->material			= mtl;
			cmd->materialPassIndex	= i;	
		}
	}

}



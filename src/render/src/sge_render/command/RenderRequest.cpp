#include "RenderRequest.h"
#include <sge_render/backend/base/Renderer.h>

namespace sge
{

	void RenderRequest::reset(RenderContext* ctx)
	{
		m_renderContext = ctx;

		commandBuffer.reset();

		if (lineMaterial)
		{
			//lineMaterial->setParam("SGE_MVP", proj * view);
			lineMaterial->setParam("SGE_MVP", Mat4f::s_identity());
		}


		m_inlineDraw .reset();
	}

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

	void RenderRequest::drawLines(Span<VertexLib::PosCol> points, Span<u16> indicies)
	{
		if (indicies.size() <= 0) return;

		auto* cmd = addDrawCall();
		cmd->baseVertex = m_inlineDraw.vertexData.size();
		cmd->startIndex = m_inlineDraw.indexData.size();

		m_inlineDraw.vertexData.appendRange(spanCast<u8>(points  ));
		 m_inlineDraw.indexData.appendRange(spanCast<u8>(indicies));

		cmd->material		= lineMaterial;
		cmd->primitive		= RenderPrimitiveType::Lines;
		cmd->vertexLayout	= decltype(points)::element_type::s_layout();
		cmd->vertexBuffer	= m_inlineDraw.vertexBuffer;
		cmd->vertexCount	= 0;
		cmd->indexBuffer	= m_inlineDraw.indexBuffer;
		cmd->indexFormat	= RenderFormatType::UInt16x1;
		cmd->indexCount		= indicies.size();
	}

	void RenderRequest::drawWiredCube(const Vec3f& center, const Vec3f& size, const Color4b& color) {

		Vec3f pt[] =
		{
			{ -0.5f, -0.5f, -0.5f },
			{  0.5f, -0.5f, -0.5f },
			{  0.5f,  0.5f, -0.5f },
			{ -0.5f,  0.5f, -0.5f },

			{ -0.5f, -0.5f,  0.5f },
			{  0.5f, -0.5f,  0.5f },
			{  0.5f,  0.5f,  0.5f },
			{ -0.5f,  0.5f,  0.5f },
		};

		VertexLib::PosCol verticies[8];

		for (size_t i = 0; i < 8; i++)
		{
			auto& v  = verticies[i];
			v.position<0>() = pt[i];
			v.color	  <0>()	= color;
		}

		u16 indices[] = {	0,1,
							1,2,
							2,3,
							3,0,
							
							4,5,
							5,6,
							6,7,
							7,4,
							
							0,4,
							1,5,
							2,6,
							3,7 };

		drawLines(verticies, indices);
	}


	void RenderRequest::commit() {
		if (!m_renderContext) return;

		m_inlineDraw.uploadToGpu();
		m_renderContext->commit(commandBuffer);
	}

	void RenderRequest::InlineDraw::reset()
	{
		vertexData.clear();
		indexData.clear();

		if (!vertexBuffer)
		{
			auto* renderer = Renderer::current();

			RenderGpuBuffer::CreateDesc desc;
			desc.type		= RenderGpuBufferType::Vertex;
			desc.bufferSize = 16;

			vertexBuffer = renderer->createGpuBuffer(desc);
		}

		if (!indexBuffer)
		{
			auto* renderer = Renderer::current();
		
			RenderGpuBuffer::CreateDesc desc;
			desc.type		= RenderGpuBufferType::Index;
			desc.bufferSize = 16;
		
			indexBuffer = renderer->createGpuBuffer(desc);
		}
	}

	void RenderRequest::InlineDraw::uploadToGpu() {
		_uploadToGpu(vertexBuffer, vertexData);
		_uploadToGpu(indexBuffer,  indexData );
	}

	void RenderRequest::InlineDraw::_uploadToGpu(SPtr<RenderGpuBuffer>& buf, const Vector<u8>& data)
	{
		auto n = data.size();
		
		if (n <= 0) return;
		if (buf->size() < n)
		{	
			auto desc = buf->desc();
			desc.bufferSize = Math::nextPow2(n);
			buf->create(desc);
		}

		buf->uploadToGpu(data);
	}

}



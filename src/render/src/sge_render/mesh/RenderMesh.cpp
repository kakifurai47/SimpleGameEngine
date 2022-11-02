#include "RenderMesh.h"
#include "EditMesh.h"
#include "MeshBuilder.h"


#include <sge_render/vertex/VertexLayoutManager.h>
#include <sge_render/backend/base/Renderer.h>

namespace sge {

	struct RenderMesh_InternalHelper {

		template<class T>
		static u8 getAttrCount(const Vector<T>& t, size_t vtxCount) {
			size_t  cnt = t.size();
			if (cnt <= 0) return 0;
			if (cnt < vtxCount) { SGE_ASSERT(false); return 0; }
			return 1;
		}

		template<class T, size_t N>
		static u8 getAttrCount(const Vector<T>(&t)[N], size_t vtxCount) {
			u8 ret = 0;
			for (u8 i = 0; i < N; i++) {
				ret += getAttrCount(t[i], vtxCount);
			}
			return ret;
		}
	};

	void RenderMesh::create(const EditMesh& src) {
		using Helper = RenderMesh_InternalHelper;
		using ST = VertexSemanticType;
		using UT = RenderFormatTypeUtil;

		clear();
		
		size_t vertexCount = src.pos.size();

		u8 colorCount		= Helper::getAttrCount(src.color,		vertexCount);
		u8 uvCount			= Helper::getAttrCount(src.uv,			vertexCount);
		u8 normalCount		= Helper::getAttrCount(src.normal,		vertexCount);
		u8 tangentCount		= Helper::getAttrCount(src.tangent,		vertexCount);
		u8 binormalCount	= Helper::getAttrCount(src.binormal,	vertexCount);

		auto type = VertexType::make(ST::POSITION,	UT::get<Tuple3f>(), 1,
									 ST::COLOR,		UT::get<Color4b>(), colorCount,
									 ST::TEXCOORD,	UT::get<Tuple2f>(), uvCount,
									 ST::NORMAL,	UT::get<Tuple3f>(), normalCount,
									 ST::TANGENT,	UT::get<Tuple3f>(), tangentCount,
									 ST::BINORMAL,	UT::get<Tuple3f>(), binormalCount);

		m_vertexLayout = VertexLayoutManager::instance()->getLayout(type);
		if (!m_vertexLayout) {
			throw SGE_ERROR("failed finding layout for this mesh \n"
							" colCount:	[{}],\n UvCount:	[{}],\n normCount:	[{}],\n"
							" tanCount:	[{}],\n bNormCount:	[{}] \n",
							colorCount, uvCount, normalCount, tangentCount, binormalCount);
		}

		for (auto p : src.pos)
		{
			if (p.x > m_AABB.max.x) m_AABB.max.x = p.x;
			if (p.y > m_AABB.max.y) m_AABB.max.y = p.y;
			if (p.z > m_AABB.max.z) m_AABB.max.z = p.z;

			if (p.x < m_AABB.min.x) m_AABB.min.x = p.x;
			if (p.y < m_AABB.min.y) m_AABB.min.y = p.y;
			if (p.z < m_AABB.min.z) m_AABB.min.z = p.z;
		}

		setSubMeshCount(1);
		m_subMeshes[0].create(src);
	}

	void RenderMesh::clear() {
		m_primitive		= RenderPrimitiveType::Triangles;
		m_vertexLayout	= nullptr;
		m_subMeshes.clear();
	}

	void RenderMesh::resetVertexBuffer(size_t newVertexCount) {
		m_vertexCount  = newVertexCount;
		if (m_vertexCount <= 0) {
			m_vertexBuffer = nullptr;
			return;
		}
		auto* renderer = Renderer::current();
		RenderGpuBuffer::CreateDesc desc;
		desc.bufferSize = m_vertexCount * _vertexStride();
		desc.type		= RenderGpuBufferType::Vertex;
		m_vertexBuffer = renderer->createGpuBuffer(desc);
	}

	void RenderMesh::resetIndexBuffer(size_t newIndexCount) {
		m_indexCount   = newIndexCount;
		if (m_indexCount <= 0) {
			m_indexBuffer = nullptr;
			return;
		}
		auto* renderer = Renderer::current();
		RenderGpuBuffer::CreateDesc desc;
		desc.bufferSize = m_indexCount * _indexStride();
		desc.type	    = RenderGpuBufferType::Index;
		m_indexBuffer = renderer->createGpuBuffer(desc);
	}

	void RenderMesh::setSubMeshCount(size_t newSize) {
		size_t oldSize = m_subMeshes.size();
		m_subMeshes.resize(newSize);

		for (size_t i = oldSize; i < newSize; i++) {
			auto& sm = m_subMeshes[i];
			sm.m_mesh = this;
		}
	}

	void RenderMesh::_calculateAABB()
	{

		




	}

	void RenderSubMesh::create(const EditMesh& src) {
		using Helper = RenderMesh_InternalHelper;
		clear();

		auto* layout = m_mesh->m_vertexLayout;

		MeshBuilder::CreateDesc buildDesc{};
		buildDesc.layout	  = layout;
		buildDesc.vertexCount = src.pos.size();
		buildDesc.indexCount  = src.indicies.size();

		MeshBuilder builder{buildDesc};

		if (buildDesc.indexCount > 0) {
			builder.index().copy(src.indicies);
		}

		for (auto& e : layout->elements) {
			using Util		= VertexSemanticUtil;
			using SmtType	= VertexSemanticType;

			auto t = Util::getType (e.semantic);
			auto i = Util::getIndex(e.semantic);

			switch (t){
				case SmtType::POSITION: builder.pos		(i).copy(src.pos	 );	break;
				case SmtType::COLOR:	builder.color	(i).copy(src.color	 );	break;
				case SmtType::TEXCOORD: builder.uv		(i).copy(src.uv[i]	 );	break;
				case SmtType::NORMAL:	builder.normal	(i).copy(src.normal	 );	break;
				case SmtType::TANGENT:	builder.tangent	(i).copy(src.tangent );	break;
				case SmtType::BINORMAL:	builder.binormal(i).copy(src.binormal);	break;
				default: SGE_ASSERT(false);
			}
		}

		builder.createIndexResult (m_mesh->m_indexFormat,   m_indexCount,  m_mesh->m_indexBuffer);
		builder.createVertexResult(layout,				   m_vertexCount, m_mesh->m_vertexBuffer);
	}

	void RenderSubMesh::clear() 
	{
		m_vertexCount = 0;
		m_indexCount  = 0;

		m_startIndex  = 0;
		m_baseVertex  = 0;
	}
}
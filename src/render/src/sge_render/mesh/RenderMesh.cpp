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

		setSubMeshCount(1);
		m_subMeshes[0].create(src);
	}

	void RenderMesh::clear() {
		m_primitive		= RenderPrimitiveType::Triangles;
		m_vertexLayout	= nullptr;
		m_subMeshes.clear();
	}

	void RenderMesh::setSubMeshCount(size_t newSize) {
		size_t oldSize = m_subMeshes.size();
		m_subMeshes.resize(newSize);

		for (size_t i = oldSize; i < newSize; i++) {
			auto& sm = m_subMeshes[i];

			sm.m_vertexLayout = m_vertexLayout;
			sm.m_primitive	  = m_primitive;
		}
	}

	void RenderSubMesh::create(const EditMesh& src) {
		using Helper = RenderMesh_InternalHelper;
		clear();	

		MeshBuilder::CreateDesc buildDesc{};
		buildDesc.layout	  = m_vertexLayout;
		buildDesc.vertexCount = src.pos.size();
		buildDesc.indexCount  = src.indicies.size();

		MeshBuilder builder{buildDesc};

		if (buildDesc.indexCount > 0) {
			builder.idx().copy(src.indicies);
		}

		for (auto& e : m_vertexLayout->elements) {
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

		builder.createIndexResult (m_indexFormat,   m_indexCount,  m_indexBuffer);
		builder.createVertexResult(m_vertexLayout, m_vertexCount, m_vertexBuffer);
	}

	void RenderSubMesh::clear() {
		m_vertexBuffer = nullptr;
		m_indexBuffer  = nullptr;

		m_vertexCount = 0;
		m_indexCount  = 0;
	}
}
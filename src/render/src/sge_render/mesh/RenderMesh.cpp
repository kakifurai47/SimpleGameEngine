#include "RenderMesh.h"
#include "EditMesh.h"

#include <memory>

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

		template<class T>
		static void copyVertexData(const Vector<T>& src, size_t index, u8* dst, size_t stride, size_t offset, size_t count) {
			SGE_ASSERT (index == 0);

			u8* p = dst + offset;

			for (size_t i = 0; i < count; i++) {
				*reinterpret_cast<T*>(p) = src[i];
				p += stride;
			}
		}

		template<class T, size_t N>
		static void copyVertexData(const Vector<T>(&src)[N], size_t index, u8* dst, size_t stride, size_t offset, size_t count) {
			return  copyVertexData(src[index], 0, dst, stride, offset, count);
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
							colorCount, uvCount, normalCount,
							tangentCount, binormalCount
			);
		}
		setSubMeshCount(1);
		m_subMeshes[0].create(src);
	}

	void RenderMesh::clear() {
		m_vertexLayout = nullptr;
		m_subMeshes.clear();
	}

	void RenderMesh::setSubMeshCount(size_t newSize) {
		size_t oldSize = m_subMeshes.size();
		m_subMeshes.resize(newSize);
		for (size_t i = oldSize; i < newSize; i++) {
			m_subMeshes[i].m_mesh = this;
		}
	}

	void RenderSubMesh::create(const EditMesh& src) {
		using Helper = RenderMesh_InternalHelper;
		clear();

		m_indexCount  = src.indicies.size();
		m_vertexCount = src.pos.size();

		if (m_vertexCount <= 0) {
			return;
		}
		auto* vertexLayout = m_mesh->m_vertexLayout;
		
		Vector_<u8, 1024> vertexData;
		vertexData.resize(vertexLayout->stride * m_vertexCount);

		size_t vtxSize	= vertexLayout->stride;
		size_t vtxCnt	= m_vertexCount;

		u8* pData		= vertexData.data();

		for (auto& e : vertexLayout->elements) {
			using Util		= VertexSemanticUtil;
			using SmtType	= VertexSemanticType;

			auto smtType  = Util::getType (e.semantic);
			auto smtIdx   = Util::getIndex(e.semantic);

			switch (smtType) {
				case SmtType::POSITION: Helper::copyVertexData(src.pos,		 smtIdx, pData, vtxSize, e.offset, vtxCnt); break;
				case SmtType::COLOR:	Helper::copyVertexData(src.color,	 smtIdx, pData, vtxSize, e.offset, vtxCnt); break;
				case SmtType::TEXCOORD: Helper::copyVertexData(src.uv, 		 smtIdx, pData, vtxSize, e.offset, vtxCnt); break;
				case SmtType::NORMAL:	Helper::copyVertexData(src.normal, 	 smtIdx, pData, vtxSize, e.offset, vtxCnt); break;
				case SmtType::TANGENT:	Helper::copyVertexData(src.tangent,	 smtIdx, pData, vtxSize, e.offset, vtxCnt); break;
				case SmtType::BINORMAL:	Helper::copyVertexData(src.binormal, smtIdx, pData, vtxSize, e.offset, vtxCnt); break;
				default: SGE_ASSERT(false);
			}
		}

		auto* renderer = Renderer::current();

		{
			RenderGpuBuffer::CreateDesc desc;
			desc.type		= RenderGpuBufferType::Vertex;
			desc.bufferSize = vertexData.size();
			m_vertexBuf = renderer->createGpuBuffer(desc);
			m_vertexBuf->uploadToGpu(vertexData);
		}

		if (m_indexCount > 0) {
			ByteSpan indexData;
			Vector_<u16, 1024> indices_sint16;

			if (m_indexCount >= UINT16_MAX) {
				m_indexFormat	 = RenderFormatType::UInt32x1;
				indexData		 = spanCast<const u8, const u32>(src.indicies);
			}
			else {
				indices_sint16.reserve(m_indexCount);
				for (auto i : src.indicies) {
					indices_sint16.emplace_back(static_cast<u16>(i));
				}
				m_indexFormat	= RenderFormatType::UInt16x1;
				indexData		= spanCast<const u8, const u16>(indices_sint16);
			}

			RenderGpuBuffer::CreateDesc desc;
			desc.type		= RenderGpuBufferType::Index;
			desc.bufferSize = indexData.size();

			m_indexBuf = renderer->createGpuBuffer(desc);
			m_indexBuf->uploadToGpu(indexData);
		}
	}

	void RenderSubMesh::clear() {
		m_vertexBuf = nullptr;
		m_indexBuf	= nullptr;

		m_vertexCount = 0;
		m_indexBuf	  = 0;
	}

}
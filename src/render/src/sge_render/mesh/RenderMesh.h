#pragma once

#include <sge_render/backend/base/Render_Common.h>

#include <sge_render/buffer/RenderGpuBuffer.h>
#include <sge_render/vertex/Vertex.h>
#include <sge_core/math/AABB3.h>



namespace sge 
{
	class EditMesh;
	class RenderMesh;

	class RenderSubMesh {
	public:
		void create(const EditMesh& src);
		void clear ();

		SGE_INLINE RenderPrimitiveType primitive   () const;

		SGE_INLINE const VertexLayout* vertexLayout() const;
		SGE_INLINE RenderFormatType    indexFormat () const;

		SGE_INLINE size_t indexCount () { return  m_indexCount; }
		SGE_INLINE size_t vertexCount() { return m_vertexCount; }

		SGE_INLINE size_t startIndex() { return m_startIndex; }
		SGE_INLINE size_t baseVertex() { return m_baseVertex; }


		SGE_INLINE RenderGpuBuffer* indexBuffer ();
		SGE_INLINE RenderGpuBuffer* vertexBuffer();

		SGE_INLINE void  setIndexCount(size_t v) {  m_indexCount = v; }
		SGE_INLINE void setVertexCount(size_t v) { m_vertexCount = v; }

		SGE_INLINE void setStartIndex(size_t v) { m_startIndex = v; }
		SGE_INLINE void setBaseVertex(size_t v) { m_baseVertex = v; }

	friend class RenderMesh;
	protected:
		RenderMesh*	m_mesh = nullptr;

		size_t  m_indexCount = 0;
		size_t m_vertexCount = 0;

		size_t m_startIndex = 0;
		size_t m_baseVertex = 0;
	};

	class RenderMesh : public RefCountBase {
	public:
		using MappedWriter = RenderGpuBuffer::MappedWriter;
		using SubMesh	   = RenderSubMesh;

		Span<SubMesh> subMeshes() { return m_subMeshes; }

		void create(const EditMesh& editMesh);
		void clear();

		SGE_INLINE RenderGpuBuffer* vertexBuffer() { return m_vertexBuffer; }
		SGE_INLINE RenderGpuBuffer*  indexBuffer() { return  m_indexBuffer; }
		
		SGE_INLINE size_t vertexCount() const { return m_vertexCount; }
		SGE_INLINE size_t  indexCount() const { return  m_indexCount; }

		SGE_INLINE void setVertexLayout(const VertexLayout* layout_) { m_vertexLayout = layout_; }
		SGE_INLINE void setIndexFormat (RenderFormatType    format_) { m_indexFormat  = format_; }

		void resetVertexBuffer(size_t newVertexCount);
		void resetIndexBuffer (size_t  newIndexCount);

		void setSubMeshCount(size_t newSize);

		const AABB3f& AABB() const { return m_AABB; }

	private:

		SGE_INLINE size_t _vertexStride() const { return m_vertexLayout->stride; }
		SGE_INLINE size_t  _indexStride() const { return m_indexFormat == RenderFormatType::UInt32x1 ? 4 : 2; }

		void _calculateAABB();

	friend class RenderSubMesh;
	protected:
		RenderPrimitiveType	m_primitive		= RenderPrimitiveType::Triangles;

		const VertexLayout*	m_vertexLayout  = nullptr;
		RenderFormatType	m_indexFormat	= RenderFormatType::None;

		SPtr<RenderGpuBuffer>  m_indexBuffer;
		SPtr<RenderGpuBuffer> m_vertexBuffer;

		Vector<SubMesh, 1> m_subMeshes;

		size_t m_vertexCount = 0;
		size_t m_indexCount  = 0;

		AABB3f m_AABB;
	};

	SGE_INLINE RenderPrimitiveType RenderSubMesh::primitive	  () const { return m_mesh->m_primitive;	}

	SGE_INLINE RenderFormatType    RenderSubMesh::indexFormat () const { return m_mesh->m_indexFormat;  }
	SGE_INLINE const VertexLayout* RenderSubMesh::vertexLayout() const { return m_mesh->m_vertexLayout; }

	SGE_INLINE RenderGpuBuffer*	   RenderSubMesh::indexBuffer () { return m_mesh->m_indexBuffer;  }
	SGE_INLINE RenderGpuBuffer*	   RenderSubMesh::vertexBuffer() { return m_mesh->m_vertexBuffer; }


}
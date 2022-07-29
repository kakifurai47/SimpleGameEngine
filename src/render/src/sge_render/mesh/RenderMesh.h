#pragma once

#include <sge_render/backend/base/Render_Common.h>

#include <sge_render/buffer/RenderGpuBuffer.h>
#include <sge_render/vertex/Vertex.h>


namespace sge {
	class EditMesh;
	class RenderMesh;

	class RenderSubMesh {
	public:
		RenderFormatType indexFormat() { return m_indexFormat; }

		SGE_INLINE size_t indexCount () { return m_indexCount;  }
		SGE_INLINE size_t vertexCount() { return m_vertexCount; }

		SGE_INLINE const VertexLayout* vertexLayout() const;
		SGE_INLINE RenderPrimitiveType primitive() const;

		SGE_INLINE RenderGpuBuffer* indexBuffer () { return m_indexBuf;  }
		SGE_INLINE RenderGpuBuffer* vertexBuffer() { return m_vertexBuf; }

		void create(const EditMesh& src);
		void clear();

	friend class RenderMesh;
	protected:
		RenderMesh* m_mesh = nullptr;
		RenderFormatType m_indexFormat = RenderFormatType::None;

		SPtr<RenderGpuBuffer> m_indexBuf;
		SPtr<RenderGpuBuffer> m_vertexBuf;

		size_t m_indexCount		= 0;
		size_t m_vertexCount	= 0;
	};


	class RenderMesh : public NonCopyable {
	public:
		using SubMesh = RenderSubMesh;

		Span<SubMesh> subMeshes() { return m_subMeshes; }

		void create(const EditMesh& editMesh);
		void clear();

		void setSubMeshCount (size_t newSize);

	friend class SubMesh;
	protected:

		RenderPrimitiveType m_primitive = RenderPrimitiveType::Triangles;
		const VertexLayout*	m_vertexLayout = nullptr;
		Vector_<SubMesh, 1> m_subMeshes;
	};

	SGE_INLINE const VertexLayout* RenderSubMesh::vertexLayout() const { return m_mesh->m_vertexLayout; }
	SGE_INLINE RenderPrimitiveType RenderSubMesh::primitive	  () const { return m_mesh->m_primitive;    }
}
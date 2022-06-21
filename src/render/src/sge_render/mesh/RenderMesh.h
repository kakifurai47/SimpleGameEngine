#pragma once

#include <sge_render/buffer/RenderGpuBuffer.h>
#include <sge_render/vertex/Vertex.h>


namespace sge {
	class EditMesh;
	class RenderMesh;

	class RenderSubMesh {
	public:
		RenderFormatType indexFormat() { return m_indexFormat; }

		size_t indexCount () { return m_indexCount;  }
		size_t vertexCount() { return m_vertexCount; }

		const VertexLayout* vertexLayout();

		RenderGpuBuffer* indexBuffer () { return m_indexBuf;  }
		RenderGpuBuffer* vertexBuffer() { return m_vertexBuf; }

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
		const VertexLayout*	m_vertexLayout = nullptr;

		Vector_<SubMesh, 1> m_subMeshes;
	};

	SGE_INLINE const VertexLayout* RenderSubMesh::vertexLayout() {
		return m_mesh->m_vertexLayout;
	}
}
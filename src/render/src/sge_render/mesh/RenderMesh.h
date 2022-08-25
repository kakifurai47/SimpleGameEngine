#pragma once

#include <sge_render/backend/base/Render_Common.h>

#include <sge_render/buffer/RenderGpuBuffer.h>
#include <sge_render/vertex/Vertex.h>

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
		SGE_INLINE RenderFormatType    indexFormat () const { return m_indexFormat; }

		SGE_INLINE size_t indexCount () { return  m_indexCount; }
		SGE_INLINE size_t vertexCount() { return m_vertexCount; }

		SGE_INLINE RenderGpuBuffer* indexBuffer () { return  m_indexBuffer; }
		SGE_INLINE RenderGpuBuffer* vertexBuffer() { return m_vertexBuffer; }

	friend class RenderMesh;
	protected:
		RenderMesh*		 m_mesh		   = nullptr;

		RenderFormatType m_indexFormat = RenderFormatType::None;

		SPtr<RenderGpuBuffer>  m_indexBuffer;
		SPtr<RenderGpuBuffer> m_vertexBuffer;

		size_t  m_indexCount	= 0;
		size_t m_vertexCount	= 0;
	};

	class RenderMesh : public NonCopyable {
	public:
		using SubMesh = RenderSubMesh;

		Span<SubMesh> subMeshes() { return m_subMeshes; }

		void create(const EditMesh& editMesh);
		void clear();

		void setSubMeshCount(size_t newSize);

	friend class RenderSubMesh;
	protected:
		Vector<SubMesh, 1>	m_subMeshes;

		const VertexLayout* m_vertexLayout	= nullptr;
		RenderPrimitiveType m_primitive		= RenderPrimitiveType::Triangles;
	};


	SGE_INLINE const VertexLayout* RenderSubMesh::vertexLayout() const { return m_mesh->m_vertexLayout; }
	SGE_INLINE RenderPrimitiveType RenderSubMesh::primitive	  () const { return m_mesh->m_primitive;	}
}
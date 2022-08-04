#pragma once

#include <sge_render/backend/base/Render_Common.h>

#include <sge_render/buffer/RenderGpuBuffer.h>
#include <sge_render/vertex/Vertex.h>

#include <sge_render/mesh/DrawableUnit.h>

namespace sge 
{
	class EditMesh;
	class RenderMesh;

	class RenderSubMesh : public DrawableUnit {
	public:
		void create(const EditMesh& src);
		void clear ();

	friend class RenderMesh;
	};

	class RenderMesh : public NonCopyable {
	public:
		using SubMesh = RenderSubMesh;

		Span<SubMesh> subMeshes() { return m_subMeshes; }

		void create(const EditMesh& editMesh);
		void clear();

		void setSubMeshCount(size_t newSize);
	protected:
		Vector_<SubMesh, 1> m_subMeshes;

		const VertexLayout* m_vertexLayout	= nullptr;
		RenderPrimitiveType m_primitive		= RenderPrimitiveType::Triangles;
	};
}
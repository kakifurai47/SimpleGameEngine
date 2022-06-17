#pragma once

#include <sge_render/buffer/RenderGpuBuffer.h>
#include <sge_render/vertex/Vertex.h>

namespace sge {
	class EditMesh;
	class RenderMesh : public NonCopyable {
	public:
		void create(const EditMesh& editMesh);

		SPtr<RenderGpuBuffer> m_vertexBuf;
		SPtr<RenderGpuBuffer> m_indexBuf;

	};
}
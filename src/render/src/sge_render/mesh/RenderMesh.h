#pragma once

#include "../vertex/Vertex.h"

namespace sge
{
	class EditMesh;
	class RenderMesh : public NonCopyable
	{
	public:
		RenderMesh(const EditMesh& mesh);








	private:
		Vector<u8> m_vtxData;
		Vector<u8> m_idxData;
		
	};
}
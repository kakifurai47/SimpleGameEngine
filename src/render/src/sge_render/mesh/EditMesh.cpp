#include "EditMesh.h"
#include <sge_render/mesh/WavefrontObjLoader.h>

namespace sge {
	void EditMesh::loadObj(StrView filename) {
		WavefrontObjLoader objLoader;
		objLoader.loadFile(*this, filename);
	}

	void EditMesh::clear() {
		indicies.clear();

		pos.clear();
		for (auto& t : uv) {
			t.clear();
		}
		color.clear();
		
		normal.clear();
		tangent.clear();
		binormal.clear();
	}

}


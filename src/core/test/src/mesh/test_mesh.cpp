#include <sge_core/base/UnitTest.h>
#include <sge_render/mesh/WavefrontObjLoader.h>
#include <sge_render/mesh/RenderMesh.h>

namespace sge {


	class Test_Mesh : public UnitTestBase {
	public:
		void test_mesh() {
			//String filename = __FILE__;
			//filename.append("_sample.obj");
			//
			//EditMesh editMesh;
			//editMesh.loadObj(filename);

			//RenderMesh mesh;
			//mesh.create(editMesh);

		}

	};



}

void test_mesh() {
	using namespace sge;
	SGE_TEST_CASE(Test_Mesh, test_mesh());
}
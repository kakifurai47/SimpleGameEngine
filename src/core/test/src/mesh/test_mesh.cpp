#include <sge_core/base/UnitTest.h>
#include <sge_render/mesh/WavefrontObjLoader.h>
#include <sge_render/mesh/RenderMesh.h>

namespace sge {


	class Test_Mesh : public UnitTestBase {
	public:
		void test_mesh() {
			String filename = __FILE__;
			filename.append("_sample.obj");
			
			EditMesh editMesh;
			editMesh.loadObj(filename);

			RenderMesh mesh;
			mesh.create(editMesh);

			size_t a = std::numeric_limits<u32>::max() + 1;
			//u32 a = std::numeric_limits<u32>::max();

			

			SGE_DUMP_VAR(static_cast<u32>(a));




		}

	};



}

void test_mesh() {
	using namespace sge;
	SGE_TEST_CASE(Test_Mesh, test_mesh());
}
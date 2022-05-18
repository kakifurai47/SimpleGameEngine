#include <sge_core/base/UnitTest.h>
#include <sge_render/vertex/Vertex.h>


#include <typeinfo>

namespace sge {



class Test_Vertex : public UnitTestBase {
public:

	void test_layout() {
		using SMT = Vertex_SemanticType;
		using FMT = Render_FormatType;

		{
			using V0  = VertexLib::PosTexCol;
			V0 v0;

			v0.position<0>().x = 4;
			auto b = v0.color<3>().x;
			SGE_DUMP_VAR(b);

		}
	}
};

} // namespace 

void test_vertex() {
	using namespace sge;
	SGE_TEST_CASE(Test_Vertex, test_layout());
}

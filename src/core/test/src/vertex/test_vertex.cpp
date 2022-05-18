#include <sge_core/base/UnitTest.h>
#include <sge_render/vertex/Vertex.h>


#include <typeinfo>

namespace sge {


class Test_Vertex : public UnitTestBase {
public:

	void test_layout() {
		using FMT = Render_FormatType;


		{
			using V0  = VertexLib::PosTexCol;
			meta::print_list(V0::SlotList{});

			V0 v0;

			
			
			eastl::get<0>(v0.data).x = 3;
			SGE_DUMP_VAR(sizeof(v0));


			eastl::tuple<float, int> tup01;


			eastl::get<0>(tup01) = 1;


		}
	}
};

} // namespace 

void test_vertex() {
	using namespace sge;
	SGE_TEST_CASE(Test_Vertex, test_layout());
}

#include <sge_core/base/UnitTest.h>

#include <sge_core/math/Vec2.h>
#include <sge_core/math/Vec3.h>
#include <sge_core/math/Vec4.h>

#include <sge_core/math/Quat4.h>
#include <sge_core/math/Mat4.h>

namespace sge {
	class Test_Vec : public UnitTestBase {
	public:
		void test_vec() {


		}
	};
}

void test_vec() {
	using namespace sge;
	SGE_TEST_CASE(Test_Vec, test_vec());
}
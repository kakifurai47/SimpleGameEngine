#include <sge_core/base/UnitTest.h>
#include <sge_core/alllocators/LinearAllocator.h>

namespace sge {

	class Test_LinearAllocator : public UnitTestBase {
	public:
		void test_linearAllocator() {
			
		}
	};

} // namespace 

void test_LinearAllocator() {
	using namespace sge;
	SGE_TEST_CASE(Test_LinearAllocator, test_linearAllocator());
}

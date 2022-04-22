#include <sge_core/base/UnitTest.h>
#include <sge_core/string/Fmt.h>

namespace sge {

class Test_Fmt : public UnitTestBase {
public:	
	void test_string() {

	}
};

} // namespace 

void test_Fmt() {
	using namespace sge;
	SGE_TEST_CASE(Test_Fmt, test_string());
}

#include <sge_core/base/UnitTest.h>
#include <sge_core/log/Log.h>

// SGE_OPTIMIZE_OFF

#define	RUN_TEST( fn )	\
	SGE_LOG("\n===== " #fn " ================================\n"); \
	void fn(); \
	fn(); \
//----

namespace sge {

	void run_temp_test() {
		//RUN_TEST(test_mesh);
		RUN_TEST(test_vertex);
		//RUN_TEST(test_parser);
		//RUN_TEST(test_JsonSE);
	}

	int test_main() {
		run_temp_test();
		SGE_LOG("\n\n==== Program Ended ==== \n");
		return 0;
	}

} // namespace

int main() {
	return sge::test_main();
}

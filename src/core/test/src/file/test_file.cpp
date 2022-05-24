#include <sge_core/base/UnitTest.h>
#include <sge_core/file/FileStream.h>

#include <fstream>
#include <sstream>

namespace sge {



	class Test_File : public UnitTestBase {
	public:

		void test_utf() {


			String filename(__FILE__);
			filename.append("_sample.txt");

			std::ifstream inFile(filename.c_str());
			std::stringstream strStream;
			strStream << inFile.rdbuf();

			const StringA src = strStream.str().c_str();

			SGE_DUMP_VAR(src);
		}
	};
}

void test_file() {
	using namespace sge;
	SGE_TEST_CASE(Test_File, test_utf());

}
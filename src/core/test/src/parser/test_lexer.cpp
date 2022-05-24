#include <sge_core/base/UnitTest.h>
#include <sge_core/parser/Lexer.h>
#include <sge_render/shader/ShaderForm.h>

#include <fstream>
#include <sstream>
#include <vector>
#include <algorithm>
#include <EASTL/algorithm.h>

namespace sge {

	void printVector(std::vector<int> v)
	{
		// lambda expression to print vector
		std::for_each(v.begin(), v.end(), [](int i)
			{
				std::cout << i << " ";
			});
		std::cout << std::endl;
	}

	void print(StrView v) {

		for (auto* i = v.begin(); i != v.end(); i++) {
			SGE_DUMP_VAR(*i);
		}
	}

	class Test_Lexer : public UnitTestBase {
	public:

		void test_lexer() {

			 
			String filename(__FILE__);
			filename.append("_sample.txt");

			std::ifstream inFile(filename.c_str());
			std::stringstream strStream;
			strStream << inFile.rdbuf(); //read the file
			
			const StringA src = strStream.str().c_str();

			Lexer lex;
			ShaderForm form;

			Lexer::Type outType{};
			String outString;

			for (;;) {
				bool get = lex.nextToken(src);				
				if (get) {
					lex.retrieve(outType, outString);

					if (outType == Lexer::Type::Command) {
						continue;
					}
					SGE_LOG("Token [{}]:	 [{}]", Lexer::_type2char(outType), outString);
				}else {
					break;
				}
			}
		}
	};
}

void test_parser() {
	using namespace sge;
	SGE_TEST_CASE(Test_Lexer, test_lexer());
}
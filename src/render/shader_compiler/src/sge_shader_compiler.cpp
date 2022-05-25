
#include <sge_core/app/ConsoleApp.h>
#include "ShaderParser.h"

namespace sge {


	class ShaderCompiler : public ConsoleApp {

	public:

	protected:
		virtual void onRun() {
			{
				String file = getExecutableFileName();
				String path = FilePath::dirname(file);
				path.append("/../../../../../../examples/Test101");
				Directory::setCurrent(path);

				auto dir = Directory::getCurrent();
				SGE_LOG("dir = {}", dir);
			}

			ShaderInfo form;
			StrView shaderFilename = "Assets/Shaders/test.shader";


			{
				ShaderParser parser;
				parser.readFile(form, shaderFilename);
			}





		}




	};


}

int main() {
	sge::ShaderCompiler app;
	app.run();
	return 0;
}

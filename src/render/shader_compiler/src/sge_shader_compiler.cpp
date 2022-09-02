
#include <sge_core/app/ConsoleApp.h>
#include <sge_core/serializer/json/JsonUtil.h>

#include "ShaderCompiler_DX11.h"
#include "ShaderParser.h"

namespace sge {
	class ShaderCompiler : public ConsoleApp {
	protected:

		void compileShader(StrView shaderFilename) 
		{
			auto* proj = ProjectSettings::instance();

			ShaderInfo info;
			
			String outputPath = Fmt("{}/{}", proj->importedPath(), shaderFilename);
			Directory::create(outputPath);
			
			{
				ShaderParser parser;
				parser.readFile(info, shaderFilename);

				auto jsonFilename = Fmt("{}/info.json", outputPath);
				JsonUtil::writeFile(jsonFilename, info, false);
			}

			{ // DX11
				size_t passIndex = 0;
				for (auto& pass : info.passes) {
					auto passOutPath = Fmt("{}/dx11/pass{}", outputPath, passIndex);

					if (pass.vsFunc.size()) {
						ShaderCompiler_DX11 c;
						c.compile(passOutPath, ShaderStageMask::Vertex, shaderFilename, pass.vsFunc);
					}
					if (pass.psFunc.size()) {
						ShaderCompiler_DX11 c;
						c.compile(passOutPath, ShaderStageMask::Pixel, shaderFilename, pass.psFunc);
					}

					passIndex++;
				}
			}
		}


		virtual void onRun() {
			
			String file = getExecutableFileName();
			String path = FilePath::dirname(file);
			path.append("/../../../../../../examples/Test101");

			auto* proj = ProjectSettings::instance();
			proj->setProjectRoot(path);
			//---------

			StrView shaderFilename[] = {
				"Assets/Shaders/terrain.shader",
				"Assets/Shaders/test.shader",
				"Assets/Shaders/editorgui.shader",
			};

			for (size_t i = 0; i < shaderFilename->size(); i++) 
			{
				compileShader(shaderFilename[i]);
			}

			SGE_LOG("----------end------------");
		}
	};
}

int main() {
	sge::ShaderCompiler app;
	app.run();
	return 0;
}

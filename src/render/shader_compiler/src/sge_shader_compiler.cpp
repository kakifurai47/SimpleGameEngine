
#include <sge_core/app/ConsoleApp.h>
#include <sge_core/json/Json.h>

#include "ShaderCompiler_DX11.h"
#include "ShaderParser.h"

namespace sge {
	class ShaderCompiler : public ConsoleApp {
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

			ShaderInfo info;
			StrView shaderFilename = "Assets/Shaders/test.shader";
			
			String outputPath = Fmt("LocalTemp/Imported/{}", shaderFilename);
			Directory::create(outputPath);
			
			TempString code;
			auto codeFilename = Fmt("{}/code.hlsl", outputPath);
			auto infoFilename = Fmt("{}/info.json", outputPath);
			{
				ShaderParser parser;
				parser.readFile(info, shaderFilename);
				
				for (size_t i = 0; i < parser.line(); i++) {
					code += "//\n";
				}
				auto remain = parser.getRemainSource();
				code += remain;
				
				File::writeFileIfChanged(codeFilename, code, true);
			}
			{
				size_t passIndex = 0;
				for (auto& pass : info.passes) {
					auto passOutPath = Fmt("{}/dx11/pass{}", outputPath, passIndex);
				
					if (pass.vsFunc.size()) {
						ShaderCompiler_DX11 c;
						c.compile(passOutPath, ShaderStage::Vertex, codeFilename, pass.vsFunc);
						c.reflect(passOutPath, ShaderStage::Vertex, info);
					}
					if (pass.psFunc.size()) {
						ShaderCompiler_DX11 c;
						c.compile(passOutPath, ShaderStage::Pixel, codeFilename, pass.psFunc);
						c.reflect(passOutPath, ShaderStage::Pixel, info);
					}
					passIndex++;
				}
			}

			JsonSerializer se;
			se.write(info);
			se.save (infoFilename);

			SGE_LOG("----------end------------");
		}
	};
}

int main() {
	sge::ShaderCompiler app;
	app.run();
	return 0;
}

#include <sge_core/serializer/json/JsonUtil.h>
#include "Shader.h"

namespace sge {
	void Shader::create(StrView filename) {
		auto compiledPath = Fmt("LocalTemp/Imported/{}", filename);
		auto infoFilename = Fmt("{}/info.json", compiledPath);

		JsonUtil::readFile(infoFilename, m_info);
		onCreate(compiledPath);
	}
}

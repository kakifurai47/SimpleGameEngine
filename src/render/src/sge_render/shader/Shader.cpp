#include <sge_core/json/Json.h>

#include "Shader.h"

namespace sge {
	void Shader::create(StrView filename) {
		auto compiledPath = Fmt("LocalTemp/Imported/{}", filename);
		auto infoFilename = Fmt("{}/info.json", compiledPath);

		JsonDeserializer   se;
		se.load(infoFilename);
		se.read(m_info);

		onCreate(compiledPath);
	}
}

#include "Json.h"
#include <sge_core/file/File.h>
#include <sge_core/file/MemMapFile.h>

namespace sge {

	void JsonSerializer::save(StrView filename) {
		File::writeFileIfChanged(filename, m_json.dump(4).c_str(), true);
	}

	void JsonDeserializer::load(StrView filename) {
		MemMapFile memmap;
		memmap.open(filename);
		auto d = memmap.span();
		m_json = Json::parse({ d.data(), d.size() });
	}

}
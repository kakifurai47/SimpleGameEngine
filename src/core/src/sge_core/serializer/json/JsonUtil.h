#pragma once

#include <sge_core/base/Error.h>
#include <sge_core/file/File.h>
#include <sge_core/file/MemMapFile.h>
#include "JsonSerializer.h"
#include "JsonDeserializer.h"

namespace sge {

	struct JsonUtil {
		JsonUtil() = delete;

		template<class T> static void readFile			(StrView filename, T& obj);
		template<class T> static void writeFile			(StrView filename, T& obj, bool createDir, bool logResult = true);
		template<class T> static void writeFileIfChanged(StrView filename, T& obj, bool createDir, bool logResult = true);

		template<class T, size_t N> static void serialize  (String_<N>& json, T& obj);
		template<class T>			static void deserialize(StrView		json, T& obj);
	};

	template<class T> inline
	void JsonUtil::readFile(StrView filename, T& obj) {
		MemMapFile mm;
		mm.open(filename);

		auto str  = StrView_make(mm);
		deserialize(str, obj);
	}
	template<class T> inline
	void JsonUtil::writeFile(StrView filename, T& obj, bool createDir, bool logResult) {
		TempString json;
		serialize(json, obj);
		File::writeFile(filename, json, createDir, logResult);
	}

	template<class T> inline
	void JsonUtil::writeFileIfChanged(StrView filename, T& obj, bool createDir, bool logResult) {
		TempString json;
		serialize(json, obj);
		File::writeFileIfChanged(filename, json, createDir, logResult);
	}

	template<class T, size_t N> inline
	void JsonUtil::serialize(String_<N>& outJson, T& obj) {
		Json json;
		JsonSerializer2 se(json);
		se.io(obj);
		auto temp = json.dump(1, '\t');
		outJson = temp.c_str();
	}

	template<class T> inline
	void JsonUtil::deserialize(StrView json, T& obj) {
		auto j = Json::parse(json.begin(), json.end());
		JsonDeserializer2 de(j);
		de.io(obj);
	}

}
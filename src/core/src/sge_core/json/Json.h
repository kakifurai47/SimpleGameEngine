#pragma once

#include <nlohmann/json.hpp>

namespace sge {
	using Json = nlohmann::json;

	class JsonSerializer : public NonCopyable {
	public :
		template<class OBJ> static constexpr
		bool isSerializable() {
			return nlohmann::detail::has_to_json<Json, OBJ>::value;
		}

		template<class OBJ>
		void write(OBJ& obj) { io(m_json, obj); }

		template<class OBJ> 
		void io(Json& json, OBJ& obj) {
			if constexpr (isSerializable<OBJ>()) {
				json = obj;
			}
			else {
				jsonIO(*this, obj, json);
			}
		}

		template<class T, size_t N> void io(Json& json, StringT<T, N>& s);
		template<class T, size_t N> void io(Json& json, Vector_<T, N>& v);

		void save(StrView filename);

		Json& getJson() { return m_json; }
	private:
		Json m_json;

	};

	class JsonDeserializer : public NonCopyable {
	public:
		template<class OBJ> static constexpr
			bool isDeserializable() {
			return nlohmann::detail::has_from_json<Json, OBJ>::value;
		}

		template<class OBJ>
		void read(OBJ& obj) { io(m_json, obj); }

		template<class OBJ>
		void io(Json& json, OBJ& obj) {
			if constexpr (isDeserializable<OBJ>()) {
				obj = json.get<OBJ>();
			}
			else {
				jsonIO(*this, obj, json);
			}
		}

		template<class T, size_t N> void io(Json& json, StringT<T, N>& s);
		template<class T, size_t N> void io(Json& json, Vector_<T, N>& v);

		void load(StrView filename);

		Json& getJson() { return m_json; }
	private:
		Json m_json;
	};

	template<class T, size_t N>
	void JsonSerializer::io(Json& json, StringT<T, N>& s) { json = s.c_str(); }

	template<class T, size_t N>
	void JsonSerializer::io(Json& json, Vector_<T, N>& v) {
		for (auto& i : v) { io(json.emplace_back(), i); }
	}

	template<class T, size_t N>
	void JsonDeserializer::io(Json& json, StringT<T, N>& s) {
		s = json.get<std::string>().c_str();
	}
	
	template<class T, size_t N>
	void JsonDeserializer::io(Json& json, Vector_<T, N>& v) {
		for (auto& i : json) { io(i, v.emplace_back()); }
	}
}



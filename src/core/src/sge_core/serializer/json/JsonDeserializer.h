#pragma once

#include "sge_core/base/sge_base.h"
#include "sge_core/meta/type_trait.h"

namespace sge {

	class JsonDeserializer2 : public NonCopyable {
		using Util = meta::TypeTraitUtil;
	public:
		JsonDeserializer2(Json& json)
			: m_json(json) {
			m_stack.emplace_back(&json);
		}

		template<class T>
		void named_io(const char* name, T& out) {
			toObjectMember(name, out);
		}

		template<class T>
		void io(T& out) {
			if		constexpr (Util::isEnum		  <T>()) { toEnum  (out);	return; }
			else if constexpr (Util::isArithmetic <T>()) { toValue (out);	return; }
			else if constexpr (Util::isString	  <T>()) { toString(out);	return; }
			else if constexpr (Util::isVector	  <T>()) { toVector(out);	return; }
			else if constexpr (Util::isSerDes	  <T>()) { toObject(out);	return; }
			SGE_ASSERT(false);
		}
	private:

		template<class T>
		void toValue(T& out) {
			auto* cur = m_stack.back();
			if (cur->is_null())
				throw SGE_ERROR("value is null");
			out = *cur;
		}

		template<class T>
		void toEnum(T& out) {
			auto s = toStrView<char>();
			if (!enumTryParse(out, s))
				throw SGE_ERROR("errror parse enum {}", s);
		}

		template<class T>
		void toObject(T& out) {
			auto* cur = m_stack.back();
			if (!cur->is_object())
				throw SGE_ERROR("object expected");

			onSerDes(*this, out);
		}

		template<class T>
		void toObjectMember(const char* name, T& out) {
			auto* cur = m_stack.back();
			if (!cur->is_object())
				throw SGE_ERROR("object expected");

			auto& j = cur->operator[](name);
			m_stack.emplace_back(&j);
			io(out);
			m_stack.pop_back();
		}

		template<class T>
		StrViewT<T> toStrView() {
			auto* cur = m_stack.back();
			if (!cur->is_string())
				throw SGE_ERROR("string expected");

			auto* p = cur->get_ptr<Json::string_t*>();
			return { p->data(), p->size() };
		}

		template<class T, size_t N>
		void toString(StringT<T, N>& out) {
			out = toStrView<T>();
		}

		template<class VECTOR>
		void  toVector(VECTOR& out) {
			auto* cur = m_stack.back();
			if (!cur->is_array())
				throw SGE_ERROR("array expected");

			auto& arr = *cur->get_ptr<Json::array_t*>();
			out.reserve(arr.size());

			for (auto& i : arr) {
				auto& e = out.emplace_back();
				m_stack.emplace_back(&i);
				io(e);
				m_stack.pop_back();
			}
		}

	private:
		Vector_<Json*, 64> m_stack;
		Json& m_json;
	};
}
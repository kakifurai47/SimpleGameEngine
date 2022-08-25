#pragma once

#include "sge_core/base/sge_base.h"
#include "sge_core/meta/type_trait.h"

namespace sge {
	class JsonSerializer2 : public NonCopyable {
	public:
		JsonSerializer2(Json& out)
			: m_json(out) {
			m_stack.emplace_back(&out);
		}

		template<class T>
		void named_io(const char* name, T& t) {
			toObjectMember(name, t);
		}

		template<class T>
		void io(T& t) {
			if		constexpr (meta::isEnum		  <T>()) { toEnum	(t);		return; }
			else if constexpr (meta::isArithmetic <T>()) { toValue	(t);		return; }
			else if constexpr (meta::isString	  <T>()) { toStrView(t.view());	return; }
			else if constexpr (meta::isVector	  <T>()) { toSpan	(t.span());	return; }
			else if constexpr (meta::isSerDes	  <T>()) { toObject	(t);		return; }
			else {
				SGE_ASSERT(false); 
			}
		}
	private:
		inline
		void throwIfAssigned(Json& j) {
			if (j.is_null()) return;
			throw SGE_ERROR("already contains value");
		}

		template<class T>
		void toObject(T&& obj) {
			auto* cur = m_stack.back();
			throwIfAssigned(*cur);
			*cur = Json::object();
			onSerDes(*this, obj);
		}

		template<class T>
		void toEnum(T&& en) {
			auto* cur = m_stack.back();
			throwIfAssigned(*cur);
			toValue( enumStr(en));
		}

		template<class T>
		void toValue(T&& val) {
			auto* cur = m_stack.back();
			throwIfAssigned(*cur);
			*cur = val;
		}

		template<class T>
		void toObjectMember(const char* name, T&& mem) {
			auto* cur = m_stack.back();
			if (!cur->is_object())
				throw SGE_ERROR("not inside object");
			auto& j = cur->operator[](name);
			m_stack.emplace_back(&j);
			io(mem);
			m_stack.pop_back();
		}

		template<class T>
		void toSpan(Span<T> span) {
			auto* cur = m_stack.back();
			throwIfAssigned(*cur);
			*cur =  Json::array();
			auto* arr = cur->get_ptr<Json::array_t*>();
			arr->reserve(span.size());

			for (auto& i : span) {
				auto& e = cur->emplace_back();
				m_stack.emplace_back(&e);
				io(i);
				m_stack.pop_back();
			}
		}

		template<class T>
		void toStrView(StrViewT<T> str) {
			auto* cur = m_stack.back();
			throwIfAssigned(*cur);
			*cur = "";
			auto* dst = cur->get_ptr<Json::string_t*>();
			dst->assign(str.begin(), str.end());
		}

	private:
		Vector<Json*, 64> m_stack;
		Json& m_json;
	};
}
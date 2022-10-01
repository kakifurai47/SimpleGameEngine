#pragma once

#include "sge_core/meta/type_trait.h"

namespace sge
{
	class TypeInfo;
	class Object;

	template<class T> const TypeInfo*  sge_typeof();
	template<class DST> DST sge_cast  (Object* src);

	struct TypeInfoUtil
	{
		template<class T> static
		const TypeInfo* getContainerElementInfo() {
			if constexpr (meta::is_container<T>()) { return sge_typeof<T::ElementType>(); }
			else								   { return nullptr; }
		}
	};

	class FieldInfo
	{
	public:

		FieldInfo() = default;

		template<class T, class FIELD>
		FieldInfo(const char* name_, FIELD T::* fieldPtr_)
			: fieldType(sge_typeof<FIELD>())
			, name(name_)
			, offset(memberOffset(fieldPtr_))
		{
		}

		const TypeInfo* const	fieldType   = nullptr;
		const char* const		name	    = "";
		const intptr_t			offset	    = 0;
	};


	class TypeInfo
	{
		using Util = TypeInfoUtil;
	public:

		TypeInfo() = default;

		bool isKindOf(const TypeInfo* t) const
		{
			const TypeInfo* p = this;
			while (p) {
				if (p == t) return true;
				p = p->base;
			}
			return false;
		}

		template<class T> bool isKindOf() const { return isKindOf(sge_typeof<T>()); }



		const char*      name	 = "";
		const TypeInfo*  base	 = nullptr;
			  size_t     size	 = 0;

		bool			 isContainer			 = false;
		const TypeInfo*  containerElement		 = nullptr;
//		size_t			 containerSize			 = 0;
//		bool			 containerEnableOverflow = false;

//		FieldInfo* fields;

		Span<FieldInfo> fields;
	};

	template<class T>
	class TypeInfoInit_NoBase : public TypeInfo
	{
	public:
		TypeInfoInit_NoBase(const char* name_) {
			this->name			   = name_;
			this->size			   = sizeof(T);
			this->containerElement = Util::getContainerElementInfo<T>();
		}
	};

	template<class T, class BASE>
	class TypeInfoInit : public TypeInfoInit_NoBase<T>
	{
	public:
		TypeInfoInit(const char* name_) : TypeInfoInit_NoBase(name_)
		{
			static_assert(std::is_base_of<BASE, T>::value);
			base = sge_typeof<BASE>();
		}
	};

	




#define SGE_TYPE_INFO(T, BASE) \
	private: \
		class TI_Base : public TypeInfoInit<T, BASE> \
		{ \
		public: \
			TI_Base() : TypeInfoInit<T, BASE>(#T) {} \
			\
			static Span<FieldInfo> s_fields(); \
		}; \
	public: \
		static  const TypeInfo* s_typeInfo(); \
		virtual const TypeInfo*   typeInfo() const override { return sge_typeof<T>(); } \
//-----

#define SGE_GET_TYPE_IMPL(T) \
	const TypeInfo* T::s_typeInfo() { \
		class TI : public TI_Base { \
		public: \
			TI() : TI_Base() { fields = s_fields(); } \
		}; \
		static TI ti; \
		return &ti; \
	} \
//-----
	
	class Object : public RefCountBase {
		using This = Object;
	public:
		static  const TypeInfo* s_typeInfo();
		virtual const TypeInfo*   typeInfo() const { return sge_typeof<This>(); }

		virtual ~Object() = default;
	};

	template<class T, class ENABLE = void>
	class TypeClass {
	public:
		static const TypeInfo* s_info() { return T::s_typeInfo(); }
	};

	template<class T> inline const TypeInfo* sge_typeof() { return TypeClass<T>::s_info(); }

	template<> const TypeInfo* sge_typeof<bool>();

	template<> const TypeInfo* sge_typeof<u8  >();
	template<> const TypeInfo* sge_typeof<u16 >();
	template<> const TypeInfo* sge_typeof<u32 >();
	template<> const TypeInfo* sge_typeof<u64 >();
	template<> const TypeInfo* sge_typeof<u128>();

	template<> const TypeInfo* sge_typeof<i8 >();
	template<> const TypeInfo* sge_typeof<i16>();
	template<> const TypeInfo* sge_typeof<i32>();
	template<> const TypeInfo* sge_typeof<i64>();

//	template<> const TypeInfo* sge_typeof<f16 >();
	template<> const TypeInfo* sge_typeof<f32 >();
	template<> const TypeInfo* sge_typeof<f64 >();
	template<> const TypeInfo* sge_typeof<f128>();

	template<> const TypeInfo* sge_typeof<char16_t>();
	template<> const TypeInfo* sge_typeof<char32_t>();
	template<> const TypeInfo* sge_typeof<wchar_t >();
		
	template<class DST>
	inline DST sge_cast(Object* src)
	{
		static_assert(std::is_pointer<DST>::value);
		SGE_ASSERT(src->typeInfo());

		if (!src) return nullptr;
		if (!src->typeInfo()->isKindOf<std::remove_pointer<DST>::type>()) {
			return nullptr;
		}
		return static_cast<DST>(src);
	}

#define SGE_CONTAINER_TYPECLASS(CONTAINER) \
	template<class T> class TypeClass<CONTAINER<T>> { \
	public: \
		static const TypeInfo* s_info(); \
	}; \

	SGE_CONTAINER_TYPECLASS(Vec2)
	SGE_CONTAINER_TYPECLASS(Vec3)
	SGE_CONTAINER_TYPECLASS(Vec4)

	SGE_CONTAINER_TYPECLASS(Mat4)

	SGE_CONTAINER_TYPECLASS(ColorR)
	SGE_CONTAINER_TYPECLASS(ColorRG)
	SGE_CONTAINER_TYPECLASS(ColorRGB)
	SGE_CONTAINER_TYPECLASS(ColorRGBA)
	SGE_CONTAINER_TYPECLASS(ColorL)
	SGE_CONTAINER_TYPECLASS(ColorLA)
}
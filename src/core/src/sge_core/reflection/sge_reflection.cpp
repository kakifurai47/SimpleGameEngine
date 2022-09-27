#include "sge_reflection.h"

namespace sge
{

#define SGE_TYPE_OF_IMPL(TYPE) \
	template<> \
	const TypeInfo* sge_typeof<TYPE>() \
	{ \
		static TypeInfo info(TYPE{}, #TYPE); \
		return &info; \
	} \
	\

	SGE_TYPE_OF_IMPL(bool)
	
	SGE_TYPE_OF_IMPL(u8)
	SGE_TYPE_OF_IMPL(u16)
	SGE_TYPE_OF_IMPL(u32)
	SGE_TYPE_OF_IMPL(u64)
	SGE_TYPE_OF_IMPL(u128)
 
	SGE_TYPE_OF_IMPL(i8)
	SGE_TYPE_OF_IMPL(i16)
	SGE_TYPE_OF_IMPL(i32)
	SGE_TYPE_OF_IMPL(i64)
 
//	SGE_TYPE_OF_IMPL(f16)
	SGE_TYPE_OF_IMPL(f32)
	SGE_TYPE_OF_IMPL(f64)
	SGE_TYPE_OF_IMPL(f128)

	SGE_TYPE_OF_IMPL(char16_t)
	SGE_TYPE_OF_IMPL(char32_t)
	SGE_TYPE_OF_IMPL(wchar_t)
	
	
	
	const TypeInfo* Object::s_typeInfo() {
		static TypeInfoInit_NoBase<Object> ti("Object");
		return &ti;
	}

}
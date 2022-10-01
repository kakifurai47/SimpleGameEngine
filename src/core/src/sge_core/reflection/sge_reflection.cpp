#include "sge_reflection.h"

namespace sge
{

#define SGE_TYPE_OF_IMPL(TYPE) \
	template<> \
	const TypeInfo* sge_typeof<TYPE>() \
	{ \
		static TypeInfoInit_NoBase<TYPE> ti(#TYPE); \
		return &ti; \
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


#define SGE_TYPECLASS_FIELD_1(CONTAINER, FIELD1) \
	template<class T> \
	const TypeInfo* TypeClass<CONTAINER<T>>::s_info() \
	{ \
		static TypeInfoInit_NoBase<CONTAINER<T>> ti(#CONTAINER); \
		static FieldInfo fi[] = \
		{ \
			{#FIELD1, &CONTAINER<T>::FIELD1}, \
		}; \
		ti.isContainer		= true; \
		ti.containerElement = sge_typeof<T>(); \
		ti.fields			= fi; \
		\
		return &ti; \
	} \
//-----
	
#define SGE_TYPECLASS_FIELD_2(CONTAINER, FIELD1, FIELD2) \
	template<class T> \
	const TypeInfo* TypeClass<CONTAINER<T>>::s_info() \
	{ \
		static TypeInfoInit_NoBase<CONTAINER<T>> ti(#CONTAINER); \
		static FieldInfo fi[] = \
		{ \
			{#FIELD1, &CONTAINER<T>::FIELD1}, \
			{#FIELD2, &CONTAINER<T>::FIELD2}, \
		}; \
		ti.isContainer		= true; \
		ti.containerElement = sge_typeof<T>(); \
		ti.fields			= fi; \
		\
		return &ti; \
	} \
//-----

#define SGE_TYPECLASS_FIELD_3(CONTAINER, FIELD1, FIELD2, FIELD3) \
	template<class T> \
	const TypeInfo* TypeClass<CONTAINER<T>>::s_info() \
	{ \
		static TypeInfoInit_NoBase<CONTAINER<T>> ti(#CONTAINER); \
		static FieldInfo fi[] = \
		{ \
			{#FIELD1, &CONTAINER<T>::FIELD1}, \
			{#FIELD2, &CONTAINER<T>::FIELD2}, \
			{#FIELD3, &CONTAINER<T>::FIELD3}, \
		}; \
		ti.isContainer		= true; \
		ti.containerElement = sge_typeof<T>(); \
		ti.fields			= fi; \
		\
		return &ti; \
	} \
//-----

#define SGE_TYPECLASS_FIELD_4(CONTAINER, FIELD1, FIELD2, FIELD3, FIELD4) \
	template<class T> \
	const TypeInfo* TypeClass<CONTAINER<T>>::s_info() \
	{ \
		static TypeInfoInit_NoBase<CONTAINER<T>> ti(#CONTAINER); \
		static FieldInfo fi[] = \
		{ \
			{#FIELD1, &CONTAINER<T>::FIELD1}, \
			{#FIELD2, &CONTAINER<T>::FIELD2}, \
			{#FIELD2, &CONTAINER<T>::FIELD3}, \
			{#FIELD2, &CONTAINER<T>::FIELD4}, \
		}; \
		ti.isContainer		= true; \
		ti.containerElement = sge_typeof<T>(); \
		ti.fields			= fi; \
		\
		return &ti; \
	} \
//-----

	//Vec
	SGE_TYPECLASS_FIELD_2(Vec2, x, y)
	SGE_TYPECLASS_FIELD_3(Vec3, x, y, z)
	SGE_TYPECLASS_FIELD_4(Vec4, x, y, z, w)
	
	template class TypeClass<Vec2f>;
	template class TypeClass<Vec2d>;
	template class TypeClass<Vec2i>;
	template class TypeClass<Vec2b>;
	
	template class TypeClass<Vec3f>;
	template class TypeClass<Vec3d>;
	
	template class TypeClass<Vec4f>;
	template class TypeClass<Vec4d>;

	//Mat
	SGE_TYPECLASS_FIELD_4(Mat4, cx, cy, cz, cw)

	template class TypeClass<Mat4f>;
	template class TypeClass<Mat4d>;

	//Color
	SGE_TYPECLASS_FIELD_1(ColorR,    r)
	SGE_TYPECLASS_FIELD_2(ColorRG,   r, g)
	SGE_TYPECLASS_FIELD_3(ColorRGB,  r, g, b)
	SGE_TYPECLASS_FIELD_4(ColorRGBA, r, g, b, a)

	SGE_TYPECLASS_FIELD_1(ColorL,  l)
	SGE_TYPECLASS_FIELD_2(ColorLA, l, a)

	template class TypeClass<ColorRGBAf>;
	template class TypeClass<ColorRGBAb>;
	template class TypeClass<ColorRGBAs>;

	template class TypeClass<ColorLf>;
	template class TypeClass<ColorLb>;
	template class TypeClass<ColorLs>;

	template class TypeClass<ColorLAf>;
	template class TypeClass<ColorLAb>;
	template class TypeClass<ColorLAs>;
}
#pragma once

#include <sge_core/reflection/sge_reflection.h>

namespace sge
{
	class ClassA : public Object {
	public:
		SGE_TYPE_INFO(ClassA, Object)
	private:
		int  a_test_int  = 3;
		bool a_test_bool = true;
	};

	class ClassB : public ClassA
	{
	public:

		SGE_TYPE_INFO(ClassB, ClassA)

		bool  classB_bool  = false;
		int   classB_int   = 0;
		float classB_float = 3.0f;

//		Vec2f		classB_vec2f;
//		Vector<int> classB_vector;
	};

	class ClassC : public Object
	{
		SGE_TYPE_INFO(ClassC, Object)



	};
	



}
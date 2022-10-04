#include <sge_core/base/UnitTest.h>
#include <sge_core/reflection/sge_reflection.h>

#include "test_reflection_class.h"

namespace sge {

	class Test_Reflection : public UnitTestBase {
	public:

		void test_typeInfo()
		{
			auto* b = ClassB::s_typeInfo();

			SGE_DUMP_VAR(b->name);
			SGE_DUMP_VAR(b->size);
			SGE_DUMP_VAR(b->base->name);

			auto fs = b->fieldArray;

			for (auto& f : fs) {
				SGE_DUMP_VAR(f.name);
				SGE_DUMP_VAR(f.offset);
			}
		}

		void test_cast()
		{
			ClassB  b;

			ClassA* a = &b;

			SGE_DUMP_VAR(a->typeInfo()->isKindOf<ClassB>());

			auto* pa = sge_cast<ClassA*>(a);
			auto* pb = sge_cast<ClassB*>(a);
			auto* pc = sge_cast<ClassC*>(a);
			
			SGE_ASSERT(pa);
			SGE_ASSERT(pb);
			SGE_ASSERT(pc == false);
		}

		void test_iterator()
		{
			auto* i = sge_typeof<ClassB>();

			for (auto& f : i->fields())
			{
				SGE_DUMP_VAR(f.name, f.offset);
			}
		}


		void test_container()
		{



			auto* p = TypeClass<ColorRGBAf>::s_info();
			SGE_DUMP_VAR(p->name);
			SGE_DUMP_VAR(p->containerElement->name);
		}

		void test_reflect()
		{
//			test_typeInfo(); 
//			test_cast();
		}

	};
}

void test_reflection()
{
	using namespace sge;
	SGE_TEST_CASE(Test_Reflection, test_iterator());


}
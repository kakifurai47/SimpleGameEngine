#include "test_reflection_class.h"

namespace sge
{
	Span<FieldInfo> ClassA::TI_Base::s_fields()
	{
		static FieldInfo fi[] = {
			{ "a_test_int",		&ClassA::a_test_int	 },
			{ "a_test_bool",	&ClassA::a_test_bool },
		};
		return fi;
	}
	
	Span<FieldInfo> ClassB::TI_Base::s_fields()
	{
		static FieldInfo fi[] = {
			{"classB_bool",		&ClassB::classB_bool	},
			{"classB_int",		&ClassB::classB_int		},
			{"classB_float",	&ClassB::classB_float	},
//			{"classB_vec2f",	&ClassB::classB_vec2f	},
//			{"classB_vector"	&ClassB::classB_vector	},
		};
		return fi;
	}

	Span<FieldInfo> ClassC::TI_Base::s_fields()
	{
		return {};
	}

	SGE_GET_TYPE_IMPL(ClassA)
	SGE_GET_TYPE_IMPL(ClassB)
	SGE_GET_TYPE_IMPL(ClassC)

}



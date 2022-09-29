#include "Component.h"

namespace sge
{

	Span<FieldInfo> Component::TI_Base::s_fields() {
		return {};
	}

	SGE_GET_TYPE_IMPL(Component);

}
#include "Transform.h"

namespace sge
{
	TransformSystem* TransformSystem::instance() 
	{
		static TransformSystem sys;
		return &sys;
	}





	Component* TransformSystem::onCreateComponent()
	{
		return new Transform();
	}

	Span<FieldInfo> Transform::TI_Base::s_fields() 
	{
		static FieldInfo fi[] =
		{
			{"Position", &Transform::postion },
			{"Scale",    &Transform::scale   },
		};
		return fi;
	}
	
	
	SGE_GET_TYPE_IMPL(Transform)




}
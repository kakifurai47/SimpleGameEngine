#pragma once

#include <sge_engine/ecs/ComponentSystem.h>

namespace sge
{

	class TransformSystem : public ComponentSystem
	{
	public:

		static TransformSystem* instance();

		virtual Component* onCreateComponent();
		virtual void onDestroyComponent(Component* c) {}
	};





	class Transform : public Component_Impl<TransformSystem, false>
	{
	public:
		SGE_TYPE_INFO(Transform, Component);



		float x, y, z;
	};






}
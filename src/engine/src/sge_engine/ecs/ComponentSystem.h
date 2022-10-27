#pragma once 

#include "Component.h"

namespace sge
{
	class ComponentSystem : public NonCopyable
	{
	public:

		void destroyComponent(Component* c) { onDestroyComponent(c); }

	protected:
		virtual void onDestroyComponent(Component* c) = 0;

	};




}
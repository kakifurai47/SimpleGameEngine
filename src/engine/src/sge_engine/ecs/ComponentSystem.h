#pragma once 

#include "Component.h"

namespace sge
{
	class ComponentSystem : public NonCopyable
	{
	public:
		template<class T>
		SPtr<T> createComponent()			   { return static_cast<T*> ( onCreateComponent() ); }
		void    destroyComponent(Component* c) { onDestroyComponent(c); }

	protected:

		virtual Component* onCreateComponent()		  = 0;
		virtual void onDestroyComponent(Component* c) = 0;
	};




}
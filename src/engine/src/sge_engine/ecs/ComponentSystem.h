#pragma once 

#include "Component.h"

namespace sge
{
	class ComponentSystem : public NonCopyable
	{

	public:
		template<class T>
		SPtr<T> createComponent() {
			auto* c = onCreateComponent();
			m_components.emplace_back (c);
			return static_cast<T*>(c);
		}
		void   destroyComponent(Component* c);

//	protected:
		virtual Component* onCreateComponent()		  = 0;
		virtual void onDestroyComponent(Component* c) = 0;

		Vector<Component*> m_components; 



	};




}
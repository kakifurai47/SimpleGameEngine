#include "ComponentSystem.h"

namespace sge
{
//	SPtr<Component> ComponentSystem::createComponent() {
//		auto* c = onCreateComponent();
//		m_components.emplace_back(c);
//		return c;
//	}

	void ComponentSystem::destroyComponent(Component* c) {
		onDestroyComponent(c);
		m_components.erase_first_unsorted(c);
	}


}
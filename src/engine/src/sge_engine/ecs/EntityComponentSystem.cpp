#include "EntityComponentSystem.h"

namespace sge
{
	Component::~Component() {
		system()->destroyComponent(this);
	}

	void ComponentSystem::destroyComponent(Component* c) {
		onDestroyComponent(c);
		m_components.erase_first_unsorted(c);
	}
}
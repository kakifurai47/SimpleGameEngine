#include "EntityComponentSystem.h"

namespace sge
{
	Span<FieldInfo> Component::TI_Base::s_fields() {
		return {};
	}

	SGE_GET_TYPE_IMPL(Component);


	SPtr<Component> ComponentSystem::createComponent() {
		auto* c = onCreateComponent();
		m_components.emplace_back (c);
		return c;
	}

	void ComponentSystem::destroyComponent(Component* c) {
		onDestroyComponent(c);
		m_components.erase_first_unsorted(c);
	}
}
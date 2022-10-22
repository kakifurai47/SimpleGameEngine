#include "EditorSelection.h"

namespace sge
{
	bool EntitySelection::has(EntityId id) const {
		for (auto i : m_entities) { if (i == id) return true; }
		return false;
	}

	void EntitySelection::add(EntityId id) {
		m_entities.emplace_back(id);
		m_lastSelect = id;
	}

	void EntitySelection::select(EntityId id) {
		deselectAll();
		add(id);
	}

	void EntitySelection::deselectAll() {
		m_entities.clear();
		m_lastSelect = EntityId::None;
	}

}
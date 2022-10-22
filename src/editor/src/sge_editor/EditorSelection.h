#pragma once

#include <sge_engine/ecs/Entity.h>


namespace sge
{
	class EntitySelection : public NonCopyable
	{
	public:

		EntityId lastSelect() { return m_lastSelect; }

		bool has		(EntityId id) const;
		void add		(EntityId id);
		void select		(EntityId id);
		void deselectAll();

	private:

		EntityId				m_lastSelect = EntityId::None;
		Vector<EntityId>		m_entities;
	};
}
#pragma once

#include <sge_engine/ecs/Entity.h>

namespace sge
{

	class Scene : public NonCopyable
	{
	public:

		Span<Entity*> entities() { return m_entities; }


		Entity* addEntity();
		Entity* addEntity(StrView name);

		Entity* findEntityById(EntityId id);


	private:
		u64							m_nextEntityId = 0;

		Vector<Entity*>				m_entities;
		Map<EntityId, SPtr<Entity>> m_entityIdMap;
	};

	inline
	Entity* Scene::findEntityById(EntityId id)
	{
		auto it = m_entityIdMap.find(id);
		if (it == m_entityIdMap.end()) { return nullptr; }
		return it->second;
	}





}
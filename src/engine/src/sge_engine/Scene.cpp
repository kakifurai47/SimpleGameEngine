#include "Scene.h"

namespace sge
{

	Entity* Scene::addEntity()
	{
		auto* p = new Entity(static_cast<EntityId>(++m_nextEntityId));
		m_entities.emplace_back(p);
		m_entityIdMap.emplace(p->id(), p);
		return p;
	}

	Entity* Scene::addEntity(StrView name)
	{
		auto* p = addEntity();
		p->setName(name);
		return p;
	}

}
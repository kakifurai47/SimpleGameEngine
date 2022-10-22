#pragma once

#include <sge_engine/ecs/Component/CTransform.h>

namespace sge
{

	enum class EntityId : u64 { None = 0 };


	class Entity : public Object
	{
	public:

		Entity(EntityId id_) : m_id(id_) {
			m_transform = addComponent<CTransform>();
		}

		void setName(StrView name_) { m_name = name_; }

		Span<SPtr<Component>> components()		 { return m_components; }
		CTransform*			  transform ()		 { return m_transform;  }
		EntityId			  id()		   const { return m_id;			}
		StrView				  name()	   const { return m_name;		}

		template<class T> WPtr<T> addComponent();
		template<class T> WPtr<T> getComponent();
		template<class T> void removeComponent();

	protected:

		EntityId				m_id;
		String					m_name;

		CTransform*				m_transform = nullptr;
		Vector<SPtr<Component>> m_components;
	};

	template<class T>
	void Entity::removeComponent() {
		auto*  t = m_components.begin();
		for (; t < m_components.end(); t++) 
		{
			if (sge_cast<T*>(*t)) {
				t->internal_set_entity(nullptr);
				m_components.erase_unsorted (t);
				return;
			}
		}		
	}

	template<class T> 
	WPtr<T> Entity::getComponent()
	{
		for (auto& s : m_components) {
			if (T* t = sge_cast<T*>(s)) { return t; }
		}
		return nullptr;
	}

	template<class T> 
	WPtr<T> Entity::addComponent() 
	{
		if (!T::s_allowMultiple())
		{
			if ( auto* p = getComponent<T>().ptr()) {
				throw SGE_ERROR("adding component : existing type");
			}
		}
		SPtr<T> p = T::System::instance()->createComponent<T>();
		m_components.emplace_back(p);
		p->internal_set_entity(this);

		return p.ptr();
	}






}
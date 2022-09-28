#pragma once

#include <sge_core/base/sge_base.h>
#include <sge_core/reflection/sge_reflection.h>

#include <sge_core/math/Quat4.h>


namespace sge {

	class ComponentSystem;


	class Component : public Object
	{
	public:
		static bool allowMultiple() { return false; }

		virtual ~Component();
		virtual ComponentSystem* system() = 0;
	private:
	};

	class ComponentSystem : public NonCopyable
	{
	public:
		SPtr<Component>	createComponent() { return onCreateComponent(); }
		void destroyComponent(Component* c);

	protected:
		virtual Component* onCreateComponent()		  = 0;
		virtual void onDestroyComponent(Component* c) = 0;

		Vector<Component*> m_components;
	};


	class Entity : public NonCopyable
	{
	public:
		template<class T> WPtr<T> addComponent();
		template<class T> WPtr<T> getComponent();
		template<class T> void removeComponent();

	protected:
		Vector<SPtr<Component>> m_components;
	};

	template<class T>
	void Entity::removeComponent() {
		auto*  t = m_components.begin();
		for (; t < m_components.end(); t++) {
			if (sge_cast<T>(t)) break;
		}
		m_components.erase_unsorted(t);
	}

	template<class T> 
	WPtr<T> Entity::getComponent()
	{
		for (auto& s : m_components) {
			if (auto* t = sge_cast<T>(s)) { return t; }
		}
		return nullptr;
	}

	template<class T> 
	WPtr<T> Entity::addComponent() 
	{
		if (!T::allowMultiple()) {
			if ( auto* p = getComponent<T>().ptr()) {
				throw SGE_ERROR("adding component : existing type");
				return p;
			}
		}
		auto p = T::System::instance()->createComponent();
		m_components.emplace_back(p);
		return newC;
	}
}


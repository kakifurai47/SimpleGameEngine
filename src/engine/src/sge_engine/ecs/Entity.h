#pragma once

//#include <sge_core/base/sge_base.h>
//#include <sge_core/pointer/WPtr.h>
//#include <sge_core/pointer/SPtr.h>
//
//
#include "Component.h"

namespace sge
{

	class Entity : public NonCopyable
	{
	public:		

		Span<SPtr<Component>>	    components() { return m_components; }

		template<class T> WPtr<T> addComponent();
		template<class T> WPtr<T> getComponent();
		template<class T> void removeComponent();
	protected:
		Vector<SPtr<Component>> m_components;
	};

	template<class T>
	void Entity::removeComponent() {
		auto*  t = m_components.begin();
		for (; t < m_components.end(); t++) 
		{
			if (sge_cast<T*>(*t))
			{
				m_components.erase_unsorted(t);
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
//		if (!T::s_allowMultiple()) 
//		{
//			if ( auto* p = getComponent<T>().ptr()) {
//				throw SGE_ERROR("adding component : existing type");
//				return p;
//			}
//		}
		SPtr<T> p = T::System::instance()->createComponent<T>();
		m_components.emplace_back(p);

		return p.ptr();
	}






}
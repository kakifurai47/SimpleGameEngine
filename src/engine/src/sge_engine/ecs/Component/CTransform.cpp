#include "CTransform.h"

namespace sge
{
	TransformSystem* TransformSystem::instance() {
		static TransformSystem sys;
		return &sys;
	}

	Component* TransformSystem::onCreateComponent() 
	{
		auto* c  =  new CTransform();
		m_components.emplace_back(c);
		return c;
	}

	void TransformSystem::onDestroyComponent(Component* c)
	{
		m_components.erase_first_unsorted( static_cast<CTransform*>( c ) );
	}

	Span<FieldInfo> CTransform::TI_Base::s_fields()
	{
		static FieldInfo fi[] = {
			SGE_FIELD_INFO_CALLBACK(This, Position, m_position),
			SGE_FIELD_INFO_CALLBACK(This, Scale,	m_scale),
		};
		return fi;
	}
	
	
	SGE_GET_TYPE_IMPL(CTransform)


	void CTransform::setParent(CTransform* parent)
	{
		if (m_parent == parent) {
			return;
		}

		if (m_parent) 
		{
			for (auto& c : m_parent->m_childs) {
				if (c == this) {
					m_parent->m_childs.erase(&c);
					break;
				}
			}
		}

		m_parent = parent;

		if (m_parent) {
			m_parent->m_childs.emplace_back(this);
		}
	}
}
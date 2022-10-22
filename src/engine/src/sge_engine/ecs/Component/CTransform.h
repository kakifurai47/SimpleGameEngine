#pragma once

#include <sge_engine/ecs/ComponentSystem.h>

namespace sge
{

	class CTransform;

	class TransformSystem : public ComponentSystem
	{
	public:

		static TransformSystem* instance();

		Span<CTransform*> components() { return m_components; }

	protected:
		virtual Component* onCreateComponent()				override;
		virtual void	   onDestroyComponent(Component* c) override;

		Vector<CTransform*> m_components;
	};

	class CTransform : public Component_Impl<TransformSystem, false>
	{
	public:
		SGE_TYPE_INFO(CTransform, Component);

		virtual ~CTransform() { setParent(nullptr); }

		bool			hasParent() const { return m_parent != nullptr;  }
		CTransform*		   parent()		  { return m_parent; }
		Span<CTransform*>  childs()		  { return m_childs; }
		
		void setParent	(CTransform* parent_);
		void addChild	(CTransform*  child_) { if (child_) child_->setParent(this);	}
		void removeChild(CTransform*  child_) { if (child_) child_->setParent(nullptr); }

		void		  setPosition(const Vec3f&    newPos) { _set(m_position, newPos); }
		void		  setRotation(const Quat4f&   newRot) { _set(m_rotation, newRot); }
		void		  setScale   (const Vec3f&  newScale) { _set(m_scale,  newScale); }

		const Vec3f&  getPosition() const { return m_position; }
		const Quat4f& getRotation() const { return m_rotation; }
		const Vec3f&  getScale   () const { return m_scale;	   }

	private:

		template<class T>
		void _set(T& oriVal, const T& newVal)
		{
			if (oriVal == newVal) return;
			m_isDirty = true; 
			oriVal  = newVal;
		}

	private:

		bool   m_isDirty  =  true;

		Vec3f  m_position =  Vec3f ::s_zero	   ();
		Quat4f m_rotation =  Quat4f::s_identity();
		Vec3f  m_scale	  =  Vec3f ::s_one	   ();

		Mat4f  m_modelMat =  Mat4f::s_identity ();

		CTransform*			m_parent = nullptr;
		Vector<CTransform*>	m_childs;
	};






}
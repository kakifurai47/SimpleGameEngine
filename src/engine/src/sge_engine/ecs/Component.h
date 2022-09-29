#pragma once 

//#include <sge_core/reflection/sge_reflection.h>

namespace sge
{
	class ComponentSystem;

	class Component : public Object
	{	
	public:
		SGE_TYPE_INFO(Component, Object);

		virtual ~Component() = default;
		virtual ComponentSystem* system() = 0;
		virtual bool	  allowMultiple() = 0;
	};

	template<class SYSTEM, bool allow_multiple>
	class Component_Impl : public Component
	{
	public:
		using System = SYSTEM;

		static constexpr bool s_allowMultiple() { return allow_multiple; }

		virtual ~Component_Impl() {
			System::instance()->destroyComponent(this);
		}

		virtual bool	  allowMultiple() override { return s_allowMultiple();  }

		virtual ComponentSystem* system() override 
		{
			static_assert(std::is_base_of<ComponentSystem, System>::value);
			return System::instance(); 
		}
	};
}
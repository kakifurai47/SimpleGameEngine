#pragma once

#include <sge_engine/ecs/ComponentSystem.h>
#include <sge_render/mesh/RenderMesh.h>
#include <sge_render/shader/Material.h>

namespace sge
{
	class RenderRequest;
	class RenderMesh;
	class Material;

	class CRenderer;

	class RenderSystem : public ComponentSystem
	{
	public:
		static RenderSystem* instance();

		Span<CRenderer*> components() { return m_components; }

		template<class T> SPtr<T> newComponent();

		void render(RenderRequest& request);

	protected:
		virtual void onDestroyComponent(Component* c) override;

		Vector<CRenderer*> m_components;
	};

	template<class T>
	SPtr<T> RenderSystem::newComponent() {
		static_assert(std::is_base_of_v<CRenderer, T>);
		auto* c = new T();
		m_components.emplace_back(c);
		return c;
	}

	class CRenderer : public Component_Impl<RenderSystem, false>
	{
	public:
		SGE_TYPE_INFO(CRenderer, Component);

		void setMaterial(Material* mat) { m_material = mat; }


	friend class RenderSystem;
	protected:

		SPtr<Material>	 m_material;
		SPtr<RenderMesh> m_mesh;
	};
}


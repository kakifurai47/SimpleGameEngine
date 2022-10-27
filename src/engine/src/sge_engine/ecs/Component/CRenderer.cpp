#include "CRenderer.h"

#include <sge_render/command/RenderRequest.h>

namespace sge
{
	RenderSystem* RenderSystem::instance()
	{
		static RenderSystem system;
		return &system;
	}

	void RenderSystem::onDestroyComponent(Component* c) {
		SGE_DUMP_VAR("OnDestroy");
		m_components.erase_first_unsorted(static_cast<CRenderer*>(c));
	}

	void RenderSystem::render(RenderRequest& request) {
		for (auto* c : m_components) 
		{
			if (!c->m_mesh || !c->m_material) continue;
			request.drawMesh(SGE_LOC, *c->m_mesh, c->m_material);
		}
	}


	Span<FieldInfo> CRenderer::TI_Base::s_fields()
	{
		return {};
	}

	SGE_GET_TYPE_IMPL(CRenderer)
//----
	

}
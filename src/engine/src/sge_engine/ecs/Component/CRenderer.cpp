#include "CRenderer.h"

#include <sge_render/command/RenderRequest.h>
#include <sge_engine/ecs/Entity.h>
#include <sge_core/math/Frustum3.h>

namespace sge
{
	RenderSystem* RenderSystem::instance()
	{
		static RenderSystem system;
		return &system;
	}

	void RenderSystem::onDestroyComponent(Component* c) 
	{
		m_components.erase_first_unsorted(static_cast<CRenderer*>(c));
	}

	void RenderSystem::render(RenderRequest& request) 
	{
		Frustum3f::Planes planes;

		auto vp = request.proj * request.view;

		m_culledEntities.clear();

		for (auto* c : m_components) 
		{
			if (!c->m_mesh || !c->m_material) continue;

			auto* t = c->entity()->transform();
			request.model =   t->getModelMat();
			
			auto mvp = vp * request.model;

			planes.reset(mvp);

			RenderMesh& mesh = *c-> m_mesh;
			auto&	    aabb = mesh.AABB();

			if (Frustum3f::s_isInBound(planes, aabb))
			{
				request.drawMesh(SGE_LOC, *c->m_mesh, c->m_material);
			}
			else
			{
				m_culledEntities.emplace_back(c->entity());
			}
		}
	}


	Span<FieldInfo> CRenderer::TI_Base::s_fields()
	{
		return {};
	}

	SGE_GET_TYPE_IMPL(CRenderer)
//----
	

}
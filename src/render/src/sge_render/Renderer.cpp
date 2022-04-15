
#include "Renderer.h"
#include "sge_render/base/Renderer_Base.h"

#include "sge_render/d3d11/Renderer_DirectX11.h"


namespace sge
{
	void Renderer::create(NativeUIWindow* window, RenderAPI api)
	{
		if (!window) return;

		resetAPI(api);
		if (m_baseRenderer) {
			m_baseRenderer->init(window);
		}		
	}

	void Renderer::run()
	{
		if (!m_baseRenderer) return;
		m_baseRenderer->render();
	}

	void Renderer::clear()
	{
		if (m_baseRenderer) {
			m_baseRenderer->destroy();
			m_baseRenderer = nullptr;
		}
	}

	void Renderer::resetAPI(RenderAPI api)
	{
		m_API = api;
		clear();

		switch (api) {
		case RenderAPI::dx11: m_baseRenderer = eastl::make_unique<Renderer_DirectX11>();  break;
		default: m_baseRenderer = nullptr; break;
		}
	}

}
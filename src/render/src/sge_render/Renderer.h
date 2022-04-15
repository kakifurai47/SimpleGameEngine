#pragma once

#include "sge_render/base/sge_render_base.h"
#include "sge_render/base/Renderer_Base.h"

namespace sge
{
	class Renderer_Base;
	class NativeUIWindow;

	class Renderer : NonCopyable
	{
		using BaseRendererPtr = UPtr<Renderer_Base>;
	public:
		~Renderer() = default;

		void create(NativeUIWindow* window, RenderAPI api);
		void run();
		void clear();

		void drawTriangle() { if (m_baseRenderer) m_baseRenderer->drawTriangle(); }

	private:
		void resetAPI(RenderAPI api);

	private:
		RenderAPI		m_API;
		BaseRendererPtr m_baseRenderer;
	};
}
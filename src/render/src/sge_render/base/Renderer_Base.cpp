
#include "Renderer_Base.h"
#include "sge_render/d3d11/Renderer_DirectX11.h"

namespace sge
{
	void Renderer_Base::init(NativeUIWindow* window)
	{
		if (!window) return;
		onInit(window);
	}

	void Renderer_Base::render()
	{
		onRenderUpdate();
	}

	void Renderer_Base::destroy()
	{
		onDestroy();
	}

}



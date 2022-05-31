
#include "Renderer.h"
#include "../dx11/Renderer_DX11.h"

namespace sge
{
	Renderer_CreateDesc::Renderer_CreateDesc() {
		type = Render_ApiType::DX11;
	}

	sge::Renderer* Renderer::s_current = nullptr;

	Renderer* Renderer::create(CreateDesc& desc) {
		Renderer* rdr = nullptr;

		switch (desc.type) {
		#if SGE_RENDER_COMP_DX11
			case Render_ApiType::DX11:  rdr = new Renderer_DX11(desc); break;
		#endif	
			default: throw SGE_ERROR("Unsupported platform");
		}
		if (s_current) {
			s_current->destroy();
		}
		s_current = rdr;
		return rdr;
	}

	void Renderer::destroy() {
		onDestory();
	}

}
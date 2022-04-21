
#include "Renderer.h"
#include "../d3d11/Renderer_DX11.h"

namespace sge
{
	Renderer_CreateDesc::Renderer_CreateDesc() {
		type = Render_ApiType::DX11;
	}

	sge::Renderer* Renderer::s_current = nullptr;

	Renderer* Renderer::create(CreateDesc& desc) {
		switch (desc.type)  
		{
		#if SGE_RENDER_COMP_DX11
			case Render_ApiType::DX11:  return new Renderer_DX11(desc);
		#endif	
		}
		throw SGE_ERROR("Unsupported platform");
	}

	

}
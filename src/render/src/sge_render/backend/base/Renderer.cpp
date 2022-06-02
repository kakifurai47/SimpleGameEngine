
#include "Renderer.h"
#include "../dx11/Renderer_DX11.h"

namespace sge
{
	Renderer* Renderer::s_current = nullptr;

	Renderer_CreateDesc::Renderer_CreateDesc()
		: multithread(false) {
#if SGE_OS_WINDOWS
		apiType = Render_ApiType::DX11;
#endif
	}


	Renderer* Renderer::create(CreateDesc& desc) {
		Renderer* p = nullptr;
		switch (desc.apiType) {
			case Render_ApiType::DX11: p = new Renderer_DX11(desc); break;
			default: throw SGE_ERROR("Unsupported platform");
		}
		return p;
	}

	Renderer::Renderer() {
		SGE_ASSERT(s_current == nullptr);
		s_current = this;
	}

	Renderer::~Renderer() {
		SGE_ASSERT(s_current = this);
		s_current = nullptr;
	}
}
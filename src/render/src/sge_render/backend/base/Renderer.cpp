
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

		p->stockTextures.create();


		return p;
	}

	Renderer::Renderer() {
		SGE_ASSERT(s_current == nullptr);
		s_current = this;
	}

	Renderer::~Renderer() {
		SGE_ASSERT(m_shaders.size() == 0);
		SGE_ASSERT(s_current == this);
		s_current = nullptr;
	}

	SPtr<Shader> Renderer::createShader(StrView filename)
	{
		auto id = File::getId(filename);

		auto it = m_shaders.find(id);
		if (it != m_shaders.end()) {
			return it->second;
		}

		auto s = onCreateShader(filename);
		m_shaders[id] = s;
		return s;
	}

}
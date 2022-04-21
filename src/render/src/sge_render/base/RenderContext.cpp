#include "RenderContext.h"
#include "Renderer.h"

namespace sge {

	RenderContext* RenderContext::create(CreateDesc& desc) {
		auto* rdr = Renderer::current();
		if (!rdr) return nullptr;
		return rdr->onCreateRenderContext(desc);
	}

}


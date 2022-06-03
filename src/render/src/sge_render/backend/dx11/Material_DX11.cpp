#include "Renderer_DX11.h"
#include "Material_DX11.h"

namespace sge {
	void Material_DX11::onSetShader(StrView shaderPath) {
		m_shader = ShaderManager_DX11::current()->find(shaderPath);
		auto& info = m_shader->info();

		auto* renderer = Renderer_DX11::current();		
		RenderGpuBuffer_CreateDesc desc;
		desc.type = RenderGpuBufferType::Const;

		m_constBuf.reserve(info.constBufInfos.size());
		for (const auto& c : info.constBufInfos) {
			auto& buf = m_constBuf.emplace_back();
			desc.bufferSize = c.size;
			buf = renderer->createGpuBuffer(desc);
		}
	}
}
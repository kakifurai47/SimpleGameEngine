#include "Material.h"
#include <sge_render/backend/base/Renderer.h>

namespace sge {

	MaterialStage::MaterialStage(ShaderStage* shaderStage)
		: m_shadStage(shaderStage)
	{
		auto& cbInfos = shaderStage->info()->constBuffers;
		m_constBufs.clear();

		size_t cbCount = cbInfos.size();
		m_constBufs.reserve(cbCount);

		for (auto& i : cbInfos) {
			auto& cb = m_constBufs.emplace_back();
			cb.create(i);
		}
	}

	void MaterialStage::ConstBuffer::create(const Info& info) {
		m_info = &info;
		m_dirty = true;

		cpuBuffer.resize(info.dataSize);

		RenderGpuBuffer::CreateDesc desc;
		desc.type		= RenderGpuBufferType::Const;
		desc.bufferSize = info.dataSize;

		gpuBuffer = Renderer::current()->createGpuBuffer(desc);
	}

	void Material::setShader(Shader* shad) {
		if (shad == m_shader) {
			return;
		}
		m_matPasses.clear();
		if (!shad) return;

		auto& passes = shad->passes();
		m_matPasses.reserve(passes.size());

		for (auto& pass : passes) {
			auto* mp = onCreatePass(pass.get());
			m_matPasses.emplace_back(mp);
		}
	}

}

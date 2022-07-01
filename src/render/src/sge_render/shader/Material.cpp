#include "Material.h"
#include <sge_render/backend/base/Renderer.h>

namespace sge {

	MaterialStage::MaterialStage(MaterialStage&& other)
		: m_shadStage(std::move(other.m_shadStage))
		, m_constBufs(std::move(other.m_constBufs))
	{
	}

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

	void MaterialStage::setParam(StrView name, const Material_ValuePtr& valPtr) {
		if (!m_shadStage) return;
		for (auto& cb : m_constBufs) { cb.setParam(name, valPtr); }
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

	void MaterialStage::ConstBuffer::uploadToGpu() {
		if (!m_dirty) return;
		m_dirty = false;

		if (!gpuBuffer) return;
		gpuBuffer->uploadToGpu(cpuBuffer);
	}

	template<class T>
	void MaterialStage::ConstBuffer::_setParam(const VarInfo* varInfo, const T& value) {
		auto inputType  = RenderFormatTypeUtil::get<T>();
		if  (inputType != varInfo->formatType) {
			throw SGE_ERROR("set param type mismatch: name: [{}], input: [{}], dst: [{}]",
							varInfo->name, inputType, varInfo->formatType
			);
		}
		auto end = varInfo->offset + sizeof(T);
		if  (end > cpuBuffer.size())
			throw SGE_ERROR("const buf : setParam out of range");

		auto* dst = cpuBuffer.data() + varInfo->offset;		
		*reinterpret_cast<T*>(dst) = value;
		m_dirty = true;
	}

	void MaterialStage::ConstBuffer::setParam(StrView name, const Material_ValuePtr& valPtr)
	{
		auto* varInfo = m_info->findVariable(name);
		if (!varInfo) return;

		switch (valPtr.type) {
			using Type = RenderFormatType;
			case  Type::Float32x1:	 { _setParam(varInfo, valPtr.asValue<float  >()); break; }
			case  Type::Float32x2:	 { _setParam(varInfo, valPtr.asValue<Tuple2f>()); break; }
			case  Type::Float32x3:	 { _setParam(varInfo, valPtr.asValue<Tuple3f>()); break; }
			case  Type::Float32x4:	 { _setParam(varInfo, valPtr.asValue<Tuple4f>()); break; }
			case  Type::Float32_4x4: { _setParam(varInfo, valPtr.asValue<Mat4f>	 ()); break; }
			default: throw SGE_ERROR("setParam : unsupported type: {}", valPtr.type);
		}
	}

	MaterialPass::MaterialPass(Material* material, ShaderPass* shadPass,
							VertexStage& vtxStage, PixelStage& pxlStage)
		: m_material(material), m_shadPass(shadPass)
		, m_vertexStage(&vtxStage)
		, m_pixelStage (&pxlStage)
	{
	}

	void MaterialPass::setParam(StrView name, const Material_ValuePtr& valPtr) {
		if (m_vertexStage) m_vertexStage->setParam(name, valPtr);
		if ( m_pixelStage)  m_pixelStage->setParam(name, valPtr);
	}

	void Material::setShader(Shader* shad) {
		if (m_shader == shad) {
			return;
		}
		m_shader = shad;
		m_matPasses.clear();
		if (!shad) return;
		onSetShader(shad->passes());
		onResetPasses (m_matPasses);
	}

	void Material::setParam(StrView name, const Material_ValuePtr& valPtr) {
		for (auto* p : m_matPasses) { p->setParam(name, valPtr); }
	}

}

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
		auto* info = shaderStage->info();

		{
			auto& cbInfos	= info->constBuffers;
			size_t cbCount	= cbInfos.size();

			m_constBufs.reserve(cbCount);
			for (auto& i : cbInfos) {
				auto& cb = m_constBufs.emplace_back();
				cb.create(i);
			}
		}
		{
			auto& texInfos	= info->textures;
			size_t texCount = texInfos.size();

			m_texParams.reserve(texCount);
			for (auto& i : texInfos) {				
				auto& tex = m_texParams.emplace_back();
				tex.create(i);
			}
		}
	}

	void MaterialStage::setParam(StrView name, const Material_ValuePtr& valPtr) {
		if (!m_shadStage) return;
		for (auto& cb : m_constBufs) { cb.setParam(name, valPtr); }
	}

	void MaterialStage::setTexParam(StrView name, Texture* tex) {
		if (!m_shadStage)   return;
		SGE_ASSERT(tex != nullptr);

		for (auto& p : m_texParams) {
			if (p.name().compare(name) == 0) {
				p.setTexParam(tex);
				break;
			}
		}
	}

	Texture* MaterialStage::TexParam::getUpdatedTexture() {
		if (!m_tex) {
			//TODO : handle error here
			throw SGE_ERROR("missing texture");
			//SGE_LOG("missing texture");
		}
		//TODO : update texture
		return m_tex;
	}

	void MaterialStage::TexParam::setTexParam(Texture* tex)
	{
		if (m_info->formatType != tex->format()) {
			m_tex.reset(nullptr);
			throw SGE_ERROR("invalid texture type");
		}
		m_tex = tex;
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
			case Type::Float32x1:	 _setParam(varInfo, valPtr.asValue<float  >  ()); break; 
			case Type::Float32x2:	 _setParam(varInfo, valPtr.asValue<Tuple2f>  ()); break; 
			case Type::Float32x3:	 _setParam(varInfo, valPtr.asValue<Tuple3f>  ()); break; 
			case Type::Float32x4:	 _setParam(varInfo, valPtr.asValue<Tuple4f>  ()); break; 
			case Type::Float32_4x4:  _setParam(varInfo, valPtr.asValue<Mat4f>    ()); break;
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

	void MaterialPass::setTexParam(StrView name, Texture* tex) {
		if (m_vertexStage) m_vertexStage->setTexParam(name, tex);
		if ( m_pixelStage)  m_pixelStage->setTexParam(name, tex);
	}

	Span<FieldInfo> Material::TI_Base::s_fields()
	{
//		TODO : add SPtr<T> reflection
//		static FieldInfo fi[] = {
//			{"Shader", &Material::m_shader },
//		};
//		return fi;
		return {};
	}

	SGE_GET_TYPE_IMPL(Material)



	Material::Pass* Material::getPass(size_t index) {
		if (index >= m_matPasses.size()) {
			SGE_ASSERT(false);
			return nullptr;
		}
		return m_matPasses[index];
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

	void Material::_setParam(StrView name, const Material_ValuePtr& valPtr) {
		for (auto* p : m_matPasses) { p->setParam(name, valPtr); }
	}

	void Material::_setTexParam(StrView name, Texture* tex) {
		for (auto* p : m_matPasses) { p->setTexParam(name, tex); }
	}

}

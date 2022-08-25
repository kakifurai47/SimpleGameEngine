#include "Shader_DX11.h"
#include "Renderer_DX11.h"
#include "RenderContext_DX11.h"

#include <sge_core/serializer/json/JsonUtil.h>

namespace sge {

	template<>
	void ShaderStage_DX11_Impl<ShaderVertexStageBase_DX11>::bind(RenderContext_DX11* ctx) {
		if (!m_d3dVertexShader) throw SGE_ERROR("dx vertex shader is null");
		auto* dc = ctx->renderer()->d3dDeviceContext();
		dc->VSSetShader(m_d3dVertexShader, 0, 0);
	}

	template<>
	void ShaderStage_DX11_Impl<ShaderPixelStageBase_DX11>::bind(RenderContext_DX11* ctx) {
		if (!m_d3dPixelShader) throw SGE_ERROR("dx pixel shader is null");
		auto* dc = ctx->renderer()->d3dDeviceContext();
		dc->PSSetShader(m_d3dPixelShader, 0, 0);
	}

	template<>
	void ShaderStage_DX11_Impl<ShaderVertexStageBase_DX11>::_createShader(DX11_ID3DDevice* dev) {
		auto hr = dev->CreateVertexShader(Util::toBufferPtr(m_byteCode), m_byteCode.size(), nullptr, m_d3dVertexShader.ptrForInit());
		DX11Util::throwIfError(hr);
	}

	template<>
	void ShaderStage_DX11_Impl<ShaderPixelStageBase_DX11>::_createShader(DX11_ID3DDevice* dev) {
		auto hr = dev->CreatePixelShader(Util::toBufferPtr(m_byteCode), m_byteCode.size(), nullptr, m_d3dPixelShader.ptrForInit());
		DX11Util::throwIfError(hr);
	}

	template<class STAGE_BASE>
	void ShaderStage_DX11_Impl<STAGE_BASE>::load(StrView passPath, DX11_ID3DDevice* dev) {
		auto profile = DX11Util::getDxStageProfile( kStageMask() );
	
		auto byteCodeFilename = Fmt("{}/{}.bin", passPath, profile);
		File::readFile(byteCodeFilename,  m_byteCode);

		auto& infoFilename = byteCodeFilename.append(".json");	
		JsonUtil::readFile(infoFilename, m_stageInfo);

		_createShader(dev);
	}

	template ShaderStage_DX11_Impl<ShaderVertexStageBase_DX11>;
	template ShaderStage_DX11_Impl< ShaderPixelStageBase_DX11>;

	ShaderPass_DX11::ShaderPass_DX11(StrView passPath, Info& info)
		:Base(&info, m_dx11VertexStage, m_dx11PixelStage)
	{
		auto* dev = Renderer_DX11::current()->d3dDevice();

		if (info.vsFunc.size()) { m_dx11VertexStage.load(passPath, dev); }
		if (info.psFunc.size()) {  m_dx11PixelStage.load(passPath, dev); }
	}

	ShaderPass_DX11::ShaderPass_DX11(ShaderPass_DX11&& other)
		: Base(other.m_passInfo, m_dx11VertexStage, m_dx11PixelStage)
		, m_dx11VertexStage(std::move(other.m_dx11VertexStage))
		,  m_dx11PixelStage(std::move( other.m_dx11PixelStage))
	{
	}

	void Shader_DX11::onCreate(StrView compiledPath) {
		size_t n = m_info.passes.size();		    
		m_dx11ShadPasses.reserve(n);

		for (size_t i = 0; i < n; i++) {
			auto  passPath  = Fmt("{}/dx11/pass{}", compiledPath, i);
			m_dx11ShadPasses.emplace_back(passPath, m_info.passes[i]);
		}
	}

	void Shader_DX11::onResetPasses(Vector<Pass*, 1>& outPasses) {
		outPasses.clear();
		outPasses.reserve(m_dx11ShadPasses.size());
		for (auto& p : m_dx11ShadPasses) {
			outPasses.emplace_back(&p);
		}
	}

	void Shader_DX11::onDestroy() {
		ShaderManager_DX11::current()->erase(m_key);
	}
}

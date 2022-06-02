#include "Shader_DX11.h"
#include "Renderer_DX11.h"

namespace sge {
	
	void Shader_DX11::onCreate(StrView compiledPath) {
		size_t passIndex = 0;
		for (const auto& p : m_info.passes) {
			auto passPath   = Fmt("{}/dx11/pass{}",compiledPath, passIndex);
	
			if (p.vsFunc.size()) _onCreateShad(passPath, ShaderStage::Vertex, p.vsFunc);
			if (p.psFunc.size()) _onCreateShad(passPath, ShaderStage::Pixel,  p.psFunc);
			passIndex++;
		}
	}
	
	void Shader_DX11::_onCreateShad(StrView passPath, ShaderStage stage, StrView funcName) {
		auto profile  =  DX11Util::getDxStageProfile(stage);
		auto filename = Fmt("{}/{}.bin", passPath, profile);
	
		MemMapFile memmap;
		memmap.open(filename);
		auto byteCode = memmap.span();
	
		auto* dev = Renderer_DX11::current()->d3dDevice();

		HRESULT hr{};
		switch (stage) {
		case ShaderStage::Vertex: {
			hr = dev->CreateVertexShader(DX11Util::toBufferPtr(byteCode), byteCode.size(),
										 nullptr, &m_d3dVtxShads.emplace_back());
		} break;
		case ShaderStage::Pixel: {
			hr = dev->CreatePixelShader(DX11Util::toBufferPtr(byteCode), byteCode.size(),
										nullptr, &m_d3dPxlShads.emplace_back());
		}break;
			default: throw SGE_ERROR("Unhandled ShaderStage");
		}
		DX11Util::throwIfError(hr);
	}
}

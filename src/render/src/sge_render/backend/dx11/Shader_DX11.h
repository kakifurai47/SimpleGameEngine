#pragma once

#include "../../shader/Shader.h"
#include "Render_DX11_Common.h"

namespace sge {

	class Shader_DX11 : public Shader {
	public:	
		virtual void onCreate(StrView compiledPath) override;

		Span<DX11_ID3DVertexShader*> d3dVtxShads() { return m_d3dVtxShads; }
		Span<DX11_ID3DPixelShader*>	 d3dPxlShads() { return m_d3dPxlShads; }
	
	private:

		Vector_<DX11_ID3DVertexShader*, 1> m_d3dVtxShads;
		Vector_<DX11_ID3DPixelShader*,  1> m_d3dPxlShads;

		void _onCreateShad(StrView passPath, ShaderStage stage, StrView funcName);
	};
}

#pragma once

#include "../../shader/Shader.h"
#include "Render_DX11_Common.h"

namespace sge {

	class Shader_DX11 : public Shader {
	public:	
		virtual void onCreate(StrView compiledPath) override;

		Span<DX11_ID3DVertexShader*> vtxPasses() { return m_vtxShads; }
		Span<DX11_ID3DPixelShader*>	 pxlPasses() { return m_pxlShads; }
	
	private:

		Vector_<DX11_ID3DVertexShader*, 1> m_vtxShads;
		Vector_<DX11_ID3DPixelShader*,  1> m_pxlShads;

		void _onCreateShad(StrView passPath, ShaderStage stage, StrView funcName);
	};
}

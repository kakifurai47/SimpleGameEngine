#include "Material_DX11.h"
#include "Shader_DX11.h"

namespace sge {
	void Material_DX11::onSetShader(StrView shaderPath) {
		m_shader = ShaderManager_DX11::current()->find(shaderPath);


	}
}
#include "ShaderManager.h"
#include "../backend/dx11/Shader_DX11.h"

namespace sge {

	template<class SHADER>
	ShaderManager< SHADER>* ShaderManager<SHADER>::current() {
		static This s;
		return &s;
	}

	template<class SHADER>
	SHADER* ShaderManager<SHADER>::find(StrView filename) {
		auto id = File::getId(filename);

		auto it = m_shaders.find(id);
		if (it != m_shaders.end()) {
			return &it->second;
		}
		auto*  shad = &m_shaders[id];
		shad->create(filename);
		return shad;
	}

	template ShaderManager<Shader_DX11>;
}
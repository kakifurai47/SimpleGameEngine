#pragma once

#include "../../Shader/Material.h"
#include "Shader_DX11.h"

namespace sge {
	class Material_DX11 : public Material {
		void onSetShader(StrView shaderPath) override;
	};


}
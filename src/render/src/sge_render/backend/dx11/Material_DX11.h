#pragma once

#include "../../Shader/Material.h"

namespace sge {

	class Material_DX11 : public Material {

		
		void onSetShader(StrView shaderPath) override;

	};


}
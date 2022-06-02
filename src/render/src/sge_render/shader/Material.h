#pragma once
#include "Shader.h"
#include "ShaderManager.h"

namespace sge {
	class Material : public Object {
	public:
		virtual ~Material() = default;

		Shader* shader() const { return constCast(m_shader); }

		void setShader(StrView shaderPath) { onSetShader(shaderPath); }

		template<class TYPE>
		void set(StrView propName, TYPE type) {}

	protected:
		virtual void onSetShader(StrView shaderPath) {};
		const Shader* m_shader = nullptr;
	};
}

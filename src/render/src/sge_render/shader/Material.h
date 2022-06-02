#pragma once
#include "Shader.h"
#include "ShaderManager.h"

namespace sge {
	class Material : public Object {
	public:
		virtual ~Material() = default;

		void setShader(StrView shaderPath) { onSetShader(shaderPath); }
		void create()					   { onCreate(); }

		Shader* shader() const { return constCast(m_shader); }
		
		template<class TYPE>
		void set(StrView propName, TYPE type) {}

	protected:
		virtual void onSetShader(StrView shaderPath) {};
		virtual void onCreate	() {};
	
		const Shader* m_shader = nullptr;
	};
}

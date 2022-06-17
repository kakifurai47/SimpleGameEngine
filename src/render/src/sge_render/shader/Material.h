#pragma once
#include "Shader.h"
#include "ShaderManager.h"
#include "../buffer/RenderGpuBuffer.h"

namespace sge {
	class Material : public Object {
	public:
		virtual ~Material() = default;

		Shader* shader() const { return constCast(m_shader); }

		void setShader(StrView shaderPath) { onSetShader(shaderPath); }

		Span<SPtr<RenderGpuBuffer>> buffers() { return m_constBuf; }

		template<class TYPE>
		void set(StrView propName, TYPE type) {
			const auto& info = m_shader->info();
			for (const auto& p : info.props) {
				//if (!p.isDefined) continue;//TODO : type validation //FIX
				//if (p.name == propName) {//use id instead of string compare
				//	ByteSpan data{ reinterpret_cast<u8*>(&type), sizeof(TYPE) };
				//	m_constBuf[p.slotIdx]->uploadToGpu(data, p.offset);
				//	break;
				//}
			}
		}

	protected:
		virtual void onSetShader(StrView shaderPath) {};
		Vector_<SPtr<RenderGpuBuffer>, 1> m_constBuf;
		const Shader* m_shader = nullptr;
	};
}

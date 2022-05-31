#pragma once
#include "Shader.h"

namespace sge {

	class Material : public NonCopyable {
	public:

		//Material(Shader* shader_) {
		//	shader = shader_;
		//}

		template<class TYPE>
		void set(StrView propName, TYPE type) {

		}



		const Shader* shader = nullptr;
	};


}

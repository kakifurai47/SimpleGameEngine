#pragma once
#include <sge_core/base/sge_base.h>

namespace sge {
	enum class ShaderPropType : u8 {
		None,
		Int,
		Float,
		Vec2f,
		Vec3f,
		Vec4f,
		Color4f,
		Matrix,
		Double,
	};

#define ShaderPropType_ENUM_LIST(E) \
		E(Int) \
		E(Float) \
		E(Vec2f) \
		E(Vec3f) \
		E(Vec4f) \
		E(Color4f) \
		E(Matrix) \
		E(Double) \
//------

	SGE_ENUM_STR_UTIL(ShaderPropType)

	class ShaderInfo : public NonCopyable {
	public:
		struct Attr {
			String tag;
			String defaultValue;
		};

		struct Prop {
			ShaderPropType propType = ShaderPropType::None;
			Attr		   attribute;
			String		   name;
			String		   defaultValue;
		};

		struct Pass {
			String name;
			String vsFunc;
			String psFunc;
		};

		Vector_<Prop, 8>	props;
		Vector_<Pass, 1>	passes;
	};

	class ShaderStageInfo : public NonCopyable {
	public:




	};





	template<class SE> inline
	void onSerDes(SE& se, ShaderInfo& info) {
		SGE_SERDES_IO(se, info, props);
		SGE_SERDES_IO(se, info, passes);
	}

	template<class SE> inline
	void onSerDes(SE& se, ShaderInfo::Attr& attr) {
		SGE_SERDES_IO(se, attr, tag);
		SGE_SERDES_IO(se, attr, defaultValue);
	}

	template<class SE> inline
	void onSerDes(SE& se, ShaderInfo::Prop& prop) {
		SGE_SERDES_IO(se, prop, propType);
		SGE_SERDES_IO(se, prop, attribute);
		SGE_SERDES_IO(se, prop, name);
		SGE_SERDES_IO(se, prop, defaultValue);
	}

	template<class SE> inline
	void onSerDes(SE& se, ShaderInfo::Pass& pass) {
		SGE_SERDES_IO(se, pass, name);
		SGE_SERDES_IO(se, pass, vsFunc);
		SGE_SERDES_IO(se, pass, psFunc);
	}


}
#pragma once
#include <sge_core/base/sge_base.h>
#include <sge_render/vertex/Vertex.h>
#include <sge_core/math/Vec.h>


namespace sge {
	enum class ShaderStageMask {
		None,
		Vertex  = 1 << 0,
		Pixel	= 1 << 1,
	};

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

	struct ShaderPropTypeUtil {
		ShaderPropTypeUtil() = delete;
		
		using Type = ShaderPropType;

		template<class T> static constexpr Type get();

		template<> static constexpr	Type get<i32>()		{ return Type::Int;		}
		template<> static constexpr	Type get<f32>()		{ return Type::Float;	}
		template<> static constexpr	Type get<Vec2f>()	{ return Type::Vec2f;	}
	//	template<> static constexpr	Type get<Vec3f>()	{ return Type::Vec3f;	}
	//	template<> static constexpr	Type get<Vec4f>()	{ return Type::Vec4f;	}
		template<> static constexpr	Type get<Color4f>()	{ return Type::Color4f; }
	};

	class ShaderInfo {
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

	class ShaderStageInfo {
	public:
		using FormatType = RenderFormatType;
		using Semantic	 = VertexSemantic;

		String	filename;
		String	profile;
		ShaderStageMask stage = ShaderStageMask::None;
		
		struct Param {
			String	   name;
			FormatType formatType;
			i16 bindPoint = 0;
			i16 bindCount = 0;
		};
		
		struct Input {
			String		name;
			Semantic	semantic   = Semantic::None;
			FormatType	formatType = FormatType::None;
		};

		struct Variable {
			String		name;
			FormatType	formatType = FormatType::None;
			size_t		offset	   = 0;
			bool		rowMajor   = true;
		};

		struct ConstBuffer {
			String	 name;
			i16		 bindPoint = 0;
			i16		 bindCount = 0;
			size_t	 dataSize  = 0;

			Vector_<Variable, 4> variables;

			const Variable* findVariable(StrView propName) const {
				for (auto& v : variables) {
					if (v.name == propName) return &v;
				}
				return nullptr;
			}
		};

		struct Texture {
			String		name;
			i16			bindPoint  = 0;
			i16			bindCount  = 0;
			FormatType	formatType = FormatType::None;
		};

		struct Sampler {
			String		name;
			i16			bindPoint	= 0;
			i16			bindCount	= 0;
			FormatType	formatType  = FormatType::None;
		};

		Vector_<Input, 8>		inputs;
		Vector_<Param, 8>		params;
		Vector_<ConstBuffer, 8>	constBuffers;

		Vector_<Texture, 8>		textures;
		Vector_<Sampler, 8>		samplers;
	};

	template<class SE> inline
	void onSerDes(SE& se, ShaderStageInfo& stageInfo) {
		SGE_SERDES_IO(se, stageInfo, profile);
		SGE_SERDES_IO(se, stageInfo, inputs);
		SGE_SERDES_IO(se, stageInfo, params);
		SGE_SERDES_IO(se, stageInfo, constBuffers);
		SGE_SERDES_IO(se, stageInfo, textures);
		SGE_SERDES_IO(se, stageInfo, samplers);
	}

	template<class SE> inline
	void onSerDes(SE& se, ShaderStageInfo::Param& param) {
		SGE_SERDES_IO(se, param, name);
		SGE_SERDES_IO(se, param, formatType);
		SGE_SERDES_IO(se, param, bindPoint);
		SGE_SERDES_IO(se, param, bindCount);
	}

	template<class SE> inline
	void onSerDes(SE& se, ShaderStageInfo::Input& input) {
		SGE_SERDES_IO(se, input, name);
		SGE_SERDES_IO(se, input, semantic);
		SGE_SERDES_IO(se, input, formatType);
	}

	template<class SE> inline
	void onSerDes(SE& se, ShaderStageInfo::Variable& variable) {
		SGE_SERDES_IO(se, variable, name);
		SGE_SERDES_IO(se, variable, formatType);
		SGE_SERDES_IO(se, variable, offset);
		SGE_SERDES_IO(se, variable, rowMajor);
	}

	template<class SE> inline
	void onSerDes(SE& se, ShaderStageInfo::ConstBuffer& constBuf) {
		SGE_SERDES_IO(se, constBuf, name);
		SGE_SERDES_IO(se, constBuf, bindPoint);
		SGE_SERDES_IO(se, constBuf, bindCount);
		SGE_SERDES_IO(se, constBuf, dataSize);
		SGE_SERDES_IO(se, constBuf, variables);
	}

	template<class SE> inline
	void onSerDes(SE& se, ShaderStageInfo::Texture& texture) {
		SGE_SERDES_IO(se, texture, name);
		SGE_SERDES_IO(se, texture, bindPoint);
		SGE_SERDES_IO(se, texture, bindCount);
		SGE_SERDES_IO(se, texture, formatType);
	}

	template<class SE> inline
		void onSerDes(SE& se, ShaderStageInfo::Sampler& sampler) {
		SGE_SERDES_IO(se, sampler, name);
		SGE_SERDES_IO(se, sampler, bindPoint);
		SGE_SERDES_IO(se, sampler, bindCount);
		SGE_SERDES_IO(se, sampler, formatType);
	}
}
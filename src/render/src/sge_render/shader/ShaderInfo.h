#pragma once
#include <sge_core/base/sge_base.h>
#include <sge_core/json/Json.h>

namespace sge {

#define SGE_SHADER_PROP_TYPE \
	E(None) \
	E(Int) \
	E(Float) \
	E(Vec2f) \
	E(Vec3f) \
	E(Vec4f) \
	E(Color4f) \
	E(Matrix) \
	E(Double) \
//==========

	enum class ShaderPropType : u8 {
#define E(T) T,	
		SGE_SHADER_PROP_TYPE
#undef	E
	};

	inline
	bool enumTryParse(ShaderPropType& out, StrView str) {
#define E(T) if (str == #T) { out = ShaderPropType::T; return true; }
		SGE_SHADER_PROP_TYPE
		return false;
#undef	E
	}

	inline
	StrView enumStr(ShaderPropType v) {
#define E(T) case ShaderPropType::T: return #T;
		switch (v) {
			SGE_SHADER_PROP_TYPE
		default: return "";
		}
#undef	E
	}
#undef SGE_SHADER_PROP_TYPE

	struct ShaderInfo : public NonCopyable {
		struct Attr {
			String tag;
			String defVal;
		};

		struct Prop {
			bool isDefined = false;

			ShaderPropType type = ShaderPropType::None;
			Attr	attr;
			String	name;
			String	defval;

			size_t	offset	= 0;
			size_t	slotIdx	= 0;
		};

		struct Pass {
			String name;
			String vsFunc;
			String psFunc;
		};

		struct ConstBufInfo {
			size_t size    = 0;
			size_t slotIdx = 0;
		};

		Vector_<Prop, 4> properties;
		Vector_<Pass, 1> passes;

		Vector_<ConstBufInfo, 1> constBufInfos;
	};

	template<class SE> inline
	void jsonIO(SE& se, ShaderInfo& info, Json& js) {
		se.io(js["Properties"],		info.properties);
		se.io(js["Passes"],			info.passes);
		se.io(js["ConstBufInfo"],	info.constBufInfos);
	}

	template<class SE> inline
	void jsonIO(SE& se, ShaderInfo::ConstBufInfo& info, Json& js) {
		se.io(js["Size"],	   info.size);
		se.io(js["SlotIndex"], info.slotIdx);
	}

	template<class SE> inline
	void jsonIO(SE& se, ShaderInfo::Pass& pass, Json& js) {
		se.io(js["Name"],	pass.name);
		se.io(js["vsFunc"], pass.vsFunc);
		se.io(js["psFunc"], pass.psFunc);
	}

	template<class SE> inline
	void jsonIO(SE& se, ShaderInfo::Attr& attr, Json& js) {
		se.io(js["Tag"],		  attr.tag);
		se.io(js["DefaultValue"], attr.defVal);
	}

	template<class SE> inline
	void jsonIO(SE& se, ShaderInfo::Prop& prop, Json& js) {
		se.io(js["IsDefined"],	  prop.isDefined);
		se.io(js["Type"],		  prop.type);
		se.io(js["Attribute"],	  prop.attr);
		se.io(js["Name"],		  prop.name);
		se.io(js["DefaultValue"], prop.defval);
		se.io(js["SlotIndex"],	  prop.slotIdx);
		se.io(js["Offset"],		  prop.offset);
	}



}

SGE_FORMATTER_ENUM(sge::ShaderPropType)
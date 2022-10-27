#pragma once

#include <sge_core/base/sge_base.h>
#include <sge_core/base/sge_macro.h>

namespace sge 
{
	#define RenderState_BlendFactor_ENUM_LIST(E) \
		E(Disable, ) \
		E(One, ) \
		E(Zero, ) \
		\
		E(SrcColor,	  ) \
		E(SrcAlpha,	  ) \
		E(DstColor,	  ) \
		E(DstAlpha,	  ) \
		E(ConstColor, ) \
		\
		E(OneMinusSrcColor, ) \
		E(OneMinusSrcAlpha, ) \
		E(OneMinusDstColor, ) \
		E(OneMinusDstAlpha, ) \
		E(OneMinusConstColor, ) \
		\
		E(SrcAlphaSaturate, ) \
//------
	SGE_ENUM_CLASS(RenderState_BlendFactor, : u8)


#define RenderState_BlendOp_ENUM_LIST(E) \
		E(Disable, ) \
		E(Add,) \
		E(Sub,) \
		E(RevSub,) \
		E(Min,) \
		E(Max,) \
//------
	SGE_ENUM_CLASS(RenderState_BlendOp, : u8)


#define RenderState_CullType_ENUM_LIST(E) \
		E(Off,) \
		E(Back,) \
		E(Front,) \
//------
	SGE_ENUM_CLASS(RenderState_CullType, : u8)

#define RenderState_DepthTestOp_ENUM_LIST(E) \
	E(None, ) \
	E(Less,	) \
	E(LessEqual, ) \
	E(Equal, ) \
	E(GreaterEqual, ) \
	E(Greater, ) \
	E(NotEqual, ) \
	E(Always, ) \
	E(Never, ) \
//------
	SGE_ENUM_CLASS(RenderState_DepthTestOp, : u8)


	struct RenderState_Blend {
		using BlendOp = RenderState_BlendOp;
		using Factor  = RenderState_BlendFactor;

		struct Function {
			BlendOp	op		  = BlendOp::Disable;
			Factor  srcFactor = Factor::SrcAlpha;
			Factor  dstFactor = Factor::OneMinusSrcAlpha;
		};

		bool isEnable() const { return rgbFunc.op != BlendOp::Disable || alphaFunc.op != BlendOp::Disable; }

		Function rgbFunc;
		Function alphaFunc;
//		Color4f	 constColor = Color4f(1, 1, 1, 1);
	};

	struct RenderState_DepthTest {
		using DepthTestOp = RenderState_DepthTestOp;

		DepthTestOp	op = DepthTestOp::LessEqual;
		bool writeMask = true;

		bool isEnable() const { return op != DepthTestOp::Always; }
	};

	struct RenderState {
		using Blend		= RenderState_Blend;
		using CullType	= RenderState_CullType;
		using DepthTest = RenderState_DepthTest;

		bool  isWireFrame	= false;
		CullType cullType   = CullType::Back;

		Blend	  blend;
		DepthTest depthTest;
	};

	template<class SE> inline
		void onSerDes(SE& se, RenderState_Blend::Function& blendFunc) {
		SGE_SERDES_IO(se, blendFunc, op);
		SGE_SERDES_IO(se, blendFunc, srcFactor);
		SGE_SERDES_IO(se, blendFunc, dstFactor);
	}

	template<class SE> inline
		void onSerDes(SE& se, RenderState_Blend& blend) {
		SGE_SERDES_IO(se, blend, rgbFunc);
		SGE_SERDES_IO(se, blend, alphaFunc);
//		SGE_SERDES_IO(se, blend, constColor);
	}

	template<class SE> inline
		void onSerDes(SE& se, RenderState_DepthTest& depthTest) {
		SGE_SERDES_IO(se, depthTest, op);
		SGE_SERDES_IO(se, depthTest, writeMask);
	}

	template<class SE> inline
		void onSerDes(SE& se, RenderState& renderState) {
		SGE_SERDES_IO(se, renderState, isWireFrame);
		SGE_SERDES_IO(se, renderState, cullType);
		SGE_SERDES_IO(se, renderState, blend);
		SGE_SERDES_IO(se, renderState, depthTest);
	}


}
#pragma once

#include "../vertex/Vertex.h"
#include "../buffer/RenderGpuBuffer.h"
#include "../shader/Material.h"

namespace sge {

	enum class RenderCommandType : u8 {
		None,
		ClearFrameBuffers,
		SwapBuffers,
		DrawCall,
	};

	class RenderCmd : public NonCopyable {
	public:
		using Type = RenderCommandType;

		RenderCmd(Type type) : m_type(type) {}
		virtual ~RenderCmd() {}

		Type type() const { return m_type; }

#if _DEBUG
		SrcLoc debugLoc;
#endif
	private:
		Type m_type = Type::None;
	};

	class RenderCmd_ClearFrameBuffers : public RenderCmd {
		using Base = RenderCmd;
		using This = RenderCmd_ClearFrameBuffers;
	public:
		RenderCmd_ClearFrameBuffers()
			: Base(Type::ClearFrameBuffers)
		{
		}

		This& setColor(const Color4f& color_) { color = color_; return *this; }
		This& dontClearColor() { color.reset(); return *this; }

		Opt<Color4f> color = Color4f(1, 1, 1, 1);
		Opt<float>	 depth = 0;
	};

	class RenderCmd_DrawCall : public RenderCmd {
		using Base = RenderCmd;
		using This = RenderCmd_DrawCall;
	public:
		RenderCmd_DrawCall()
			:Base(Type::DrawCall)
		{
		}


		
		SPtr<Material> material;


	};

	class RenderCmd_SwapBuffers : public RenderCmd {
		using Base = RenderCmd;
		using This = RenderCmd_SwapBuffers;
	public:
		RenderCmd_SwapBuffers()
			: Base(Type::SwapBuffers)
		{
		}
	};
}

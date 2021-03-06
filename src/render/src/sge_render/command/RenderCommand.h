#pragma once

#include "../vertex/Vertex.h"
#include "../buffer/RenderGpuBuffer.h"
#include "../shader/Material.h"
#include "../mesh/RenderMesh.h"

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
		RenderCmd_ClearFrameBuffers() : Base(Type::ClearFrameBuffers) {}

		This& setColor(const Color4f& color_) { color = color_; return *this; }
		This& dontClearColor() { color.reset(); return *this; }

		meta::opt<Color4f> color = Color4f(1, 1, 1, 1);
		meta::opt<float>   depth = 0;
	};

	class RenderCmd_DrawCall : public RenderCmd {
		using Base = RenderCmd;
		using This = RenderCmd_DrawCall;
	public:
		RenderCmd_DrawCall() :Base(Type::DrawCall) {}

		const VertexLayout* vertexLayout = nullptr;

		RenderFormatType indexFormat = RenderFormatType::None;

		size_t indexCount  = 0;
		size_t vertexCount = 0;

		SPtr<RenderGpuBuffer> vertexBuffer;
		SPtr<RenderGpuBuffer> indexBuffer;

		SPtr<Material> material;
		size_t		   materialPassIndex;
	};

	class RenderCmd_SwapBuffers : public RenderCmd {
		using Base = RenderCmd;
		using This = RenderCmd_SwapBuffers;
	public:
		RenderCmd_SwapBuffers() : Base(Type::SwapBuffers) {}
	};
}

#pragma once

#include <sge_core/graph/Color.h>
#include <sge_render/backend/base/Render_Common.h>

#include <sge_core/alllocators/LinearAllocator.h>

#include "../vertex/Vertex.h"
#include "../buffer/RenderGpuBuffer.h"
#include "../shader/Material.h"
#include "../mesh/RenderMesh.h"
#include "../texture/Texture.h"

namespace sge {

	enum class RenderCommandType : u8 {
		None,
		ClearFrameBuffers,
		SwapBuffers,
		DrawCall,
	};

	class RenderCommand : public NonCopyable {
	public:
		using Type = RenderCommandType;

		RenderCommand(Type type) : m_type(type) {}
		virtual ~RenderCommand() {}

		Type type() const { return m_type; }

#if _DEBUG
		SrcLoc debugLoc;
#endif
	private:
		Type m_type = Type::None;
	};

	class RenderCommand_ClearFrameBuffers : public RenderCommand {
		using Base = RenderCommand;
		using This = RenderCommand_ClearFrameBuffers;
	public:
		RenderCommand_ClearFrameBuffers() : Base(Type::ClearFrameBuffers) {}

		This& setColor(const Color4f& color_) { color = color_; return *this; }
		This& dontClearColor() { color.reset(); return *this; }

		Opt<Color4f> color = Color4f(1, 1, 1, 1);
		Opt<float>   depth = 1;
	};

	class RenderCommand_DrawCall : public RenderCommand {
		using Base = RenderCommand;
		using This = RenderCommand_DrawCall;
	public:
		RenderCommand_DrawCall() :Base(Type::DrawCall) {}

		const VertexLayout* vertexLayout	= nullptr;

		RenderPrimitiveType primitive		= RenderPrimitiveType::None ;
		RenderFormatType	indexFormat		= RenderFormatType::UInt16x1;

		size_t indexCount  = 0;
		size_t vertexCount = 0;

		size_t startIndex = 0;
		size_t baseVertex = 0;

		SPtr<RenderGpuBuffer> vertexBuffer;
		SPtr<RenderGpuBuffer> indexBuffer;

		SPtr<Material> material;
		size_t		   materialPassIndex = 0;

		auto getMaterialPass() { return material ? material->getPass(materialPassIndex) : nullptr; }
	};

	


	class RenderCommand_SwapBuffers : public RenderCommand {
		using Base = RenderCommand;
		using This = RenderCommand_SwapBuffers;
	public:
		RenderCommand_SwapBuffers() : Base(Type::SwapBuffers) {}
	};


	class RenderCommandBuffer : public NonCopyable 
	{
	public:
		static constexpr u64 size = 4 * 1024 * 1024;

		 RenderCommandBuffer();
		~RenderCommandBuffer();

		Span<RenderCommand*> commands() { return m_commands; }


		template<class CMD>
		CMD* newCommand() {
			auto s = sizeof(CMD);
			auto a = std::min<size_t>( 32, Math::nextPow2(s));
			auto* cmd = new (m_allocator.allocate(s, a))CMD();
			m_commands.emplace_back(cmd);
			return cmd;
		}

		void reset();


	private:
		u8*	m_buffer = nullptr;
		LinearAllocator	  m_allocator;
		Vector<RenderCommand*> m_commands;		
	};
}

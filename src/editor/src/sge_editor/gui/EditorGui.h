#pragma once

#include <imgui.h>
#include <sge_render/command/RenderRequest.h>


namespace sge {

	namespace EditorGui = ImGui;

	struct EditorGuiUtil
	{
		template<class T> static
		inline ByteSpan makeByteSpan(ImVector<T>& vec) {
			return spanCast<const u8, const T>(Span<const T>(vec.Data, vec.Size));
		}

		static 
		inline int getGuiButton(UIMouseEventButton src) {
			switch (src) {
				case UIMouseEventButton::Left:		return ImGuiMouseButton_::ImGuiMouseButton_Left;
				case UIMouseEventButton::Right:		return ImGuiMouseButton_::ImGuiMouseButton_Right;
				case UIMouseEventButton::Middle:	return ImGuiMouseButton_::ImGuiMouseButton_Middle;

				default: throw SGE_ERROR("Unhandled mouse button pressed");
			}
		}
	};

	struct EditorGuiHandle_CreateDesc {
		NativeUIWindow* window = nullptr;
	};

	class EditorGuiHandle : public NonCopyable
	{
	public:
		using CreateDesc = EditorGuiHandle_CreateDesc;
		using Util		 = EditorGuiUtil;

		~EditorGuiHandle();
		
		void create(CreateDesc& desc);

		void beginRender();
		void render(RenderRequest& request);

		void onUIMouseEvent(UIMouseEvent& ev);

	protected:

		RenderMesh			m_mesh;
		VertexLayout		m_vertLayout;
		SPtr<Material>		m_material;
		SPtr<Texture2D>		m_fontsTex2D;
	};


}
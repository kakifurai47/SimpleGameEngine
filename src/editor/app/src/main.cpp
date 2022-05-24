
#include <sge_editor.h>
#include <array>

namespace sge {

	class MainWin : public NativeUIWindow {
	public:
		using Base = NativeUIWindow;
		 
		virtual void MainWin::onCreate() override {
			Base::onCreate();
			{
				RenderContext::CreateDesc desc;
				desc.window = this;
				m_ctx.reset(RenderContext::create(desc));
			}
		}

		virtual void MainWin::onPaint() override {
			Base::onPaint();
			auto* c0 = m_buffer.addCmd<RenderCmd_SetViewport>();
			auto* c1 = m_buffer.addCmd<RenderCmd_Clear>();
			c1->color = Color4f(0, 0, 0.5f, 1);







			if (m_ctx) {
				m_ctx->render(m_buffer);
			}
		}

		UPtr<RenderContext> m_ctx = nullptr;
		CommandBuffer m_buffer;
		EditMesh editMesh;
	};
	
	class EditorApp : public NativeUIApp 
	{
		using Base = NativeUIApp;
	protected:
		virtual void EditorApp::onCreate() override
		{
			Base::onCreate();
			
			{
				Renderer::CreateDesc desc;
				desc.type = Render_ApiType::DX11;
				Renderer::create(desc);
			}
			{
				m_mainWin.create();
			}
		}

		virtual void EditorApp::onQuit() override {
			Base::onQuit();
			//m_renderer.clear();	
		}
	private:
		MainWin  m_mainWin;
		
		//Renderer m_renderer;
	};
}

int main() {
	sge::EditorApp app;	
	app.run();

	return 0;
}

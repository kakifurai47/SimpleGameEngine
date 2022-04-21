
#include <sge_editor.h>

namespace sge {
	class MainWin : public NativeUIWindow {
	public:
		using Base = NativeUIWindow;

		virtual void MainWin::onCreate() override {
			Base::onCreate();

			RenderContext::CreateDesc desc;
			desc.window = this;
			m_ctx.reset(RenderContext::create(desc));			
		}

		virtual void MainWin::onPaint() override {
			Base::onPaint();
		}

		UPtr<RenderContext> m_ctx = nullptr;
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

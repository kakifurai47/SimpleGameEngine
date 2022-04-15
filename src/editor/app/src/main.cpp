
#include <sge_editor.h>



namespace sge {
	class MainWin : public NativeUIWindow {
	public:
		using Base = NativeUIWindow;

		void setRenderer(Renderer* renderer) {
			m_renderer = renderer;
		}

		virtual void MainWin::onPaint() override {
			Base::onPaint();
			if (m_renderer) {
				m_renderer->run();
			}
		}

		virtual void MainWin::onDestroy() override {
			m_renderer = nullptr;
			Base::onDestroy();			
		}

		Renderer* m_renderer = nullptr;
	};
	
	class EditorApp : public NativeUIApp 
	{
		using Base = NativeUIApp;
	protected:
		virtual void EditorApp::onCreate() override 
		{
			Base::onCreate();
			m_mainWin.create();

			m_renderer.create(&m_mainWin, RenderAPI::dx11);
			m_mainWin.setRenderer(&m_renderer);
			{
				m_renderer.drawTriangle();
			}
			m_renderer.run();
		}

		virtual void EditorApp::onQuit() override {
			Base::onQuit();
			m_renderer.clear();	
		}
	private:
		MainWin  m_mainWin;
		Renderer m_renderer;
	};
}

int main() {
	sge::EditorApp app;	
	app.run();

	return 0;
}

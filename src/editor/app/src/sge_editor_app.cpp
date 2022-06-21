#include <sge_editor.h>
#include <fileapi.h>

namespace sge {

	class MainWin : public NativeUIWindow {
	public:
		using Base = NativeUIWindow;
		 
		virtual void MainWin::onCreate() override {
			Base::onCreate();

			auto* renderer = Renderer::current();
			{
				RenderContext::CreateDesc desc;
				desc.window = this;
				m_renderCtx = renderer->createContext(desc);
			}
			{
				m_mat = renderer->createMaterial();
				m_mat->setShader("Assets/Shaders/test.shader");
			}
			{
				EditMesh editMesh;
#if 1
				editMesh.loadObj("Assets/Mesh/standford_bunny.obj");
#else
				auto& positions = editMesh.pos;
				positions.emplace_back(   0.0f,  0.5f, 0.0f );
				positions.emplace_back(  0.45f, -0.5f, 0.0f );
				positions.emplace_back( -0.45f, -0.5f, 0.0f );
				
				editMesh.indicies.emplace_back(0);
				editMesh.indicies.emplace_back(1);
				editMesh.indicies.emplace_back(2);
#endif
				auto& colors = editMesh.color;
				colors.resize(editMesh.pos.size(), { 168, 255, 255, 255 });
				m_mesh.create(editMesh);
			}
		}

		virtual void MainWin::onPaint() override {
			Base::onPaint();
			if (!m_renderCtx) return;

			//alpha = alpha < 1 ? alpha + 0.005 : 0;
			//if (m_mat.ptr()) {
			//	m_mat->set("x", alpha);
			//}

			m_renderCtx->beginRender();
			
			m_cmdBuf.reset();
			m_cmdBuf.clearFrameBuffers()->setColor({ 1, 1, 0.5f, 1 });
			m_cmdBuf.drawMesh(SGE_LOC, m_mesh, *m_mat);
			m_cmdBuf.swapBuffers();
			m_renderCtx->commit(m_cmdBuf);
			
			m_renderCtx->endRender();
			paintNeeded();
		}

		RenderMesh m_mesh;

		float alpha = 0;
		SPtr<RenderContext> m_renderCtx;
		RenderCommandBuffer	m_cmdBuf;
		SPtr<Material>		m_mat;
	};
	
	class EditorApp : public NativeUIApp 
	{
		using Base = NativeUIApp;
	protected:
		virtual void EditorApp::onCreate() override {

			{
				String file = getExecutableFileName();
				String path = FilePath::dirname(file);
				path.append("/../../../../../../examples/Test101");
				Directory::setCurrent(path);

				auto dir = Directory::getCurrent();
				SGE_LOG("dir = {}", dir);
			}

			Base::onCreate();
			{
				Renderer::CreateDesc desc;
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

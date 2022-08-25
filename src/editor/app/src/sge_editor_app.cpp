#include <sge_editor.h>
#include <fileapi.h>

#include <sge_render/command/RenderRequest.h>

#include <sge_core/math/MathCamera3.h>
#include <sge_render/shader/RenderState.h>
#include <sge_render/terrain/RenderTerrain.h>

namespace sge {

	struct MyStructA
	{
		int a = 3;
	};


	class MainWin : public NativeUIWindow {
	public:
		using Base = NativeUIWindow;
		 
		virtual void MainWin::onCreate() override {
			Base::onCreate();
			auto* renderer = Renderer::current();

			{
				RenderContext::CreateDesc desc;
				desc.window = this;
				m_renderContext = renderer->createContext(desc);
			}

			{
				m_terrain.createFromHeightMapFile(4, "Assets/Terrain/TerrainHeight_Small.png");



			}

			m_camera.setPos({0, 10, 10});
			m_camera.setAim({0,  0,  0});
			{
				//----
				Texture2D_CreateDesc desc;

				desc.imageToUpload.emplace();
				auto& image = desc.imageToUpload.value();
				image.load("Assets/Textures/banana.png");

				desc.colorType = image.colorType();
				desc.size = image.size();

				auto tex = renderer->createTexture2D(desc);

				//----
				auto shad = renderer->createShader("Assets/Shaders/test.shader");
				m_mat = renderer->createMaterial();
				m_mat->setShader(shad);
				m_mat->setParam("mainTex", tex);
			}
			{
				EditMesh editMesh;
#if 1
//				editMesh.loadObj("Assets/Mesh/standford_bunny.obj");
				editMesh.loadObj("Assets/Mesh/monkey.obj");
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

		virtual void MainWin::onUIMouseEvent(UIMouseEvent& ev) override 
		{
			if (ev.isDragging()) {
				
				using Modifier  = UIEventModifier;
				using Button = UIMouseEventButton;

				switch (ev.initButton) {
					case (Button::Left):	{ auto d = ev.deltaPos *   0.01f;  m_camera.orbit(d);				} break;
					case (Button::Middle):  { auto d = ev.deltaPos *   0.01f;  m_camera.move ({ d.x, d.y , 0});	} break;
					case (Button::Right):	{ auto d = ev.deltaPos * -0.005f;  m_camera.dolly(d.x + d.y);		} break;				
				}
			}
		}

		virtual void MainWin::onPaint() override {
			Base::onPaint();
			if (!m_renderContext) return;

			m_renderContext->beginRender();

			m_renderRequest.reset();

			m_renderRequest.model	= Mat4f::s_identity();
			m_renderRequest.view	= m_camera.viewMat ();
			m_renderRequest.proj	= m_camera.projMat ();

			m_renderRequest.camera_pos = m_camera.pos();
			
			m_renderRequest.clearFrameBuffers()->setColor({ 0.f, 0.f, 0.f, 1 });
			m_renderRequest.drawMesh(SGE_LOC, m_mesh, m_mat);

			m_terrain.render(m_renderRequest);

			m_renderRequest.swapBuffers();
			
			m_renderContext->commit(m_renderRequest.commandBuffer);
			m_renderContext->endRender();

			paintNeeded();
		}

		virtual void MainWin::onCloseButton() override {
			NativeUIApp::current()->quit();
		}

		RenderMesh m_mesh;
		Camera3f m_camera;

		float alpha = 0;
		SPtr<RenderContext> m_renderContext;

		RenderRequest		m_renderRequest;
		RenderCommandBuffer	m_cmdBuf;
		SPtr<Material>		m_mat;

		RenderTerrain		m_terrain;
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

		#if 1 // for quick testing
			{
				SHELLEXECUTEINFO ShExecInfo = { 0 };
				ShExecInfo.cbSize = sizeof(SHELLEXECUTEINFO);
				ShExecInfo.fMask = SEE_MASK_NOCLOSEPROCESS;
				ShExecInfo.hwnd = NULL;
				ShExecInfo.lpVerb = L"open";
				ShExecInfo.lpFile = L"compile_shaders.bat";
				ShExecInfo.lpParameters = L"";
				ShExecInfo.lpDirectory = NULL;
				ShExecInfo.nShow = SW_SHOW;
				ShExecInfo.hInstApp = NULL;
				ShellExecuteEx(&ShExecInfo);
				WaitForSingleObject(ShExecInfo.hProcess, INFINITE);
				CloseHandle(ShExecInfo.hProcess);
			}
		#endif

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

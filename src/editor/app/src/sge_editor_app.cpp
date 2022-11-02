#include <sge_editor.h>
#include <fileapi.h>

#include <sge_render/command/RenderRequest.h>

#include <sge_core/math/MathCamera3.h>
#include <sge_render/shader/RenderState.h>
#include <sge_render/terrain/RenderTerrain.h>

namespace sge 
{
	class MainWin : public EditorMainWindow 
	{
		using Base = EditorMainWindow;
	public:
		 
		virtual void MainWin::onCreate(CreateDesc& desc) override {
			Base::onCreate(desc);

			auto* renderer = Renderer::current();

			{
				m_terrain.createFromHeightMapFile(4, "Assets/Terrain/TerrainHeight_Small.png");
			}

			{
				EditorContext::createContext();
			}


			m_camera.setPos({0, 10, 10});
			m_camera.setAim({0,  0,  0});

			{//test material
				Texture2D_CreateDesc t2dDesc;

				t2dDesc.imageToUpload.emplace();
				auto& image = t2dDesc.imageToUpload.value();
				image.load("Assets/Textures/Debug.png");

				t2dDesc.colorType = image.colorType();
				t2dDesc.size = image.size();

				auto tex = renderer->createTexture2D(t2dDesc);

				//----
				auto shad = renderer->createShader("Assets/Shaders/test.shader");
				m_mat = renderer->createMaterial();
				m_mat->setShader(shad);
				m_mat->setParam("mainTex", tex);
			}
			{//test mesh
				EditMesh editMesh;
			#if 1
//				editMesh.loadObj("Assets/Mesh/standford_bunny.obj");
//				editMesh.loadObj("Assets/Mesh/monkey.obj");
				editMesh.loadObj("Assets/Mesh/box.obj");
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
			Base::onUIMouseEvent(ev);

			if (ev.isDragging()) {
				using Modifier  = UIEventModifier;
				using Button = UIMouseEventButton;

				switch (ev.capturedButton)  {
					case (Button::Left):	{ auto d = ev.deltaPos *   0.01f;  m_camera.orbit(d);				} break;
					case (Button::Middle):  { auto d = ev.deltaPos *   0.01f;  m_camera.move ({ d.x, d.y , 0});	} break;
					case (Button::Right):	{ auto d = ev.deltaPos * -0.005f;  m_camera.dolly(d.x + d.y);		} break;				
				}
			}
		}

		virtual void MainWin::onPaint() override {
			Base::onPaint();
			if (!m_renderContext) return;

			m_editorGuiHandle.beginRender();

			m_camera.setViewport(clientRect());
			
			m_renderContext->setFrameBufferSize(clientRect().size);
			m_renderContext->beginRender();

			m_renderRequest.reset();

//			if (m_showDemoWindow) {
//				EditorGui::ShowDemoWindow(&m_showDemoWindow);
//			}

			if (m_showSceneInfoWindow)
			{
				EditorGui::Begin("SceneInfo Window", &m_showSceneInfoWindow);

				PropertyAttribute att;
				att.name = "background color";

				EditorGui::ShowColor(m_backgroundCol, att);
				EditorGui::End();
			}

			m_controlTab.     draw(m_scene, m_renderRequest);
			m_hierachyWindow. draw(m_scene, m_renderRequest);
			m_inspectorWindow.draw(m_scene, m_renderRequest);

			m_renderRequest.model	= Mat4f::s_identity();
			m_renderRequest.view	= m_camera.viewMat ();
			m_renderRequest.proj	= m_camera.projMat ();

			m_renderRequest.camera_pos = m_camera.pos();
			
			m_renderRequest.clearFrameBuffers()->setColor(m_backgroundCol);

			RenderSystem::instance()->render(m_renderRequest);
			
			{
				EditorGui::Window debugWindow("CullingList");
				auto list = RenderSystem::instance()->_culledEntities();

				for (auto* e : list)
				{
					if (!e) continue;
					EditorGui::Text("name {} : id [{}]", e->name(), (int)e->id());
				}
			}

//			m_renderRequest.drawMesh(SGE_LOC, m_mesh, m_mat);
//			m_terrain.render(m_renderRequest);
			m_editorGuiHandle.render(m_renderRequest);

			if (EditorGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
				EditorGui::UpdatePlatformWindows();
				EditorGui::RenderPlatformWindowsDefault();
			}

			m_renderRequest.swapBuffers();

			m_renderContext->commit(m_renderRequest.commandBuffer);
			m_renderContext->endRender();
		}

		virtual void MainWin::onCloseButton() override {
			NativeUIApp::current()->quit(0);
		}

		RenderMesh m_mesh;
		Camera3f m_camera;

		Color4f				m_backgroundCol {};

		EditorControlTab		m_controlTab;
		EditorHierachyWindow	m_hierachyWindow;
		EditorInspectorWindow	m_inspectorWindow;

		Scene				m_scene;
		
		SPtr<Material>		m_mat;
		RenderTerrain		m_terrain;
		bool				m_showDemoWindow			= true;
		bool				m_showSceneInfoWindow		= true;		
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
				NativeUIWindow::CreateDesc desc;
//				desc.type = NativeUIWindow::CreateDesc::Type::ToolWindow;
				desc.isMainWindow   = true;
				desc.centerToScreen = true;
				desc.rect = {0,0,1280,720};
				m_mainWin.create(desc);
//				m_mainWin.setWindowTitle("");
			}
		}

		virtual void EditorApp::onUpdate(float deltaTime) override {
			m_mainWin.update(deltaTime);
			m_mainWin.paintNeeded();
		}

		virtual void EditorApp::onQuit() override { Base::onQuit(); }

	private:
		MainWin  m_mainWin;
	};
}

int main() {
	sge::EditorApp app;
	app.run();

	return 0;
}

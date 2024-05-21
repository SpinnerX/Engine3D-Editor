#include <Engine3D/Core/EngineLogger.h>
#include <Engine3D/Engine3DPrecompiledHeader.h>
#include <Engine3D/Event/KeyCodes.h>
#include <Engine3D/Event/KeyEvent.h>
#include <Engine3D/Event/MouseEvent.h>
#include <Engine3D/Scene2D/Components.h>
#include <imgui/imgui.h>
#include "EditorLayer.h"
#include "UI/UI.h"

namespace Engine3D{
	extern const std::filesystem::path _assetPath;

	EditorLayer::EditorLayer() : Layer("Sandbox2D"){
	}

	void EditorLayer::OnAttach(){
		RENDER_PROFILE_FUNCTION();
		
		// @note For creating our textures
		playIcon = Texture2D::Create("Resources/icons/PlayButton.png");
		stopIcon = Texture2D::Create("Resources/icons/StopButton.png");

		FrameBufferSpecifications frameBufSpecs;
		// @note In graphics, there are different formats OpenGL handles RGB, such as RGBA8, RED (same as RGBA8 but one channel that is an int.)
		frameBufSpecs.attachments = { FrameBufferTextureFormat::RGBA8, FrameBufferTextureFormat::RED_INTEGER, FrameBufferTextureFormat::DEPTH24STENCIL8 };
		frameBufSpecs.width = 1280;
		frameBufSpecs.height = 720;

	
		framebuffer = FrameBuffer::Create(frameBufSpecs); // Creating out frame buffer
		currentlyActiveScene = CreateRef<Scene>();

		auto commandLineArgs = Application::GetCmdLineArg();

		if(commandLineArgs.count > 1){
			auto sceneFilepath = commandLineArgs[1];
			SceneSerializer serializer(currentlyActiveScene);
			serializer.deserializeRuntime(sceneFilepath);
		}

		editorCamera = EditorCamera(30.0f, 1.778f, 0.1f, 1000.0f);
		
		sceneHeirarchyPanel.setContext(currentlyActiveScene);
	}

	void EditorLayer::OnDetach(){
		RENDER_PROFILE_FUNCTION();
	}

	void EditorLayer::OnUpdate(Timestep ts){
		RENDER_PROFILE_FUNCTION();
		
		// Updating scripts
		// Iterate all entities in ScriptableEntity
		if(FrameBufferSpecifications spec = framebuffer->getSpecifications();
				viewportSize.x > 0.0f and viewportSize.y > 0.0f &&
				(spec.width != viewportSize.x || spec.height != viewportSize.y)){
			framebuffer->resize((uint32_t)viewportSize.x, (uint32_t)viewportSize.y);
			editorCamera.setViewportSize(viewportSize.x, viewportSize.y);
			currentlyActiveScene->onViewportResize(viewportSize.x, viewportSize.y); // viewport resizing every time the window size is changed
		}
		
		// Update (if mouse cursor is focused in window.)
		// editorCamera.OnUpdate(ts);
		framebuffer->Bind();
		RendererCommand::setClearColor({ 0.1f, 0.1f, 0.1f, 1 });
		RendererCommand::clear();
		
		// @note Clearing out entity ID attachment to -1
		framebuffer->clearColorAttachment(1, -1);

		Ref<Texture2D> icon = sceneState == SceneState::Edit ? playIcon : stopIcon;
		switch (sceneState) {
		case SceneState::Edit:
			{
				editorCamera.OnUpdate(ts);
				currentlyActiveScene->OnUpdateEditor(editorCamera);
			}
			break;
		case SceneState::Play:
			{
				currentlyActiveScene->OnUpdateRuntime(ts);
				break;
			}
		}
		
		// @note this gives us the cursor location.
		auto[mouseX, mouseY] = ImGui::GetMousePos();
		mouseX -= viewportBound[0].x; // making top-left (0, 0), if subtracting x and y with mouse pos.
		mouseY -= viewportBound[0].y;

		glm::vec2 viewportSize = viewportBound[1] - viewportBound[0];
		mouseY = viewportSize.y - mouseY; // This makes our bottom left (0, 0)
		int currentMouseX = (int)mouseX;
		int currentMouseY = (int)mouseY;
		
		// @note giving feedback the pixel of that vertex buffer.
		if(mouseX >= 0 and mouseY >= 0 and mouseX < (int)viewportSize.x and mouseY < (int)viewportSize.y){
			int pixel = framebuffer->readPixel(1, currentMouseX, currentMouseY);
			hoveredEntity = (pixel == -1 || pixel > 10000000) ? Entity() : Entity((entt::entity)pixel, currentlyActiveScene.get());
		}

		framebuffer->Unbind();
	}

	void EditorLayer::OnUIRender(){
		static bool dockSpaceOpened = true;
		static bool opt_fullscreen = true;
		static bool opt_padding = false;
		static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

		// We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
		// because it would be confusing to have two docking targets within each others.
		ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
		if (opt_fullscreen){
			const ImGuiViewport* viewport = ImGui::GetMainViewport();
			ImGui::SetNextWindowPos(viewport->Pos);
			ImGui::SetNextWindowSize(viewport->Size);
			ImGui::SetNextWindowViewport(viewport->ID);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
			window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
			window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
		}
		else{
			dockspace_flags &= ~ImGuiDockNodeFlags_PassthruCentralNode;
		}

		// When using ImGuiDockNodeFlags_PassthruCentralNode, DockSpace() will render our background
		// and handle the pass-thru hole, so we ask Begin() to not render a background.
		if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
			window_flags |= ImGuiWindowFlags_NoBackground;
		
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		ImGui::Begin("DockSpace Demo", &dockSpaceOpened, window_flags);
		ImGui::PopStyleVar();

		if(opt_fullscreen)
			ImGui::PopStyleVar(2);

		// Submit the DockSpace
		ImGuiIO& io = ImGui::GetIO();
		float minWinSizeX = 370.0f;
		ImGuiStyle& style = ImGui::GetStyle();
		style.WindowMinSize.x = 370.0f;
		

		if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable){
			ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
			ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
		}
		style.WindowMinSize.x = minWinSizeX;

		//! @note serialization/deserialization file IO UI widgets
		UI::UI_CreateMenubar([this](){
			if (ImGui::BeginMenu("File")){	
				if(ImGui::MenuItem("New", "Ctrl+N")){
					NewScene();
				}
				ImGui::Separator();

				if(ImGui::MenuItem("Open", "Ctrl+O")){
					LoadScene();
				}
				ImGui::Separator();

				if(ImGui::MenuItem("Save as", "Ctrl+Shift+s")){
					SaveAs();
				}
				ImGui::Separator();

				if(ImGui::MenuItem("Exit", "Ctrl+X")){
					Application::Close();
				}
				ImGui::EndMenu();
			}
		});
		
		// @note TODO: Probably adding panels to a list, in the cases that there will be multiple panels for the editor.
		sceneHeirarchyPanel.OnUIRender();
		contentBrowserPanel.OnUIRender();

		UI::UI_CreateWidget("Debug Renderer", [this](){
			std::string name = "None";
			if(hoveredEntity){
				name = hoveredEntity.GetComponent<TagComponent>().tag;
			}

			ImGui::Text("Hovered Entity: %s", name.c_str());

			auto stats = Renderer2D::getStats();
			ImGui::Text("Debug Renderer Information");
			ImGui::Text("Draw Calls %d", stats.drawCalls);
			ImGui::Text("Quads: %d", stats.quadCount);
			ImGui::Text("Vertices: %d", stats.getTotalVertexCount());
			ImGui::Text("Indices: %d", stats.getTotalIndexCount());
		});

		//! @note Viewport acts as our main screen that we use to render all of our scenes
		//! @note We do not have a main window, but we represent our entire editor as a viewport!
		UI::UI_CreateWidget("Viewport", [this](){
			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{0, 0});
			isViewportFocused = ImGui::IsWindowFocused(); // If viewport is focused then we don't want to block incoming events.
			isViewportHovered = ImGui::IsWindowHovered();

			auto viewportMinRegion = ImGui::GetWindowContentRegionMin();
			auto viewportMaxRegion = ImGui::GetWindowContentRegionMax();
			// @note If tab bar is expanded, then the cursor will be expanded
			auto viewportOffset = ImGui::GetWindowPos();

			viewportBound[0] = { viewportMinRegion.x + viewportOffset.x, viewportMinRegion.y + viewportOffset.y};
			viewportBound[1] = { viewportMaxRegion.x + viewportOffset.x, viewportMaxRegion.y + viewportOffset.y};

			Application::GetImGuiLayer()->SetBlockEvents(!isViewportFocused && !isViewportHovered); // if either out of focused or hovered
			
			ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
			viewportSize = {viewportSize.x, viewportSize.y};
			// Assuming the viewPortPanelSize is this type.
			if(viewportSize != *((glm::vec2 *)&viewportPanelSize) && viewportPanelSize.x > 0 && viewportPanelSize.y > 0){
				// Recreating the frame buffer.
				framebuffer->resize((uint32_t)viewportPanelSize.x, (uint32_t)viewportPanelSize.y);
				viewportSize = {viewportPanelSize.x, viewportPanelSize.y};
			}

			// By passing this renderer ID, this gives us the ID of the texture that we want to render.
			uint32_t textureID = framebuffer->getColorAttachmentRendererID(); // Getting color buffer from frame buffer
			ImGui::Image(reinterpret_cast<void *>(textureID), ImVec2{viewportSize.x, viewportSize.y}, ImVec2{0, 1}, ImVec2{1, 0});
			
			// @note This allows us to drop content browser items to this specific target od draggind/dropping
			//! @note ImGui::BeginDragDropTarget() specifically means that if user has a source and hovering
			//! @note Whereas ImGui::BeginDragDropSource() means if user is clicking and dragging source
			//! @note How we can drag and drop our scenes into our viewport
			if(ImGui::BeginDragDropTarget()){

				//! @note We must check if this is valid before we load in our scene from our drag dropped target
				if(const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(ContentBrowserPanel::GetContentsID().c_str())){
					std::string filepath((const char*)payload->Data, payload->DataSize);

					LoadScene(std::filesystem::path(_assetPath) / filepath);
				}

				ImGui::EndDragDropTarget();
			}

			// Gizmos
			Entity selectedEntity = sceneHeirarchyPanel.getSelectedEntity();
			
			// @note only wanting to select the gizmo only when selected an entity and the type isn't -1 (unknown)
			if(selectedEntity && gizmo_t != -1){
				ImGuizmo::SetOrthographic(false);
				ImGuizmo::SetDrawlist();
				
				// Drawing the gizmo (and figuring out where the camera is.
				ImGuizmo::SetRect(viewportBound[0].x, viewportBound[0].y, viewportBound[1].x - viewportBound[0].x, viewportBound[1].y - viewportBound[0].y);

				// @note Getting the camera information
				// Editor Camera
				const glm::mat4& cameraProjection = editorCamera.getProjection();
				glm::mat4 cameraView = editorCamera.getViewMatrix();

				// Snapping
				bool isSnap = InputPoll::IsKeyPressed(KeyCode::LeftControl);
				float snapValue = 0.5f;

				if(gizmo_t == ImGuizmo::OPERATION::ROTATE)
					snapValue = 45.0f;

				float snapValues[3] = {snapValue, snapValue, snapValue};
				
				// Enttiy Transform
				auto& tc = selectedEntity.GetComponent<TransformComponent>();
				glm::mat4 transform = tc.GetTransform();


				ImGuizmo::Manipulate(glm::value_ptr(cameraView), glm::value_ptr(cameraProjection), (ImGuizmo::OPERATION)gizmo_t, ImGuizmo::LOCAL, glm::value_ptr(transform), nullptr, isSnap ? snapValues : nullptr);
				
				// @note checking if gizmo is selected.
				if(ImGuizmo::IsUsing()){
					glm::vec3 translation, rotation, scale;
					Math::decomposeTransform(transform, translation, rotation, scale);	
					glm::vec3 deltaRotation = rotation - tc.rotation;
					tc.translation = translation;
					tc.rotation += deltaRotation; // @note preventing gimbal lock
					tc.scale = scale;
				}
			}

			ImGui::PopStyleVar();
		});
		
		//! @note Top Tool bar for the UI for play/stop/edit simulation icons
		UI::UI_CreateToolbar("##toolbox", [this](){
			bool toolbarEnabled = (bool)currentlyActiveScene;

            ImVec4 tintColor = ImVec4(1, 1, 1, 1);
            if(!toolbarEnabled) tintColor.w = 0.5f;
            
            float size = ImGui::GetWindowHeight() - 4.0f;
            ImGui::SetCursorPosX((ImGui::GetWindowContentRegionMax().x * 0.5f) - (size * 0.5f));

            // @note checking to see what state we are in. (If playing/stopping)
            Ref<Texture2D> icon = sceneState == SceneState::Edit ? playIcon : stopIcon;
            
            // @note GetWindowContentRegionMax().x is how much space is there for content (widgets)
            // @note 0.5f is the offset for padding.
            // @note takes button size and halves it and makes the offset the center of that tab. (centering  buttons)
            ImGui::SameLine((ImGui::GetWindowContentRegionMax().x * 0.5f) - (size * 0.5f));

            // if(ImGui::ImageButton((ImTextureID)icon->GetRendererID(), ImVec2{size, size}, ImVec2(0, 0), ImVec2(1, 1))){
            if(ImGui::ImageButton(reinterpret_cast<void *>(icon->GetRendererID()), ImVec2{size, size}, ImVec2(0, 0), ImVec2(1, 1))){
            	if(sceneState == SceneState::Edit)
            		OnScenePlay();
            	else if(sceneState == SceneState::Play)
            		OnSceneEdit();
            }
		});


		ImGui::End();

	}

	void EditorLayer::OnEvent(Event& e){
		editorCamera.OnEvent(e);
		EventDispatcher dispatcher(e);

		dispatcher.Dispatch<KeyPressedEvent>([this](KeyPressedEvent& e){
			if(e.GetRepeatedCount() > 0){
				return false;
			}

			if(InputPoll::IsKeyPressed(ENGINE_KEY_ESCAPE)){
				Application::Close();
			}

			bool control = InputPoll::IsKeyPressed(KeyCode::LeftControl) || InputPoll::IsKeyPressed(KeyCode::RightControl);
			bool shift = InputPoll::IsKeyPressed(KeyCode::LeftShift) || InputPoll::IsKeyPressed(KeyCode::RightShift);

			switch (e.GetKeyCode()) {
			case KeyCode::N:
			{
				if(control){
					NewScene();
				}
			}
				break;

			case KeyCode::O:
			{
				if(control){
						LoadScene();
				}
			}
				break;

			case KeyCode::S:
			{
				if(control && shift){
					SaveAs();
				}
			}
				break;
			case KeyCode::X:
				if(control){
					Application::Close();
				}
				break;
				// Gizmos controls
			case KeyCode::Q: // Selection
				gizmo_t = -1;
				break;
			case  KeyCode::W:
				gizmo_t = ImGuizmo::OPERATION::TRANSLATE;
				break;
			case KeyCode::E:
				gizmo_t = ImGuizmo::OPERATION::ROTATE;
				break;
			case KeyCode::R:
				gizmo_t = ImGuizmo::OPERATION::SCALE;
				break;
			default:
				break;
			}

			return false;
		});

		dispatcher.Dispatch<MouseButtonPressedEvent>([this](MouseButtonPressedEvent& e){
			// @note Change entity that we are clicking (only if we are hovering over that entitiy specifically.
			// @note enabling mouse picking here
			if(InputPoll::IsMousePressed(Mouse::ButtonLeft)){
				if(isViewportHovered && !ImGuizmo::IsOver() && !InputPoll::IsKeyPressed(ENGINE_KEY_LEFT_SUPER)){
					sceneHeirarchyPanel.setSelectedEntity(hoveredEntity);
				}
			}

			return false;
		});
	}

	void EditorLayer::NewScene(){
		currentlyActiveScene = CreateRef<Scene>();
		currentlyActiveScene->onViewportResize((uint32_t)viewportSize.x, (uint32_t)viewportSize.y);
		sceneHeirarchyPanel.setContext(currentlyActiveScene);
	}

	void EditorLayer::LoadScene(){
		std::string filepath = FileDialogs::openFile("Engine3D (*.engine)\0*.engine\0");
		if(filepath.empty()){
			coreLogWarn("Filepath in LoadScene() was not selected/empty!");
			return;
		}

		coreLogTrace("Loading scene path ====> {0}\n", filepath);

		if(sceneState == SceneState::Play){
			OnSceneEdit();
			return;
		}

		Ref<Scene> scene = CreateRef<Scene>();
		SceneSerializer serializer(scene);

		if(serializer.deserialize(filepath)){
			editorScene = scene;
			editorScene->onViewportResize((uint32_t)viewportSize.x, (uint32_t)viewportSize.y);
			sceneHeirarchyPanel.setContext(editorScene);
			currentlyActiveScene = editorScene;
		}
	}
	
	void EditorLayer::LoadScene(const std::filesystem::path& path){
		// currentlyActiveScene = CreateRef<Scene>();
		// currentlyActiveScene->onViewportResize((uint32_t)viewportSize.x, (uint32_t)viewportSize.y);
		// sceneHeirarchyPanel.setContext(currentlyActiveScene);

		if(path.string().empty()){
			coreLogWarn("Filepath in LoadScene(filesystem::path) was not selected/empty!");
			return;
		}

		Ref<Scene> scene = CreateRef<Scene>();
		SceneSerializer serializer(scene);
		if(serializer.deserialize(path)){
			editorScene = scene;
			editorScene->onViewportResize((uint32_t)viewportSize.x, (uint32_t)viewportSize.y);
			sceneHeirarchyPanel.setContext(editorScene);
			currentlyActiveScene = editorScene;
		}
	}

	void EditorLayer::SaveAs(){
		std::string filepath = FileDialogs::saveFile("Engine3D (*.engine)\0*.engine\0");

		if(filepath.empty()){
			return;
		}
		
		if(!filepath.empty()){
			SceneSerializer serializer(currentlyActiveScene);
			serializer.serializer(filepath);
		}
	}

	void EditorLayer::OnScenePlay(){
		sceneState = SceneState::Play;

		// currentlyActiveScene = Scene::Copy(editorScene); //! @note  Assigning that our current active scene is our editor.

		currentlyActiveScene = editorScene;
		currentlyActiveScene->OnRuntimeStart();
		sceneHeirarchyPanel.setContext(currentlyActiveScene);
	}

	void EditorLayer::OnSceneEdit(){
		sceneState = SceneState::Edit;
		currentlyActiveScene->OnRuntimeStop();
		currentlyActiveScene = editorScene;
		sceneHeirarchyPanel.setContext(currentlyActiveScene);
	}
};

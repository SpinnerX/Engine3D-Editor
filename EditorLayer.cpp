#include "EditorLayer.h"
#include <Engine3D/OpenGL/OpenGLShader.h>
#include <Engine3D/Entt/entt.h>
#include <Engine3D/Scene2D/SceneSerializer.h>
#include <Engine3D/platforms/PlatformUtils.h>
#include <Engine3D/Math/Math.h>
#include <box2d/b2_body.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <Engine3D/Core/Application.h>
#include <imgui/imgui.h>
#include <ImGuizmo/ImGuizmo.h>
#include <Engine3D/Scene2D/Components.h>
#include <Engine3D/Renderer2D/RenderCommand.h>

namespace Engine3D{
	EditorLayer::EditorLayer() : Layer("Sandbox2D"){
	}

	void EditorLayer::onAttach(){
		RENDER_PROFILE_FUNCTION();
		
		// @note For creating our textures
		_checkerboardTexture = Texture2D::Create("assets/Checkerboard.png");
		_iconPlay = Texture2D::Create("assets/icons/PlayButton.png");
		_iconStop = Texture2D::Create("assets/icons/StopButton.png");

		FrameBufferSpecifications frameBufSpecs;
		// @note In graphics, there are different formats OpenGL handles RGB, such as RGBA8, RED (same as RGBA8 but one channel that is an int.)
		frameBufSpecs.attachments = { FrameBufferTextureFormat::RGBA8, FrameBufferTextureFormat::RED_INTEGER, FrameBufferTextureFormat::DEPTH24STENCIL8 };
		frameBufSpecs.width = 1280;
		frameBufSpecs.height = 720;

	
		_framebuffers = FrameBuffer::Create(frameBufSpecs); // Creating out frame buffer
		_activeScene = CreateRef<Scene>();

		auto commandLineArgs = Application::Get().getCommandLineArgs();

		if(commandLineArgs.count > 1){
			auto sceneFilepath = commandLineArgs[1];
			SceneSerializer serializer(_activeScene);
			serializer.deserializeRuntime(sceneFilepath);
		}

		_editorCamera = EditorCamera(30.0f, 1.778f, 0.1f, 1000.0f);
		
		_sceneHeirarchyPanel.setContext(_activeScene);
		/* hoveredEntity = Entity(); */
	}

	void EditorLayer::onDetach(){
		RENDER_PROFILE_FUNCTION();
	}

	void EditorLayer::onUpdate(Timestep ts){
		RENDER_PROFILE_FUNCTION();
		
		// Updating scripts
		// Iterate all entities in ScriptableEntity
		
		if(FrameBufferSpecifications spec = _framebuffers->getSpecifications();
				_viewportSize.x > 0.0f and _viewportSize.y > 0.0f &&
				(spec.width != _viewportSize.x || spec.height != _viewportSize.y)){
			_framebuffers->resize((uint32_t)_viewportSize.x, (uint32_t)_viewportSize.y);
			_editorCamera.setViewportSize(_viewportSize.x, _viewportSize.y);
			_activeScene->onViewportResize(_viewportSize.x, _viewportSize.y); // viewport resizing every time the window size is changed
		}
		
		// Update (if mouse cursor is focused in window.)
		/* if(_isViewportFocused){ */
			/* _cameraController.onUpdate(ts); */
		/* } */
		_editorCamera.onUpdate(ts);
		
		// Render
		Renderer2D::resetStats();
		_framebuffers->bind();
		RendererCommand::setClearColor({ 0.1f, 0.1f, 0.1f, 1 });
		RendererCommand::clear();
		Ref<Texture2D> icon = _sceneState == SceneState::Edit ? _iconPlay : _iconStop;

		/* _framebuffers->clearColorAttachment(1, -1); */
			switch (_sceneState) {
			case SceneState::Edit:
				{
					_editorCamera.onUpdate(ts);

					_activeScene->onUpdateEditor(ts, _editorCamera);
				}
				break;
			case SceneState::Play:
				{
					_activeScene->onUpdateRuntime(ts);
					break;
				}

		}
		
		// glClearTexImage does not work in Mac, hence commented this out.
		/* _activeScene->onUpdateEditor(ts, _editorCamera); */
		
		// @note this gives us the cursor location.
		auto[mouseX, mouseY] = ImGui::GetMousePos();
		mouseX -= _viewportBound[0].x; // making top-left (0, 0), if subtracting x and y with mouse pos.
		mouseY -= _viewportBound[0].y;

		glm::vec2 viewportSize = _viewportBound[1] - _viewportBound[0];
		mouseY = viewportSize.y - mouseY; // This makes our bottom left (0, 0)
		int currentMouseX = (int)mouseX;
		int currentMouseY = (int)mouseY;
		
		// @note giving feedback the pixel of that vertex buffer.
		if(mouseX >= 0 and mouseY >= 0 and mouseX < (int)viewportSize.x and mouseY < (int)viewportSize.y){
			int pixel = _framebuffers->readPixel(1, currentMouseX, currentMouseY);
			coreLogInfo("Pixel Data is {}", pixel);

			// @note For some reason pixel gives values 1036831949 or either -1, so this is just a simple work around.
			// @note Should probably have a UUID's for this...
			if(pixel == -1 || pixel > 10000000){
				hoveredEntity = Entity();
			}
			else{
				hoveredEntity = Entity((entt::entity)pixel, _activeScene.get());
			}
		}

		_framebuffers->unbind();
	}

	void EditorLayer::onImguiRender(){
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
	
		if (ImGui::BeginMenuBar()){
			if (ImGui::BeginMenu("File")){
				
				if(ImGui::MenuItem("New", "Ctrl+N")){
					newScene();
				}

				ImGui::Separator();

				if(ImGui::MenuItem("Open", "Ctrl+O")){
					openScene();
				}
				
				ImGui::Separator();

				if(ImGui::MenuItem("Save as", "Ctrl+Shift+s")){
					saveAs();
				}
				
				ImGui::Separator();


				if(ImGui::MenuItem("Exit", "Ctrl+X")) Application::Get().close();

				ImGui::EndMenu();
			}
			ImGui::EndMenuBar();
		}
		
		// @note TODO: Probably adding panels to a list, in the cases that there will be multiple panels for the editor.
		_sceneHeirarchyPanel.onImguiRender();
		_contentBrowserPanel.onImguiRender();

		ImGui::Begin("Stats");
		
		std::string name = "None";
		if(hoveredEntity){
			name = hoveredEntity.getComponent<TagComponent>().tag;
		}

		ImGui::Text("Hovered Entity: %s", name.c_str());

		auto stats = Renderer2D::getStats();
		ImGui::Text("Renderer2D Stats");
		ImGui::Text("Draw Calls %d", stats.drawCalls);
		ImGui::Text("Quads: %d", stats.quadCount);
		ImGui::Text("Vertices: %d", stats.getTotalVertexCount());
		ImGui::Text("Indices: %d", stats.getTotalIndexCount());

		ImGui::End();
	
		// Starting the viewports
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{0, 0});
		ImGui::Begin("Viewport");
		

		// Checking if window is focused, then to not block incoming events
		_isViewportFocused = ImGui::IsWindowFocused(); // If viewport is focused then we don't want to block incoming events.
		_isViewportHovered = ImGui::IsWindowHovered();

		auto viewportMinRegion = ImGui::GetWindowContentRegionMin();
		auto viewportMaxRegion = ImGui::GetWindowContentRegionMax();
		// @note If tab bar is expanded, then the cursor will be expanded
		auto viewportOffset = ImGui::GetWindowPos();

		_viewportBound[0] = { viewportMinRegion.x + viewportOffset.x, viewportMinRegion.y + viewportOffset.y};
		_viewportBound[1] = { viewportMaxRegion.x + viewportOffset.x, viewportMaxRegion.y + viewportOffset.y};

		Application::Get().getImGuiLayer()->setBlockEvents(!_isViewportFocused && !_isViewportHovered); // if either out of focused or hovered

		ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
		_viewportSize = {_viewportSize.x, _viewportSize.y};
		// Assuming the viewPortPanelSize is this type.
		if(_viewportSize != *((glm::vec2 *)&viewportPanelSize) && viewportPanelSize.x > 0 && viewportPanelSize.y > 0){
			// Recreating the frame buffer.
			_framebuffers->resize((uint32_t)viewportPanelSize.x, (uint32_t)viewportPanelSize.y);
			_viewportSize = {viewportPanelSize.x, viewportPanelSize.y};
			/* _cameraController.onResize(_viewportSize.x, _viewportSize.y); */
		}

		// By passing this renderer ID, this gives us the ID of the texture that we want to render.
		uint32_t textureID = _framebuffers->getColorAttachmentRendererID(); // Getting color buffer from frame buffer
		ImGui::Image(reinterpret_cast<void *>(textureID), ImVec2{_viewportSize.x, _viewportSize.y}, ImVec2{0, 1}, ImVec2{1, 0});
		
		// @note This allows us to drop content browser items to this specific target od draggind/dropping
		if(ImGui::BeginDragDropTarget()){
			const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM");
			std::filesystem::path filepath = (const char*)payload->Data;

			openSceneTarget(&filepath);

			ImGui::EndDragDropTarget();
		}

		// Gizmos
		Entity selectedEntity = _sceneHeirarchyPanel.getSelectedEntity();
		
		// @note only wanting to select the gizmo only when selected an entity and the type isn't -1 (unknown)
		if(selectedEntity && _gizmoType != -1){
			ImGuizmo::SetOrthographic(false);
			ImGuizmo::SetDrawlist();
			
			// Drawing the gismo (and figuring out where the camera is.
			ImGuizmo::SetRect(_viewportBound[0].x, _viewportBound[0].y, _viewportBound[1].x - _viewportBound[0].x, _viewportBound[1].y - _viewportBound[0].y);

			// @note Getting the camera information
			// Editor Camera
			const glm::mat4& cameraProjection = _editorCamera.getProjection();
			glm::mat4 cameraView = _editorCamera.getViewMatrix();

			// Snapping
			bool isSnap = InputPoll::isKeyPressed(KeyCode::LeftControl);
			float snapValue = 0.5f;

			if(_gizmoType == ImGuizmo::OPERATION::ROTATE)
				snapValue = 45.0f;

			float snapValues[3] = {snapValue, snapValue, snapValue};
			
			// Enttiy Transform
			auto& tc = selectedEntity.getComponent<TransformComponent>();
			glm::mat4 transform = tc.getTransform();


			ImGuizmo::Manipulate(glm::value_ptr(cameraView), glm::value_ptr(cameraProjection), (ImGuizmo::OPERATION)_gizmoType, ImGuizmo::LOCAL, glm::value_ptr(transform), nullptr, isSnap ? snapValues : nullptr);
			
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

		ImGui::End();
		ImGui::PopStyleVar();
		
		// Calling UI stuff
		ui_toolBar();


		ImGui::End();

	}
	
	void EditorLayer::ui_toolBar(){
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 2)); // @note ImVec making button not touch bottom
		ImGui::PushStyleVar(ImGuiStyleVar_ItemInnerSpacing, ImVec2(0, 2));
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));

		auto& color = ImGui::GetStyle().Colors;
		auto& buttonHovered = color[ImGuiCol_ButtonHovered];
		auto& buttonActive = color[ImGuiCol_ButtonActive];
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(buttonHovered.x, buttonHovered.y, buttonHovered.z, 0.5f));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(buttonActive.x, buttonActive.y, buttonActive.z, 0.5f));

		// @note setting size dynamically
		/* float size = ImGui::GetWindowHeight() - 4.0f; */
		float size = 20.0f;
		// @note nullptr meaning not closing the toolbar (not having close button
		/* ImGui::Begin("##", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse); */
		ImGui::Begin("##toolbox");
		
		// @note checking to see what state we are in. (If playing/stopping)
		Ref<Texture2D> icon = _sceneState == SceneState::Edit ? _iconPlay : _iconStop;
		
		// @note GetWindowContentRegionMax().x is how much space is there for content (widgets)
		// @note 0.5f is the offset for padding.
		// @note takes button size and halves it and makes the offset the center of that tab. (centering  buttons)
		ImGui::SameLine((ImGui::GetWindowContentRegionMax().x * 0.5f) - (size * 0.5f));

		if(ImGui::ImageButton((ImTextureID)icon->getRendererID(), ImVec2{size, size}, ImVec2(0, 0), ImVec2(1, 1))){
			if(_sceneState == SceneState::Edit)
				onScenePlay();
			else if(_sceneState == SceneState::Play)
				onSceneStop();
		}
		
		ImGui::PopStyleVar(2);
		ImGui::PopStyleColor(3);
		ImGui::End();
	}

	void EditorLayer::onEvent(Event& e){
		/* _cameraController.onEvent(e); */
		_editorCamera.onEvent(e);

		// Enabling key bindings
		EventDispatcher dispatcher(e);

		dispatcher.Dispatch<KeyPressedEvent>(bind_function(this, &EditorLayer::onKeyPressed));
		dispatcher.Dispatch<MouseButtonPressedEvent>(bind_function(this, &EditorLayer::onMousePressed));
	}
	
	bool EditorLayer::onKeyPressed(KeyPressedEvent& e){
		// Only works with shortcuts
		if(e.GetRepeatCount() > 0)
			return false;
		bool control = InputPoll::isKeyPressed(KeyCode::LeftControl) || InputPoll::isKeyPressed(KeyCode::RightControl);
		bool shift = InputPoll::isKeyPressed(KeyCode::LeftShift) || InputPoll::isKeyPressed(KeyCode::RightShift);

		switch (e.GetKeyCode()) {
		case KeyCode::N:
		{
			  if(control){
				newScene();
			  }
		}
			  break;

		case KeyCode::O:
		{
			  if(control){
					openScene();
			  }
		}
			  break;

		case KeyCode::S:
		{
			if(control && shift){
				saveAs();
			}
		}
			break;
		case KeyCode::X:
			if(control){
				Application::Get().close();
			}
			break;
			// Gizmos controls
		case KeyCode::Q: // Selection
			_gizmoType = -1;
			break;
		case  KeyCode::W:
			_gizmoType = ImGuizmo::OPERATION::TRANSLATE;
			break;
		case KeyCode::E:
			_gizmoType = ImGuizmo::OPERATION::ROTATE;
			break;
		case KeyCode::R:
			_gizmoType = ImGuizmo::OPERATION::SCALE;
			break;
		default:
			break;
		}

		return false;
	}
	void EditorLayer::newScene(){
		_activeScene = CreateRef<Scene>();
		_activeScene->onViewportResize((uint32_t)_viewportSize.x, (uint32_t)_viewportSize.y);
		_sceneHeirarchyPanel.setContext(_activeScene);
	}

	void EditorLayer::openScene(){
		
		std::string filepath = FileDialogs::openFile("Game Engine (*.engine)\0*.engine\0");
		coreLogTrace("Trace #2 -- filepath = {0}\n", filepath);
		
		if(!filepath.empty()){
			_activeScene = CreateRef<Scene>();
			_activeScene->onViewportResize((uint32_t)_viewportSize.x, (uint32_t)_viewportSize.y);
			_sceneHeirarchyPanel.setContext(_activeScene);

			SceneSerializer serializer(_activeScene);
			serializer.deserialize(filepath);
		}
		/* _activeScene = CreateRef<Scene>(); */
		/* _activeScene->onViewportResize((uint32_t)_viewportSize.x, (uint32_t)_viewportSize.y); */
		/* _sceneHeirarchyPanel.setContext(_activeScene); */

		/* SceneSerializer serializer(_activeScene); */
		/* serializer.deserialize("assets/scene/3DGreenCubeWorks.engine"); */
	}
	
	void EditorLayer::openSceneTarget(std::filesystem::path* path){
		_activeScene = CreateRef<Scene>();
		_activeScene->onViewportResize((uint32_t)_viewportSize.x, (uint32_t)_viewportSize.y);
		_sceneHeirarchyPanel.setContext(_activeScene);

		SceneSerializer serializer(_activeScene);
		serializer.deserialize(path->string());
	}

	void EditorLayer::saveAs(){
		std::string filepath = FileDialogs::saveFile("Game Engine (*.engine)\0*.engine\0");
		
		if(!filepath.empty()){
			SceneSerializer serializer(_activeScene);
			serializer.serializer(filepath);
		}
	}
	
	bool EditorLayer::onMousePressed(MouseButtonPressedEvent& e){
		// @note Change entity that we are clicking (only if we are hovering over that entitiy specifically.
		// @note enabling mouse picking here
		if(e.GetMouseButton() == Mouse::ButtonLeft){
			
			if(_isViewportHovered && !ImGuizmo::IsOver() && !InputPoll::isKeyPressed(Key::LeftAlt))
				_sceneHeirarchyPanel.setSelectedEntity(hoveredEntity);
		}

		return false;
	}

	void EditorLayer::onScenePlay(){
		_sceneState = SceneState::Play;
		_activeScene->onRuntimeStart();
	}

	void EditorLayer::onSceneStop(){
		_sceneState = SceneState::Edit;
		_activeScene->onRuntimeStop();
	}
};

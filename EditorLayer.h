#pragma once
#include <Engine3D/Core/Layer.h>
#include <Engine3D/interfaces/VertexArray.h>
#include <Engine3D/interfaces/Shader.h>
#include <Engine3D/interfaces/Texture.h>
#include <Engine3D/interfaces/Framebuffer.h>
#include <Engine3D/Scene2D/Entity.h>
#include <Engine3D/Renderer2D/EditorCamera.h>
#include <Engine3D/Events/KeyEvent.h>
#include "Panels/SceneHeirarchyPanel.h"
#include "Panels/ContentBrowserPanel.h"

namespace Engine3D{

    class EditorLayer : public Layer{
    public:
		EditorLayer();
	
		virtual ~EditorLayer() = default;

		virtual void onAttach() override;
		virtual void onDetach() override;

		virtual void onUpdate(Timestep ts) override;

		virtual void onImguiRender() override;

		virtual void onEvent(Event& e) override;
	
	private:
		bool onKeyPressed(KeyPressedEvent& e);
		bool onMousePressed(MouseButtonPressedEvent& e);

		void newScene();
		void openScene();
		void openSceneTarget(std::filesystem::path* path);
		void saveAs();

	private:
		// @note playing/stopping/simulating scenes
		void onScenePlay();
		void onSceneStop();

	private:
		// @note ui-related function calls

		void ui_toolBar();

	private:
		/* OrthographicCameraController _cameraController; */
    private:
        Ref<VertexArray> _squareVertexArrays;
		Ref<Shader> _flatColorShader;
        bool _isViewportFocused = false;
		bool _isViewportHovered = false;

		Ref<Texture2D> _checkerboardTexture;


		Ref<FrameBuffer> _framebuffers;

		SceneHeirachyPanel _sceneHeirarchyPanel;
		ContentBrowserPanel _contentBrowserPanel;

		Ref<Scene> _activeScene;
		bool isPrimaryCamera = true;
		EditorCamera _editorCamera;

		Entity hoveredEntity; // Gives us the entity we are hovering over.
		
		/* uint32_t pixelHoveredValue=0; */

		int _gizmoType = -1; // @note this is going to be the type of operationt he gizmal is going to be.

		glm::vec2 _viewportSize = {0, 0};
		glm::vec2 _viewportBound[2];
		glm::vec4 _squareColor;
		
		enum class SceneState{
			Edit=0, Play=1
		};
		
		// @note Editor Resources
		Ref<Texture2D> _iconPlay;
		Ref<Texture2D> _iconStop;



		SceneState _sceneState = SceneState::Edit;
    };

};

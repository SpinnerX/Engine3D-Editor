#pragma once
#include <Engine3D/Core/Layer.h>
#include <Engine3D/Graphics/VertexArray.h>
#include <Engine3D/Graphics/Shader.h>
#include <Engine3D/Graphics/Texture.h>
#include <Engine3D/Graphics/Framebuffer.h>
#include <Engine3D/Event/KeyEvent.h>
#include "Panels/SceneHeirarchyPanel.h"
#include "Panels/ContentBrowserPanel.h"

namespace Engine3D{

    class EditorLayer : public Layer{
    public:
		EditorLayer();
	
		virtual ~EditorLayer() = default;

		virtual void OnAttach() override;
		virtual void OnDetach() override;

		virtual void OnUpdate(Timestep ts) override;

		virtual void OnUIRender() override;

		virtual void OnEvent(Event& e) override;
	
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
        Ref<VertexArray> _squareVertexArrays;
		Ref<Shader> _flatColorShader;
        bool _isViewportFocused = false;
		bool _isViewportHovered = false;

		Ref<Texture2D> _checkerboardTexture;


		Ref<FrameBuffer> _framebuffers;

		SceneHeirachyPanel _sceneHeirarchyPanel;
		ContentBrowserPanel _contentBrowserPanel;


		Ref<Scene> _activeScene; 
		//! @note  Differentiating what scenes we are editing and our runtime.
		Ref<Scene> editorScene, runtimeScene;


		bool isPrimaryCamera = true;
		EditorCamera _editorCamera;

		Entity hoveredEntity; // Gives us the entity we are hovering over.

		int _gizmoType = -1; // @note this is going to be the type of operationt he gizmal is going to be.

		glm::vec2 _viewportSize = {0, 0};
		glm::vec2 _viewportBound[2];
		glm::vec4 _squareColor;
		
		enum class SceneState{
			Edit=0, Play=1
		};
		
		// @note Editor Resources
		Ref<Texture2D> playIcon;
		Ref<Texture2D> stopIcon;



		SceneState sceneState = SceneState::Edit;
    };

};

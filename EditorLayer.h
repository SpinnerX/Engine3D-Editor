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

		void NewScene();
		void LoadScene();

		//! @note Opening new scene target
		void LoadScene(const std::filesystem::path& path);

		void SaveAs();

	private:
		// @note playing/stopping/simulating scenes
		void OnScenePlay();
		void OnSceneEdit();

    private:
        bool isViewportFocused = false;
		bool isViewportHovered = false;

		Ref<FrameBuffer> framebuffer;

		SceneHeirachyPanel sceneHeirarchyPanel;
		ContentBrowserPanel contentBrowserPanel;


		Ref<Scene> currentlyActiveScene;
		//! @note  Differentiating what scenes we are editing and our runtime.
		Ref<Scene> editorScene, runtimeScene;


		bool isPrimaryCamera = true;
		EditorCamera editorCamera;

		Entity hoveredEntity; // Gives us the entity we are hovering over.

		int gizmo_t = -1; // @note this is going to be the type of operationt he gizmal is going to be.

		glm::vec2 viewportSize = {0, 0};
		glm::vec2 viewportBound[2];
		
		enum class SceneState{
			Edit=0, Play=1
		};
		
		// @note Editor Resources
		Ref<Texture2D> playIcon;
		Ref<Texture2D> stopIcon;



		SceneState sceneState = SceneState::Edit;
    };

};

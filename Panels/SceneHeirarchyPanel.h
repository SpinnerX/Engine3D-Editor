#pragma once
#include <Engine3D/Core/core.h>
#include <Engine3D/Scene/Scene.h>
#include <Engine3D/Scene/Entity.h>

namespace Engine3D{
	/*
	 *
	 * @function setContext
	 * @note setting the scene context.
	 * @note setting up scene heirarchy panel.
	 *
	 * */
	class SceneHeirachyPanel{
	public:
		SceneHeirachyPanel() = default;
		SceneHeirachyPanel(const Ref<Scene>& scene);

		void setContext(const Ref<Scene>& scene);

		void onImguiRender();
		
		Entity getSelectedEntity() const { return _selectionContext; }
		
		void setSelectedEntity(Entity entity);

	private:
		void drawEntityNode(Entity entity);
		void drawComponents(Entity entity);

	private:
		Ref<Scene> _context;
		Entity _selectionContext;
	};
};

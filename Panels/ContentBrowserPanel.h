#pragma once
#include <filesystem>
#include <Engine3D/Graphics/Texture.h>

namespace Engine3D{
	/**
	 *
	 *
	 * @class ContentBrowserPanel
	 * @note Helps with handling browser content
	 * @note Used for displaying what our Resources directory looks from our UI Editor
	 *
	 * */

	class ContentBrowserPanel{
	public:
		ContentBrowserPanel();

		static std::string GetContentsID() { return contentBrowserDragDropTargetID; }

		void OnUIRender();
	private:
		static std::string contentBrowserDragDropTargetID;
		std::filesystem::path _currentDirectory;
		Ref<Texture2D> directoryIcon;
		Ref<Texture2D> fileIcon;
		Ref<Texture2D> backButtonTexture;
	};
};

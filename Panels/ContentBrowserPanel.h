#pragma once
#include <filesystem>
#include <Engine3D/Graphics/Texture.h>

namespace Engine3D{
	/**
	 *
	 *
	 * @class ContentBrowserPanel
	 * @note Helps with handling browser content
	 * @note Used for displaying what our assets directory looks from our UI Editor
	 *
	 * */

	class ContentBrowserPanel{
	public:
		ContentBrowserPanel();

		void OnUIRender();
	private:
		std::filesystem::path _currentDirectory;
		Ref<Texture2D> directoryIcon;
		Ref<Texture2D> fileIcon;
		Ref<Texture2D> backButtonTexture;
	};
};

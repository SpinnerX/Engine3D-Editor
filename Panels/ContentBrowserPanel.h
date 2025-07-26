#pragma once
#include <filesystem>
#include <Engine3D/Graphics/Texture.h>

namespace Engine3D{
	/*
	 *
	 *
	 * @class ContentBrowserPanel
	 * @note contents that we will be browsing
	 *
	 *
	 * */

	class ContentBrowserPanel{
	public:
		ContentBrowserPanel();

		void OnUIRender();
	private:
		std::filesystem::path _currentDirectory;
		Ref<Texture2D> _directoryIcon;
		Ref<Texture2D> _fileIcon;
	};
};

#pragma once
#include <filesystem>
#include <Engine3D/interfaces/Texture.h>

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

		void onImguiRender();
	private:
		std::filesystem::path _currentDirectory;
		Ref<Texture2D> _directoryIcon;
		Ref<Texture2D> _fileIcon;
	};
};

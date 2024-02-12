#pragma once
#include <filesystem>
#include <GameEngine/Renderer/Texture.h>

namespace RendererEngine{
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

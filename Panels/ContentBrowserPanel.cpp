#include <Engine3D/Engine3DPrecompiledHeader.h>
#include "ContentBrowserPanel.h"
#include <imgui/imgui.h>
#include <filesystem>
#include <Engine3D/Events/MouseEvent.h>
#include <Engine3D/Events/InputPoll.h>

namespace Engine3D{
	static const std::filesystem::path _assetPath = "assets";

	ContentBrowserPanel::ContentBrowserPanel() : _currentDirectory(_assetPath){
		_directoryIcon = Texture2D::Create("assets/icons/DirectoryIcon.png");
		_fileIcon = Texture2D::Create("assets/icons/FileIcon.png");
	}

	void ContentBrowserPanel::onImguiRender(){
		ImGui::Begin("Content Browser");
		
		if(_currentDirectory != std::filesystem::path(_assetPath)){
			if(ImGui::Button("<-")){
				_currentDirectory = _currentDirectory.parent_path();
			}
		}

		// Setting up content button properties
		static float padding = 16.0f;
		static float thumbnailSize = 128.0f;
		float cellSize = thumbnailSize + padding;

		float panelWidth = ImGui::GetContentRegionAvail().x;
		int columnCount = (int)(panelWidth / cellSize);

		if(columnCount < 1)
			columnCount = 1;

		ImGui::Columns(columnCount, 0, false);


		// @note First, list all files in directory
		//
		// @note TODO things
		// @note potentially have this be loaded once, by doing something like adding it in a list or std::vector<T>
		// @note then to iterate that list every frame.
		// @note OR could do it per second, to pickup new files (since if file do change)
		
		for(auto& directoryEntry : std::filesystem::directory_iterator(_assetPath)){
			const auto& path = directoryEntry.path(); // Absolute Path
			auto relativePath = std::filesystem::relative(path, _assetPath); 
			std::string filenameString = relativePath.filename().string();
			
			ImGui::PushID(filenameString.c_str());
			Ref<Texture2D> icon = directoryEntry.is_directory() ? _directoryIcon : _fileIcon;
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));

			ImGui::ImageButton(reinterpret_cast<void *>(icon->getRendererID()), {thumbnailSize, thumbnailSize}, { 0, 1 }, { 1, 0});
			
			if(ImGui::BeginDragDropSource()){
				/* const char* itemPath = relativePath.c_str(); */
				std::string itemPath = relativePath.string();
				// @note keep in mind sizeof(itemPath) is in bytes
				ImGui::SetDragDropPayload("CONTENT_BROWSER_ITEM", itemPath.c_str(), itemPath.size());
				ImGui::EndDragDropSource();
			}

			ImGui::PopStyleColor();

			if(ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)){
				if(directoryEntry.is_directory()){
					_currentDirectory /= path.filename();
				}
			}

			ImGui::TextWrapped("%s", filenameString.c_str());
			ImGui::NextColumn();

			ImGui::PopID();
		}

		ImGui::Columns(1);
		ImGui::SliderFloat("Thumnail Size", &thumbnailSize, 16, 512);
		ImGui::SliderFloat("Padding", &padding, 0, 32);
		
		ImGui::End();
	}
};

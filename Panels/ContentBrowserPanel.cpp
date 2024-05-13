#include <Engine3D/Engine3DPrecompiledHeader.h>
#include "ContentBrowserPanel.h"
#include <imgui/imgui.h>
#include <filesystem>
#include <Engine3D/Event/MouseEvent.h>
#include <Engine3D/Event/InputPoll.h>

namespace Engine3D{
	static const std::filesystem::path _assetPath = "assets";

	//! @note This fetches the filename in our filepath
	//! @note Example is if we have "RocketGameAssets/textures/Triangle.png"
	//! @note We should retrieve Triangle.png
	static std::string GetFilename(const std::string& path){
		std::string filename = "";
		auto lastSlash = path.find_last_of("/\\");
		lastSlash = (lastSlash == std::string::npos) ? 0 : lastSlash + 1;

		auto lastDot = path.rfind('.');
		auto count = (lastDot == std::string::npos) ? path.size() - lastSlash : lastDot - lastSlash;

		filename = path.substr(lastSlash, count);
		return filename;
	}

	ContentBrowserPanel::ContentBrowserPanel() : _currentDirectory(_assetPath){
		directoryIcon = Texture2D::Create("assets/icons/DirectoryIcon.png");
		fileIcon = Texture2D::Create("assets/icons/FileIcon.png");
		backButtonTexture = Texture2D::Create("assets/icons/Back.png");
	}

	void ContentBrowserPanel::OnUIRender(){
		ImGui::Begin("Content Browser");
		// Setting up content button properties
		static float padding = 16.0f;
		static float thumbnailSize = 128.0f;
		float cellSize = thumbnailSize + padding;

		float panelWidth = ImGui::GetContentRegionAvail().x;
		int columnCount = (int)(panelWidth / cellSize);

		if(columnCount < 1)
			columnCount = 1;

		ImGui::Columns(columnCount, 0, false);
		
		if(_currentDirectory != std::filesystem::path(_assetPath)){
			// if(ImGui::Button("<-")){
			// 	_currentDirectory = _currentDirectory.parent_path();
			// }

			if(ImGui::ImageButton((ImTextureID)backButtonTexture->GetRendererID(), {10.0f, 10.0f}, { 0, 1 }, { 1, 0})){
				_currentDirectory = _currentDirectory.parent_path();
			}
		}


		// @note First, list all files in directory
		//
		// @note TODO things
		// @note potentially have this be loaded once, by doing something like adding it in a list or std::vector<T>
		// @note then to iterate that list every frame.
		// @note OR could do it per second, to pickup new files (since if file do change)
		
		for(auto& directoryEntry : std::filesystem::directory_iterator(_currentDirectory)){
			const auto& path = directoryEntry.path(); // Absolute Path
			auto relativePath = std::filesystem::relative(path, _assetPath); 
			std::string filenameString = relativePath.filename().string();
			ImGui::PushID(filenameString.c_str());

			Ref<Texture2D> icon = directoryEntry.is_directory() ? directoryIcon : fileIcon;
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
			ImGui::ImageButton((ImTextureID)icon->GetRendererID(), {thumbnailSize, thumbnailSize}, { 0, 1 }, { 1, 0});
			

			//! @note Used for drag dropping a texture to our shapes
			//! @note Make sure that the corresponding content browser item id is the same when doing drag-drop-source
			if(ImGui::BeginDragDropSource()){
				// const wchar_t* itemPath = reinterpret_cast<const wchar_t *>(relativePath.c_str());
				std::string itemPath = relativePath.string();
				ImGui::SetDragDropPayload("CONTENT_BROWSER_ITEM", itemPath.data(), itemPath.size());
				// ImGui::SetDragDropPayload("CONTENT_BROWSER_ITEM", itemPath, (wcslen(itemPath) + 1) * sizeof(wchar_t));
				// std::string itemPath = relativePath.string();
				// std::string itemName = GetFilename(itemPath);
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

#pragma once
#include <string>
#include <imgui/imgui.h>

namespace Engine3D{

    class UI{
    public:
        template<typename UFunction>
        static void UI_CreateWidget(const std::string& id, const UFunction& function){
            // ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, padding);
            if(ImGui::Begin(id.c_str())){
                function();
                ImGui::End();
            }
        }


        template<typename UFunction>
        static void UI_CreateToolbar(const std::string& id, const UFunction& function){
            ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 2)); // @note ImVec making button not touch bottom
            ImGui::PushStyleVar(ImGuiStyleVar_ItemInnerSpacing, ImVec2(0, 2));
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));

            auto& color = ImGui::GetStyle().Colors;

            auto& buttonHovered = color[ImGuiCol_ButtonHovered];
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(buttonHovered.x, buttonHovered.y, buttonHovered.z, 0.5f));

            auto& buttonActive = color[ImGuiCol_ButtonActive];
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(buttonActive.x, buttonActive.y, buttonActive.z, 0.5f));

            // @note setting size dynamically
            /* float size = ImGui::GetWindowHeight() - 4.0f; */
            // float size = 20.0f;
            // @note nullptr meaning not closing the toolbar (not having close button
            /* ImGui::Begin("##", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse); */
            // ImGui::Begin("##toolbox", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
            if(ImGui::Begin(id.c_str(), nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse)){
                function();

                ImGui::PopStyleVar(2);
                ImGui::PopStyleColor(3);
                ImGui::End();
            }
        }

        template<typename UFunction>
        static void UI_CreateMenubar(const UFunction& function){
            if(ImGui::BeginMenuBar()){
                function();
                ImGui::EndMenuBar();
            }
        }

    private:
    };
};
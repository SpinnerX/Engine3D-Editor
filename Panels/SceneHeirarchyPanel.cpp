#include "SceneHeirarchyPanel.h"
#include <GameEngine/Scene/Entity.h>
#include <GameEngine/Scene/Scene.h>
#include <GameEngine/Scene/Components.h>
#include <glm/gtc/type_ptr.hpp>
#include <imgui/imgui_internal.h>
#include <GameEngine/Renderer/Texture.h>
namespace RendererEngine{
	extern const std::filesystem::path _assetPath;
	const std::filesystem::path _assetPath = "assets";

	// Since BeginPopupContextWindow(const char*, ImGuiMouseButton, bool) is obsolete in ImGui, just recreated that function call through here.
    static inline bool BeginPopupContextWindow(const char* str_id, ImGuiMouseButton mb, bool over_items) { return ImGui::BeginPopupContextWindow(str_id, mb | (over_items ? 0 : ImGuiPopupFlags_NoOpenOverItems)); }

	SceneHeirachyPanel::SceneHeirachyPanel(const Ref<Scene>& scene){
		setContext(scene);
	}

	void SceneHeirachyPanel::setContext(const Ref<Scene>& scene){
		_context = scene;
		
		// @note check for if the selected context is in active scene and when we set the context then we'll reset it.
		_selectionContext = {};
	}
	
	void SceneHeirachyPanel::setSelectedEntity(Entity entity){
		_selectionContext = entity;
	}

	void SceneHeirachyPanel::onImguiRender(){
		// @note Drawing this whole scene heirarchy.
		// @note Setting up ImGui
		ImGui::Begin("Scene Heirarchy");
		_context->_registry.each([&](auto entityID){
			Entity entity = {entityID, _context.get() };
			
			// @note Wanting to check for a valid entity (may not want to list all entities)
			// @note Whenever creating an entity you give the tag component to it.
			drawEntityNode(entity);
		});
		
		// @note Deselecting entities/window tabs nodes
		// @note Left-click on a non entity panel will deselect the selected panels
		if(ImGui::IsMouseDown(0) && ImGui::IsWindowHovered())
			_selectionContext = {};
		
		// @note right click on blank space
		// @param string_id
		// @param popup_flags - will be the mouse flag (0=right, 1=left)
		if(BeginPopupContextWindow(0, 1, false)){
			if(ImGui::MenuItem("Create Empty Entity"))
				_context->createEntity("Empty Entity");

			ImGui::EndPopup();
		}


		ImGui::End();

		ImGui::Begin("Properties");
		
		// Checking if there is any selected context scene is valid
		if(_selectionContext){
			drawComponents(_selectionContext);
		}

		ImGui::End();
	}
	
	void SceneHeirachyPanel::drawEntityNode(Entity entity){
		auto& tc = entity.getComponent<TagComponent>().tag;

		// @param entity - actual entity that we want to pass in
		// @param flags - Click on actual arrow to expand and select that entity.
		// @param tc.c_str() - Actual name of the tree node.
		// @note entities are all unique and will not have the same entity in the same panel
		ImGuiTreeNodeFlags flags = ((_selectionContext == entity) ? ImGuiTreeNodeFlags_Selected : 0) | ImGuiTreeNodeFlags_OpenOnArrow;
		flags |= ImGuiTreeNodeFlags_SpanAvailWidth;	
		bool opened = ImGui::TreeNodeEx((void *)(uint64_t)(uint32_t)entity, flags, tc.c_str());

		if(ImGui::IsItemClicked()){
			_selectionContext = entity;
		}
		
		// @note  popup button for deleting an entity.
		bool isEntityDeleted = false;

		// @param string_id
		// @param popup_flags - will be the mouse flag (0=right, 1=left)
		if(ImGui::BeginPopupContextItem()){
			if(ImGui::MenuItem("Delete Entity"))
				isEntityDeleted = true;

			ImGui::EndPopup();
		}

		// @note if child entities, recursively open those entities.
		if(opened){
			ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth;
			bool opened2 = ImGui::TreeNodeEx((void *)(uint64_t)((uint32_t)entity + 1000), flags, "%s", tc.c_str());
			if(opened2){
				ImGui::TreePop();
			}

			ImGui::TreePop();
		}

		/* ImGui::Text("%s", tc.tag.c_str()); */

		// @note deferring when an entity is deleted or else bad things will happen.
		if(isEntityDeleted){
			_context->destroyEntity(entity);
			if(_selectionContext == entity)
				_selectionContext = {};
		}

	}
	
	/*
	 *
	 * @param label - labeling of this specific ui.
	 * @param values - transform values
	 * @param resetVal - values to reset translation back to zero
	 * @param columnWidth - width of the labels.
	 * */
	static void drawVec3Control(const std::string& label, glm::vec3& values, float resetVal=0.0f, float columnWidth=100.0f){
		ImGuiIO& io = ImGui::GetIO();
		auto boldFont = io.Fonts->Fonts[0]; // @note getting default font from vector<ImFont*>

		ImGui::PushID(label.c_str()); // Meaning that specific group of ID's are pushed have a unique label

		ImGui::Columns(2); // two columns, one for the labels
		
		ImGui::SetColumnWidth(0, columnWidth);
		ImGui::Text("%s", label.c_str());
		ImGui::NextColumn();
		
		ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{0, 0});
		
		// This calculates the height 
		float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
		ImVec2 buttonSize = { lineHeight + 3.0f, lineHeight };

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{0.8, 0.1f, 0.15f, 1.0f});
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{0.9f, 0.2f, 0.2f, 1.0f});
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{0.8, 0.1f, 0.15f, 1.0f});

		// Setting up the buttons
		
		// Setting up for the Y button
		ImGui::PushFont(boldFont);
		if(ImGui::Button("X", buttonSize))
			values.x = resetVal;

		ImGui::PopFont();
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		ImGui::DragFloat("##X", &values.x, 0.1f, 0.0f, 0.0f, "%.2f");
		ImGui::PopItemWidth();
		ImGui::SameLine();


		// Setting up for the Y button
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{0.2, 0.7f, 0.2f, 1.0f});
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{0.3f, 0.8f, 0.3f, 1.0f});
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{0.2, 0.1f, 0.2f, 1.0f});

		ImGui::PushFont(boldFont);
		if(ImGui::Button("Y", buttonSize))
			values.y = resetVal;

		ImGui::PopFont();
		ImGui::PopStyleColor(3);
		ImGui::SameLine();
		ImGui::DragFloat("##Y", &values.y, 0.1f, 0.0f, 0.0f, "%.2f");
		ImGui::PopItemWidth();
		ImGui::SameLine();

		// Setting up for the Z button
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{0.1, 0.25f, 0.8f, 1.0f});
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{0.2f, 0.35f, 0.9f, 1.0f});
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{0.8, 0.1f, 0.15f, 1.0f});
		ImGui::PushFont(boldFont);
		if(ImGui::Button("Z", buttonSize))
			values.z = resetVal;

		ImGui::PopFont();
		ImGui::PopStyleColor(3);
		ImGui::SameLine();
		ImGui::DragFloat("##Z", &values.z, 0.1f, 0.0f, 0.0f, "%.2f");
		ImGui::PopItemWidth();
		
		ImGui::PopStyleVar();

		ImGui::Columns(1);

		ImGui::PopID();
	}
	
	/*
	 *
	 * @param T is the component type we are passing
	 * @param UIFucntion is the lambda function.
	 *
	 * */
	template<typename T, typename UIFunction>
	static void drawComponent(const std::string& name, Entity entity, UIFunction uiFunction){
		const ImGuiTreeNodeFlags treeNodeFlags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_AllowItemOverlap | ImGuiTreeNodeFlags_FramePadding;

		if(entity.hasComponent<T>()){
			auto& component = entity.getComponent<T>();
			ImVec2 contentRegion = ImGui::GetContentRegionAvail();

			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2{ 4, 4 });
			
			float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f; // @note calculating height of button
			ImGui::Separator();

			bool opened = ImGui::TreeNodeEx((void *)typeid(T).hash_code(), treeNodeFlags, "%s", name.c_str());
			ImGui::PopStyleVar();
			
			// @note seeing what the most available width are.
			// @note using line height takes account for the scroll bar.
			// @note if scrollbar appears then moving everthing to the left, so there are no overlapping with the scrollbar.
			ImGui::SameLine(contentRegion.x  - lineHeight * 0.5f);
			
			if(ImGui::Button("+", ImVec2{lineHeight, lineHeight})){
				ImGui::OpenPopup("ComponentSettings");
			}

			
			// @note contain menu items for the components.
			bool isRemovedComponent = false; // @note for deferring when to delete component.
			if(ImGui::BeginPopup("ComponentSettings")){
				if(ImGui::MenuItem("Remove Component"))
					isRemovedComponent = true;

				ImGui::EndPopup();
			}

			if(opened){
				uiFunction(component);
				ImGui::TreePop();
			}
			
			if(isRemovedComponent)
				entity.removeComponent<T>();
		}
		
	}
	
	void SceneHeirachyPanel::drawComponents(Entity entity){
		// checking if entity has components.
		if(entity.hasComponent<TagComponent>()){
			auto& tag = entity.getComponent<TagComponent>().tag;

			// @context Camera0
			char buffer[256];
			
			// To make null terminated.
			memset(buffer, 0, sizeof(buffer));
			strcpy(buffer, tag.c_str());

			if(ImGui::InputText("##Tag", buffer, sizeof(buffer))) {
				tag = std::string(buffer);
			}
		}

		ImGui::SameLine();
		ImGui::PushItemWidth(-1);
		if(ImGui::Button("Add Component"))
			ImGui::OpenPopup("Add Component");

		if(ImGui::BeginPopup("Add Component")){
			if(!_selectionContext.hasComponent<CameraComponent>()){
				if(ImGui::MenuItem("Camera")){
					_selectionContext.addComponent<CameraComponent>();
					ImGui::CloseCurrentPopup();
				}
			}
			
			if(!_selectionContext.hasComponent<SpriteRendererComponent>()){
				if(ImGui::MenuItem("Sprite Renderer")){
					_selectionContext.addComponent<SpriteRendererComponent>();
					ImGui::CloseCurrentPopup();
				}
			}
			
			if(!_selectionContext.hasComponent<RigidBody2DComponent>()){
				if(ImGui::MenuItem("Rigidbody 2D")){
					_selectionContext.addComponent<RigidBody2DComponent>();
					ImGui::CloseCurrentPopup();
				}
			}
			
			if(!_selectionContext.hasComponent<BoxCollider2DComponent>()){
				if(ImGui::MenuItem("Box Collider 2D")){
					_selectionContext.addComponent<BoxCollider2DComponent>();
					ImGui::CloseCurrentPopup();
				}
			}

			ImGui::EndPopup();
		}

		ImGui::PopItemWidth();


		
		// @params translation, rotation, scale.
		// @note handling those controls the transformation of entities.
		// @note TODO probably change the api call to the following below for entity and components retrieval.
		/*
		 drawComponent<CameraComponent>("Camera"), [](){
			auto& src = entity.getComponent<SpriteRendererComponent>();
			ImGui::ColorEdit4("Color", glm::value_ptr(src.color));
		 });
		 * */
		// @note setting up Transform Component
		drawComponent<TransformComponent>("Transform", entity, [](auto& component){
			// Creating our headers.
			glm::vec3 rotation = glm::degrees(component.rotation);
			drawVec3Control("Translation", component.translation);
			drawVec3Control("Rotation", rotation);
			component.rotation = glm::radians(rotation);
			drawVec3Control("Scale", component.scale, 1.0f);
		});

		// @note Setting up camera component
		drawComponent<CameraComponent>("Camera Component", entity, [](auto& component){
			auto& camera = component.camera;

			ImGui::Checkbox("Primary", &component.isPrimary);


			const char* projectionTypeStrings[] = {"Perspective", "Orthographic"};
			const char* currentProjectionTypeString = projectionTypeStrings[(int)camera.getProjectionType()];
			
			// @note if BeginCombo has started.
			if(ImGui::BeginCombo("Projection", currentProjectionTypeString)){
					
				// @note seeing what currently selected projection type is. 
				for(int i = 0; i < 2; i++){

					// @note handling if the projection type selected is valid and selected
					bool isSelected = (currentProjectionTypeString == projectionTypeStrings[i]);
					if(ImGui::Selectable(projectionTypeStrings[i], isSelected)){
						currentProjectionTypeString = projectionTypeStrings[i];
						camera.setProjectionType((SceneCamera::ProjectionType)i);
					}
						
					// @note checking if already selected then setting the default focus.
					if(isSelected){
						ImGui::SetItemDefaultFocus();
					}
				}

				ImGui::EndCombo();
			}

			// @note checking if the camera component is orthographic.
			if(component.camera.getProjectionType() == SceneCamera::ProjectionType::Perspective){
				float verticalFov = glm::degrees(camera.getPerspectiveVerticalFOV());

				if(ImGui::DragFloat("FOV", &verticalFov))
					camera.setPerspectiveVerticalFOV(glm::radians(verticalFov));

				float perspNearClip = camera.getPerspectiveNearClip();
				if(ImGui::DragFloat("Near", &perspNearClip))
					camera.setPerspectiveNearClip(perspNearClip);
				
				float perspFarClip = camera.getPerspectiveFarClip();
				if(ImGui::DragFloat("Far", &perspFarClip))
					camera.setPerspectiveFarClip(perspFarClip);
			}
			
			if(component.camera.getProjectionType() == SceneCamera::ProjectionType::Orthographic){
				float orthoSize = camera.getOrthographicSize();

				if(ImGui::DragFloat("Size", &orthoSize))
					camera.setOrthographicSize(orthoSize);

				float nearClip = camera.getOrthoNearClip();
				if(ImGui::DragFloat("Near", &nearClip))
					camera.setOrthoNearClip(nearClip);
				
				float farClip = camera.getOrthoFarClip();
				if(ImGui::DragFloat("Far", &farClip))
					camera.setOrthoNearClip(farClip);
				ImGui::Checkbox("Fixed Aspect Ratio", &component.fixedAspectRatio);
			}
		});
		
		// @note setting up Sprite Renderer Component.
		drawComponent<SpriteRendererComponent>("Sprite Renderer", entity, [](auto& component){
				ImGui::ColorEdit4("Color", glm::value_ptr(component.color));
				// Being able to drag in a texture 
				// @note using a button that is going to be the target for texture
				ImGui::Button("Texture", ImVec2(100.0f, 0.0f));

				// @note getting the texture here
				// @note by dragging/dropping those texture
				// @note for loading a texture via drag/drop
				if(ImGui::BeginDragDropTarget()){
					if(const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM")){
						const char* filepath = (const char*)payload->Data;
						std::filesystem::path texturePath = std::filesystem::path(_assetPath) / filepath;
						component.texture = Texture2D::Create(texturePath);
					}

					ImGui::EndDragDropTarget();
				}

				// @note Doing Texture stuff
				ImGui::DragFloat("Tiling Factor", &component.tilingFactor, 0.1f, 0.0f, 100.0f);
		});

		drawComponent<RigidBody2DComponent>("RigidBody 2D", entity, [](auto& component){
			coreLogWarn("Failure Detected Debug #1");
			const char* bodyTypeString[] = { "Static", "Dynamic", "Kinematic" };
			const char* currentBodyStringType = bodyTypeString[(int)component.type];
			coreLogWarn("Failure Detected Debug #2");

			if(ImGui::BeginCombo("Body Type", "Dynamic")){
				for(int i = 0; i < 2; i++){
					bool isSelected = currentBodyStringType == bodyTypeString[i];

					if(ImGui::Selectable(bodyTypeString[i], isSelected)){
						currentBodyStringType = bodyTypeString[i];
						component.type = (RigidBody2DComponent::BodyType)i;
					}

					if(isSelected)
						ImGui::SetItemDefaultFocus();

				}
				
				ImGui::EndCombo();
			}

			coreLogWarn("Failure Detected Debug #");

			ImGui::Checkbox("Fixed Rotation", &component.hasFixedRotation);
		});


		drawComponent<BoxCollider2DComponent>("Box Collider 2D", entity, [](auto& component){
			ImGui::DragFloat("Offset", glm::value_ptr(component.offset));
			ImGui::DragFloat("Size", glm::value_ptr(component.size));
			ImGui::DragFloat("Density", &component.density);
			ImGui::DragFloat("Friction", &component.friction, 0.01f, 0.0f, 1.0f);
			ImGui::DragFloat("Restitution", &component.restitution, 0.01f, 0.0f, 1.0f);
			ImGui::DragFloat("Restitution Threshold", &component.restitutionThreshold, 0.01f, 0.1f, 1.0f);
		});
	}
};

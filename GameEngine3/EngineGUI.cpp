#include "EngineGUI.h"
#include "LuaResource.h"
#include "Mesh.h"
#include "Scene.h"
#include "Engine/tooltips.h"
#include "AABB.h"
#include "TextGeometry.h"
#include "SoundInstance.h"
int EngineGUI::selectedInstanceId = -1;
std::string EngineGUI::selectedInstanceHash = "";


int EngineGUI::eyeDropperId = -1;
int EngineGUI::eyeDropperSpecificType = -1;
int EngineGUI::eyeDropperToUpdateId = -1;
int EngineGUI::eyeDropperReason = -1;
int EngineGUI::instanceAddParentId = -1;
int EngineGUI::currentSamplesAudioPlayback = 0;
int EngineGUI::maxSamplesAudioPlayback = 0;
int EngineGUI::currentSelectionDropdown = 0;
int EngineGUI::currentSelectionDropdown2 = 0;
pugi::xml_document EngineGUI::tooltipsXML;
bool EngineGUI::wasXMLSuccessful = false;
const int EngineGUI::EDITOR_ADD_INSTANCE[] = {
	Instance::InstanceType_Camera,
	Instance::InstanceType_Mesh,
	Instance::InstanceType_Collision3D,
	Instance::InstanceType_ShaderProgram,
	Instance::InstanceType_ShaderResource,
	Instance::InstanceType_ImageResource,
	Instance::InstanceType_Material,
	Instance::InstanceType_LuaResource,
	Instance::InstanceType_Geometry,
	Instance::InstanceType_TextGeometry,
	Instance::InstanceType_Audio
};

const std::string EngineGUI::placeholderNoInstance = "No instance selected";
char EngineGUI::search[];
bool EngineGUI::isModalAddInstanceOpen = false;
bool EngineGUI::newInstanceSelected	 = false;
bool EngineGUI::isInspectorFocused = false;
bool EngineGUI::isHierarchyFocused = false;
bool EngineGUI::fullyCancelEyeDropper = false;
bool EngineGUI::isInEyeDropperMode = false;
std::vector<std::string> EngineGUI::tooltipStrings;
#define TOOLTIPS_FILEPATH "core://tooltips/tooltips.xml"
std::string EngineGUI::getTooltipString(int type) {
	if (wasXMLSuccessful && type >= 0 && type < tooltipStrings.size()) {
		return tooltipStrings[type];
	}
	else {
		return "No tooltip available.";
	}
}
void EngineGUI::InitializeEngineGUI() {

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;

	ImGui::StyleColorsDark();

	ImGui_ImplGlfw_InitForOpenGL(Graphics::getInstance()->window, true);
	ImGui_ImplOpenGL3_Init("#version 330");

	pugi::xml_parse_result result = tooltipsXML.load_file(Resource::resolveFilePath(TOOLTIPS_FILEPATH).c_str());

	if (!result) {
		wasXMLSuccessful = false;
	}
	else {
		wasXMLSuccessful = true;

		for (pugi::xml_node node : tooltipsXML.child("root").children("tooltip")) {
			tooltipStrings.push_back(node.child("text").text().get());
		}
	}

}
void EngineGUI::DrawHierarchyNode(Instance* instance) {
	if (!instance) return;

	// 1. Configure visual flags
	ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth;

	if (selectedInstanceHash == instance->getEngineGUIHash()) { // Replace ->id with your actual ID getter
		flags |= ImGuiTreeNodeFlags_Selected;
	}

	// Check if this specific instance has children to look through
	bool has_children = !instance->children.empty();
	if (!has_children) {
		flags |= ImGuiTreeNodeFlags_Leaf;
	}

	// 2. Render the node (using ### to guarantee a unique hash via ID)
	std::string display_name = instance->getEngineGUIHash();
	bool is_open = ImGui::TreeNodeEx(display_name.c_str(), flags);

	if (ImGui::BeginPopupContextItem(display_name.c_str())) {
		//printf("Drawing for Instance %s\n", instance->name.c_str());
		if (ImGui::MenuItem("Clone")) {
			// Do action
			instance->clone();
		}
		if (ImGui::MenuItem("Delete")) {
			// Do action
			Instance::destroyQueue.insert(instance);
		}
		if (ImGui::MenuItem("Insert Object")) {
			isModalAddInstanceOpen = true;
			search[0] = '\0'; // Clear search text whenever they click add
			instanceAddParentId = instance->getId();
		}

		ImGui::EndPopup();
	}

	// 3. Handle selection input
	if (ImGui::IsItemClicked()) {

		if (isEyeDropping()) {
			eyeDropperId = instance->getId();
			EngineGUI::isInEyeDropperMode = false;
			debug_log("Eye dropped on ID #%d \"%s\"\n", eyeDropperId, instance->name.c_str());
		}
		else {
			selectedInstanceHash = instance->getEngineGUIHash();
			selectedInstanceId = instance->getId();
			newInstanceSelected = true;
			debug_log("Selected instance ID #%d \"%s\"\n", selectedInstanceId, instance->name.c_str());
		}
	}

	renderTooltipForItem(instance->getTypeTooltip());
	// 4. Cascade down if the node is expanded by the user
	if (is_open) {
		if (has_children) {
			for (Instance* child : instance->children) {
				// Recursively call the same function for the next layer down
				
				if (child->omitFromHierarchyRender()) {
					continue; // Skip this child if it's null or should be omitted
				}
				DrawHierarchyNode(child);
			}
		}
		ImGui::TreePop(); // Pop out of the current indentation layer
	}
}

void EngineGUI::DrawInspectorNode() {
	// Check if we have a valid selection string

	if (isEyeDropping()) {
		//ImGui::TextDisabled("Click on an object in the Hierarchy to select it as the target.");
	}
	if (!selectedInstanceHash.empty()) {

		// Safely look up the selected pointer in your global world map
		auto it = Instance::world.find(selectedInstanceId);

		if (it != Instance::world.end() && it->second) {
			Instance* selected_instance = it->second;

			// Pass the active instance to our property renderer function
			DrawInspectorPanel(selected_instance);

		}
		else {
			ImGui::TextDisabled("Selected object no longer exists in world.");
		}
	}
	else {
		ImGui::TextDisabled("Select an object from the Hierarchy to view properties.");
	}
}

void EngineGUI::DrawInspectorPanel(Instance* inst) {

	ImGui::InputText("Name", &inst->name);
	renderTooltipForItem(IDS_IATTR_NAME);
	std::string id = std::to_string(inst->getId());
	ImGui::InputText("ID", &id, ImGuiInputTextFlags_ReadOnly);
	renderTooltipForItem(IDS_IATTR_IID);
	std::string type = Instance::getInstanceNameFromType(inst->type);
	ImGui::InputText("Class Name", &type, ImGuiInputTextFlags_ReadOnly);
	renderTooltipForItem(IDS_IATTR_CLASSNAME);

	if (hasEyeDropCompleted(inst->getId())) {
		inst->setParent(Instance::getInstanceById(eyeDropperId));
		endEyeDropper(nullptr);

	}
	ImGui::InputText("Parent", placeholderName(inst->parent), ImGuiInputTextFlags_ReadOnly);
	if (ImGui::Button("Select Parent...")) {
		startEyeDropper(inst->getId());
	}
	if (Instance::isInstanceOf(inst->type, Instance::InstanceType_Mesh)) {
		Mesh* mesh = (Mesh*)inst;

		if (hasEyeDropCompleted(inst->getId(), 1)) {
			endEyeDropper(&mesh->material);
			mesh->VAODirty = true;
		}
		if (hasEyeDropCompleted(inst->getId(), 2)) {
			endEyeDropper(&mesh->geometry);
			mesh->VAODirty = true;
		}
		ImGui::InputFloat3("Position", (float*)&mesh->position);
		renderTooltipForItem(IDS_IATTR_POSITION);
		ImGui::InputFloat3("Rotation", (float*)&mesh->rotation);
		renderTooltipForItem(IDS_IATTR_ROTATION);
		ImGui::InputFloat3("Scale", (float*)&mesh->scale);
		renderTooltipForItem(IDS_IATTR_SCALE);
		Instance* meshMat = Instance::getInstanceById(mesh->material);

		ImGui::InputText("Material", placeholderName(meshMat), ImGuiInputTextFlags_ReadOnly);
		renderTooltipForItem(IDS_IATTR_MATERIAL);
		if (ImGui::Button("Select Material...")) {
			startEyeDropper(inst->getId(), 1);
			//eyeDropperSpecificType = Instance::InstanceType_Material;
		}

		if (inst->type == Instance::InstanceType_Mesh) {
			Instance* meshGeom = Instance::getInstanceById(mesh->geometry);
			ImGui::InputText("Geometry", placeholderName(meshGeom), ImGuiInputTextFlags_ReadOnly);
			renderTooltipForItem(IDS_IATTR_GEOMETRY);
			if (ImGui::Button("Select Geometry...")) {
				startEyeDropper(inst->getId(), 2);
				eyeDropperSpecificType = Instance::InstanceType_Geometry;
			}

		}
		if (inst->type == Instance::InstanceType_TextGeometry) {
			TextGeometry* m = (TextGeometry*)inst;
			if (newInstanceSelected) {
				auto it = TextGeometry::fontCharacterMaps.find(m->font);
				if (it != TextGeometry::fontCharacterMaps.end()){
					currentSelectionDropdown = std::distance(TextGeometry::fontCharacterMaps.begin(), it);
				}
				else {
					currentSelectionDropdown = -1;
				}
			}
			if (DrawDropdown("Font", TextGeometry::fontNames.data(), TextGeometry::fontNames.size()) != -1) {
				m->setFont(TextGeometry::fontNames[currentSelectionDropdown]);
			}

			if (ImGui::InputText("Text", &m->text)) {
				m->isDirty = true;
			}

		}
		
		ImGui::Checkbox("Backface Culling", &(mesh->cull));
		if (newInstanceSelected) {
			currentSelectionDropdown2 = mesh->renderLayer;
		}
		if (DrawDropdown("Render Layer", (std::string*)Mesh::renderLayerNames, eRenderLayer_MAX, &currentSelectionDropdown2) != -1) {
			mesh->renderLayer = currentSelectionDropdown2;
		}
		return;
	}

	if (inst->type == Instance::InstanceType_Camera) {
		Camera* m = (Camera*)inst;
		ImGui::InputFloat3("Position", (float*)&m->position);
		renderTooltipForItem(IDS_IATTR_POSITION);
		ImGui::InputFloat3("Rotation", (float*)&m->rotation);
		renderTooltipForItem(IDS_IATTR_ROTATION);
		ImGui::Checkbox("Orthographic", &m->isOrthographic);
		ImGui::SliderFloat("Field Of View", &m->fov, 0.0f, 180.0f, "FOV: %.3f deg");
	}
	if (inst->type == Instance::InstanceType_Scene) {
		Scene* m = (Scene*)inst;
		if (hasEyeDropCompleted(inst->getId(), 1)) {
			endEyeDropper(&m->hitboxShader);
		}
		if (hasEyeDropCompleted(inst->getId(), 2)) {
			endEyeDropper(&m->startCamera);
		}
		ImGui::InputText("Start Camera", placeholderName(Instance::getInstanceById(m->startCamera)), ImGuiInputTextFlags_ReadOnly);
		renderTooltipForItem(IDS_IATTR_GEOMETRY);
		if (ImGui::Button("Select Start Camera...")) {
			startEyeDropper(inst->getId(), 2);
			eyeDropperSpecificType = Instance::InstanceType_Camera;
		}
	}
	if (inst->type == Instance::InstanceType_Collision3D) {
		Collision3D* m = (Collision3D*)inst;
		vec3 position = m->getPosition();
		vec3 size = m->getSize();
		if (ImGui::InputFloat3("Position", (float*)&position)) {
			m->setPosition(position);
		};
		renderTooltipForItem(IDS_IATTR_POSITION);
		if (ImGui::InputFloat3("Size", (float*)&size)) {
			m->setSize(size);
		};
		renderTooltipForItem(IDS_IATTR_ROTATION);
	}
	if (inst->type == Instance::InstanceType_Material) {
		Material* m = (Material*)inst;

		for (int i = 0; i < eMaterialTexture_MAX; i++) {
			if (hasEyeDropCompleted(inst->getId(), i + 1)) {
				debug_log("Eye dropper completed for target %d\n", i);
				if (isEyeDropperValidAndOfType(Instance::InstanceType_ImageResource)) {
					m->setTextureById(eyeDropperId, i);
				}
				else {
					m->setTextureById(-1, i);
				}
				endEyeDropper(nullptr);
			}
		}

		if (hasEyeDropCompleted(inst->getId(), 5)) {
			debug_log("Eye dropper completed for shader\n");
			if (isEyeDropperValidAndOfType(Instance::InstanceType_ShaderProgram)) {
				m->setShader((ShaderProgram*)Instance::getInstanceById(eyeDropperId));
			}
			else {
				m->setShader(nullptr);
			}
			endEyeDropper(nullptr);
		}

		ImGui::ColorEdit3("Albedo Color", (float*)&m->diffuseColor);
		renderTooltipForItem(IDS_IATTR_POSITION);
		ImGui::Text("Texture Targets");

		for (int i = 0; i < eMaterialTexture_MAX; i++) {
			ImGui::Checkbox((std::string("Use ") + ImageResource::textureTypeNames[i + 1]).c_str(), &m->textureUseFlags[i]);
			renderTooltipForItem(IDS_IATTR_GEOMETRY);
		}
		ImGui::InputText("Albedo Texture", placeholderName(Instance::getInstanceById(m->getTextureInstanceId(eMaterialTexture_Diffuse))), ImGuiInputTextFlags_ReadOnly);
		renderTooltipForItem(IDS_IATTR_GEOMETRY);
		if (ImGui::Button("Select Albedo Texture...")) {
			startEyeDropper(inst->getId(), eMaterialTexture_Diffuse + 1, true);
		}
		ImGui::InputText("Normal Map", placeholderName(Instance::getInstanceById(m->getTextureInstanceId(eMaterialTexture_Normal))), ImGuiInputTextFlags_ReadOnly);
		renderTooltipForItem(IDS_IATTR_GEOMETRY);
		if (ImGui::Button("Select Normal Map...")) {
			startEyeDropper(inst->getId(), eMaterialTexture_Normal + 1, true);
		}
		ImGui::InputText("Specular Map", placeholderName(Instance::getInstanceById(m->getTextureInstanceId(eMaterialTexture_Specular))), ImGuiInputTextFlags_ReadOnly);
		renderTooltipForItem(IDS_IATTR_GEOMETRY);
		if (ImGui::Button("Select Specular Map...")) {
			startEyeDropper(inst->getId(), eMaterialTexture_Specular + 1, true);
		}
		ImGui::InputText("Ambient Occlusion Map", placeholderName(Instance::getInstanceById(m->getTextureInstanceId(eMaterialTexture_AmbientOcclusion))), ImGuiInputTextFlags_ReadOnly);
		renderTooltipForItem(IDS_IATTR_GEOMETRY);
		if (ImGui::Button("Select Ambient Occlusion Map...")) {
			startEyeDropper(inst->getId(), eMaterialTexture_AmbientOcclusion + 1, true);
		}
		ImGui::InputText("Shader", placeholderName(Instance::getInstanceById(m->getShaderInstanceId())), ImGuiInputTextFlags_ReadOnly);
		renderTooltipForItem(IDS_IATTR_GEOMETRY);
		if (ImGui::Button("Select Shader...")) {
			startEyeDropper(inst->getId(), 5, true);
		}
		if (ImGui::Button("Auto-Prepare Material")) {
			m->autoPrepare(true);
		};

		ImGui::Checkbox("Transparent", &(m->isTransparent));
	}

	if (Instance::isInstanceOf(inst->type, Instance::InstanceType_Resource)) {
		Resource* m = (Resource*)inst;
		if (ImGui::InputText("File Path", &m->filePath, ImGuiInputTextFlags_EnterReturnsTrue)) {
			if (inst->type == Instance::InstanceType_LuaResource) {
				LuaResource* luaRes = (LuaResource*)inst;
				luaRes->loadFromFile(m->filePath);
			}

			if (inst->type == Instance::InstanceType_ImageResource) {
				ImageResource* imgRes = (ImageResource*)inst;
				imgRes->loadFromFile(m->filePath);
			}
			if (inst->type == Instance::InstanceType_ShaderResource) {
				ShaderResource* imgRes = (ShaderResource*)inst;
				imgRes->loadFromFile(m->filePath.c_str());
			}
			if (inst->type == Instance::InstanceType_Geometry) {
				Geometry* imgRes = (Geometry*)inst;
				imgRes->loadFromFile(m->filePath.c_str());
			}
		}
		renderTooltipForItem(IDS_IATTR_FILEPATH);
		if (inst->type == Instance::InstanceType_ImageResource) {
			ImageResource* imgRes = (ImageResource*)inst;
			if (newInstanceSelected) {
				printf("New instance selected, resetting dropdown to %d\n", ((ImageResource*)inst)->textureType);
				currentSelectionDropdown = ((ImageResource*)inst)->textureType + 1;
			}

			if (DrawDropdown("Texture Port", (std::string*)ImageResource::textureTypeNames, 5) != -1) {
				imgRes->textureType = currentSelectionDropdown - 1;
				imgRes->loadFromFile(imgRes->filePath);
			}
		}

		if (inst->type == Instance::InstanceType_ShaderResource) {
			ShaderResource* m = (ShaderResource*)inst;
			if (newInstanceSelected) {
				printf("New instance selected, resetting dropdown to %d\n", ((ShaderResource*)inst)->shaderType);
				if (((ShaderResource*)inst)->shaderType == GL_VERTEX_SHADER) {
					currentSelectionDropdown = 0;
				}
				else if (((ShaderResource*)inst)->shaderType == GL_FRAGMENT_SHADER) {
					currentSelectionDropdown = 1;
				}
			}
			if (DrawDropdown("Shader Type", (std::string*)ShaderResource::shaderTypeNames, 2) != -1) {
				m->shaderType = ShaderResource::shaderTypeEnums[currentSelectionDropdown];
				m->loadFromFile(m->filePath.c_str());
			}
		}

		return;
	}

	if (inst->type == Instance::InstanceType_ShaderProgram) {
			if (ImGui::Button("Compile Shader...")) {
				ShaderProgram* m = (ShaderProgram*)inst;
				m->createProgram(true);
			}
			return;
	}

	if (inst->type == Instance::InstanceType_UIRootNode) {
		UIRootNode* m = (UIRootNode*)inst;

		if (hasEyeDropCompleted(inst->getId(), 1)) {
			endEyeDropper(&m->camera);
		}

		ImGui::InputText("Camera", placeholderName(Instance::getInstanceById(m->camera)), ImGuiInputTextFlags_ReadOnly);
		renderTooltipForItem(IDS_IATTR_GEOMETRY);
		if (ImGui::Button("Select Camera...")) {
			startEyeDropper(inst->getId(), 1);
			eyeDropperSpecificType = Instance::InstanceType_Camera;
		}
	}

	if (inst->type == Instance::InstanceType_Audio) {
		Audio* m = (Audio*)inst;

		if (newInstanceSelected) {
			currentSamplesAudioPlayback = m->getTimestampSamples();
			maxSamplesAudioPlayback = m->getMaxSamples();
		}
		if (hasEyeDropCompleted(inst->getId(), 1)) {
			endEyeDropper(&m->group);
		}
		if (ImGui::InputText("File Path", &m->audioName, ImGuiInputTextFlags_EnterReturnsTrue)) {
			Audio::loadSound(m->audioName);
			m->setAudioSource(m->audioName);
		}
		if (ImGui::Button("Play")) {
			m->play();
		}
		if (ImGui::Button("Stop")) {
			m->stop();
		}
		ImGui::PushItemWidth(-1.0f);
		if (ImGui::SliderInt("##AudioSeeker", &currentSamplesAudioPlayback, 0, m->getMaxSamples(), "")) {
			// Handle manual dragging adjustments here
		}
		ImGui::PopItemWidth();

		if (ImGui::IsItemDeactivatedAfterEdit()) {
    // The user just let go of the mouse! 
			m->setTimestampSamples(currentSamplesAudioPlayback);
		}
// Keep your standard audio engine safety check
		if (!ImGui::IsItemActive()) {
			currentSamplesAudioPlayback = m->getTimestampSamples();
		}

		char timeOverlay[32];
snprintf(timeOverlay, sizeof(timeOverlay), "%02d:%02d / %02d:%02d", 
         (int)floor(m->getTimestampSeconds()) / 60, (int)floor(m->getTimestampSeconds()) % 60, 
         (int)floor(m->getMaxSeconds()) / 60, (int)floor(m->getMaxSeconds()) % 60);
// 4. Position and render the text directly below
// You can use ImGui::SameLine() variations if you want to align it to the right, 
// but by default, this prints left-aligned on the very next line:
ImGui::Text("%s", timeOverlay);

		ImGui::InputText("Sound Group", placeholderName(Instance::getInstanceById(m->group)), ImGuiInputTextFlags_ReadOnly);
		renderTooltipForItem(IDS_IATTR_GEOMETRY);
		if (ImGui::Button("Select Sound Group...")) {
			startEyeDropper(inst->getId(), 1);
		}


	}
}

void EngineGUI::startEyeDropper(int ID, int reason, bool fullyCancel) {
	eyeDropperId = -1;
	eyeDropperToUpdateId = ID;
	eyeDropperReason = reason;
	fullyCancelEyeDropper = fullyCancel;
	isInEyeDropperMode = true;
	eyeDropperSpecificType = -1;
}

bool EngineGUI::isEyeDropping() {
	return isInEyeDropperMode;
}

void EngineGUI::endEyeDropper(int* port) {
	if (port && isEyeDropperValidAndOfType(eyeDropperSpecificType)) {
		*port = eyeDropperId;
	}
	eyeDropperId = -1;
	eyeDropperToUpdateId = -1;
	eyeDropperReason = -1;
	isInEyeDropperMode = false;
	fullyCancelEyeDropper = true;
}

bool EngineGUI::hasEyeDropCompleted(int id, int reason) {
	return eyeDropperToUpdateId == id && !isEyeDropping() && eyeDropperReason == reason;
}

std::string* EngineGUI::placeholderName(Instance* inst) {
	if (inst) {
		return &inst->name;
	}
	else {
		return (std::string*)&placeholderNoInstance;
	}
}

void EngineGUI::DrawRightClickVoidContextMenuNode() {
	if (ImGui::BeginPopupContextVoid("Edit")) {
		ImGui::EndPopup();
	}
}

void EngineGUI::DrawAddInstanceModalNode() {
	
	if (!isModalAddInstanceOpen) return;
		ImGui::OpenPopup("Add Instance Type");
		ImGui::SetNextWindowSize(ImVec2(300, 400), ImGuiCond_FirstUseEver);
		if (ImGui::BeginPopupModal("Add Instance Type", &isModalAddInstanceOpen, ImGuiWindowFlags_NoResize)) {
			//printf("e\n");
			// 5. The Search Bar inside the popup
			ImGui::Text("Filter:");
			ImGui::SameLine();
			ImGui::InputTextWithHint("##InstanceSearch", "Type to filter...", search, IM_ARRAYSIZE(search));

			ImGui::Separator();

			// 6. Scrollable child region for the matching results
			if (ImGui::BeginChild("TypeList", ImVec2(0, -ImGui::GetFrameHeightWithSpacing()))) {
				//printf("b\n");
				for (const int type : EDITOR_ADD_INSTANCE) {

					// Basic case-insensitive conversion helper
					auto matches_search = [](const std::string& str, const std::string& query) {
						if (query.empty()) return true;
						auto it = std::search(str.begin(), str.end(), query.begin(), query.end(),
							[](char a, char b) { return std::tolower(a) == std::tolower(b); });
						return it != str.end();
						};

					// Filter the results
					if (!matches_search(Instance::getInstanceNameFromType(type), search)) {
						continue;
					}

					// Render matching types as selectable items
					if (ImGui::Selectable(Instance::getInstanceNameFromType(type).c_str())) {
						
						newInstanceFromType(type, instanceAddParentId);

						isModalAddInstanceOpen = false; // Close the window on selection
						instanceAddParentId = -1;
						ImGui::CloseCurrentPopup();
					}
				}
				ImGui::EndChild();
			}

			ImGui::Separator();

			// 7. Cancel button at the bottom
			if (ImGui::Button("Cancel", ImVec2(120, 0))) {
				isModalAddInstanceOpen = false;
				instanceAddParentId = -1;
				ImGui::CloseCurrentPopup();
			}
			ImGui::EndPopup();
		}
}

Instance* EngineGUI::newInstanceFromType(int type, int p) {
	Instance* i = Scene::newInstanceFromType(type);
	if (i && p) {
		i->setParent(Instance::getInstanceById(p)); 
	}

	return i;
}

void EngineGUI::renderTooltipForItem(int tooltipType) {
	if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayNormal)) {
		ImGui::BeginTooltip();
		ImGui::TextUnformatted(getTooltipString(tooltipType).c_str());
		ImGui::EndTooltip();
	}
}

int EngineGUI::DrawDropdown(std::string name, std::string options[], int numChoices, int* selection) {
	int selectionDropdown = -1;
	std::string previewText = "";
	if (*selection != -1) {
		previewText = options[*selection];
	}
	if (ImGui::BeginCombo(name.c_str(), previewText.c_str())) {

		if (ImGui::IsWindowAppearing()) {
			*selection = 0;
		}
		if (numChoices == 0) {
			numChoices = IM_ARRAYSIZE(options);
		}
		for (int n = 0; n < numChoices; n++) {
			bool is_selected = (*selection == n);
			if (ImGui::Selectable(options[n].c_str(), is_selected)) {
				*selection = n;
				selectionDropdown = n;
			}
			if (is_selected) {
				ImGui::SetItemDefaultFocus();
			}
		}
		ImGui::EndCombo();
	}

	return selectionDropdown;
}

bool EngineGUI::isEyeDropperValidAndOfType(int type) {
	Instance* i = Instance::getInstanceById(eyeDropperId);
	if (i) {
		return Instance::isInstanceOf(i->type, type) || type == -1;
	} else {
		return false;
	}
}
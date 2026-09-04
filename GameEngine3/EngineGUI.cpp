#include "EngineGUI.h"
#include "LuaResource.h"
#include "Mesh.h"
#include "Scene.h"
#include "Engine/tooltips.h"
#include "AABB.h"
int EngineGUI::selectedInstanceId = -1;
std::string EngineGUI::selectedInstanceHash = "";


int EngineGUI::eyeDropperId = -1;
int EngineGUI::eyeDropperToUpdateId = -1;
int EngineGUI::eyeDropperReason = -1;
int EngineGUI::instanceAddParentId = -1;
int EngineGUI::currentSelectionDropdown = 0;
pugi::xml_document EngineGUI::tooltipsXML;
bool EngineGUI::wasXMLSuccessful = false;
const int EngineGUI::EDITOR_ADD_INSTANCE[] = {
	Instance::InstanceType_Camera,
	Instance::InstanceType_Mesh,
	Instance::InstanceType_Collision3D
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
			printf("Eye dropped on ID #%d \"%s\"\n", eyeDropperId, instance->name.c_str());
		}
		else {
			selectedInstanceHash = instance->getEngineGUIHash();
			selectedInstanceId = instance->getId();
			newInstanceSelected = true;
			printf("Selected instance ID #%d \"%s\"\n", selectedInstanceId, instance->name.c_str());
		}
	}

	renderTooltipForItem(instance->getTypeTooltip());
	// 4. Cascade down if the node is expanded by the user
	if (is_open) {
		if (has_children) {
			for (Instance* child : instance->children) {
				// Recursively call the same function for the next layer down
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
		endEyeDropper();
	}
	ImGui::InputText("Parent", placeholderName(inst->parent), ImGuiInputTextFlags_ReadOnly);
	if (ImGui::Button("Select Parent...")) {
		startEyeDropper(inst->getId());
	}
	if (inst->type == Instance::InstanceType_Mesh) {
		Mesh* mesh = (Mesh*)inst;

		if (hasEyeDropCompleted(inst->getId(), 1)) {
			mesh->material = eyeDropperId;
			endEyeDropper();
		}
		if (hasEyeDropCompleted(inst->getId(), 2)) {
			mesh->geometry = eyeDropperId;
			endEyeDropper();
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
		}
		Instance* meshGeom = Instance::getInstanceById(mesh->geometry);
		ImGui::InputText("Geometry", placeholderName(meshGeom), ImGuiInputTextFlags_ReadOnly);
		renderTooltipForItem(IDS_IATTR_GEOMETRY);
		if (ImGui::Button("Select Geometry...")) {
			startEyeDropper(inst->getId(), 2);
		}
		return;
	}

	if (inst->type == Instance::InstanceType_Camera) {
		Camera* m = (Camera*)inst;
		ImGui::InputFloat3("Position", (float*)&m->position);
		renderTooltipForItem(IDS_IATTR_POSITION);
		ImGui::InputFloat3("Rotation", (float*)&m->rotation);
		renderTooltipForItem(IDS_IATTR_ROTATION);
	}
	if (inst->type == Instance::InstanceType_Scene) {
		Scene* m = (Scene*)inst;
		if (hasEyeDropCompleted(inst->getId(), 1)) {
			m->hitboxShader = eyeDropperId;
			endEyeDropper();
		}
		ImGui::InputText("Hitbox Shader", placeholderName(Instance::getInstanceById(m->hitboxShader)), ImGuiInputTextFlags_ReadOnly);
		renderTooltipForItem(IDS_IATTR_GEOMETRY);
		if (ImGui::Button("Select Hitbox Shader...")) {
			startEyeDropper(inst->getId(), 1);
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
				printf("Eye dropper completed for target %d\n", i);
				m->setTextureById(eyeDropperId, i);
				endEyeDropper();
			}
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
			startEyeDropper(inst->getId(), 1, true);
		}
		ImGui::InputText("Normal Map", placeholderName(Instance::getInstanceById(m->getTextureInstanceId(eMaterialTexture_Normal))), ImGuiInputTextFlags_ReadOnly);
		renderTooltipForItem(IDS_IATTR_GEOMETRY);
		if (ImGui::Button("Select Normal Map...")) {
			startEyeDropper(inst->getId(), 2, true);
		}
		ImGui::InputText("Specular Map", placeholderName(Instance::getInstanceById(m->getTextureInstanceId(eMaterialTexture_Specular))), ImGuiInputTextFlags_ReadOnly);
		renderTooltipForItem(IDS_IATTR_GEOMETRY);
		if (ImGui::Button("Select Specular Map...")) {
			startEyeDropper(inst->getId(), 3, true);
		}
		ImGui::InputText("Ambient Occlusion Map", placeholderName(Instance::getInstanceById(m->getTextureInstanceId(eMaterialTexture_AmbientOcclusion))), ImGuiInputTextFlags_ReadOnly);
		renderTooltipForItem(IDS_IATTR_GEOMETRY);
		if (ImGui::Button("Select Ambient Occlusion Map...")) {
			startEyeDropper(inst->getId(), 4, true);
		}

		if (ImGui::Button("Auto-Prepare Material")) {
			m->autoPrepare(true);
		};
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
		}
		renderTooltipForItem(IDS_IATTR_FILEPATH);
		if (inst->type == Instance::InstanceType_ImageResource) {
			ImageResource* imgRes = (ImageResource*)inst;
			if (newInstanceSelected) {
				printf("New instance selected, resetting dropdown to %d\n", ((ImageResource*)inst)->textureType);
				currentSelectionDropdown = ((ImageResource*)inst)->textureType + 1;
			}

			if (DrawDropdown("ImageTextureLocation", (std::string*)ImageResource::textureTypeNames, 5) != -1) {
				imgRes->textureType = currentSelectionDropdown - 1;
				imgRes->loadFromFile(imgRes->filePath);
			}
		}

		return;
	}

	if (inst->type == Instance::InstanceType_ShaderProgram) {
			if (ImGui::Button("Compile Shader...")) {
				ShaderProgram* m = (ShaderProgram*)inst;
				m->createProgram();
			}
			return;
	}
}

void EngineGUI::startEyeDropper(int ID, int reason, bool fullyCancel) {
	eyeDropperId = -1;
	eyeDropperToUpdateId = ID;
	eyeDropperReason = reason;
	fullyCancelEyeDropper = fullyCancel;
	isInEyeDropperMode = true;
}

bool EngineGUI::isEyeDropping() {
	return isInEyeDropperMode;
}

void EngineGUI::endEyeDropper() {
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
	Instance* i = nullptr;
	if (type == Instance::InstanceType_Camera) {
		i = new Camera();
	}
	else if (type == Instance::InstanceType_Mesh) {
		i = new Mesh();
	} 
	else if (type == Instance::InstanceType_Collision3D) {
		i = new Collision3D();
	} 
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
	if (ImGui::BeginCombo(name.c_str(), options[*selection].c_str())) {

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
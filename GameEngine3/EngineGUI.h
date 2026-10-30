#pragma once

#include <IMGUI/imgui.h>
#include <IMGUI/imgui_impl_glfw.h>
#include <IMGUI/imgui_impl_opengl3.h>
#include <IMGUI/imgui_stdlib.h>
#include "Graphics/Graphics.h"

#include "GameInstance.h"
#include <PUGIXML/pugixml.hpp>
class EngineGUI {

	static pugi::xml_document tooltipsXML;
	static bool wasXMLSuccessful;
	static const int EDITOR_ADD_INSTANCE[11];
	static void DrawInspectorPanel(Instance* inst);
	static std::string selectedInstanceHash;
	static std::vector<std::string> tooltipStrings;
	static int selectedInstanceId;
	//The instance selected by the other instance
	static int eyeDropperReason;
	//Some (most) eye droppers will only allow instances of a specific type (e.g. Material only allows ShaderProgram for shader port)
	static int eyeDropperSpecificType;
	//The instance calling for the eye drop
	static int eyeDropperToUpdateId;
	static int instanceAddParentId;
	//Whether we cancel the eye dropper or remove the reference;
	static char search[128];
	static bool isModalAddInstanceOpen;

	static int maxSamplesAudioPlayback;
	static int currentSamplesAudioPlayback;
	static const std::string placeholderNoInstance;

	static bool hasEyeDropCompleted(int id, int reason = 0);
	static std::string* placeholderName(Instance* inst);
	static Instance* newInstanceFromType(int type, int p = -1);
	static std::string getTooltipString(int type);
	static void renderTooltipForItem(int tooltipType);
	static int currentSelectionDropdown;
	static int currentSelectionDropdown2;
	static int DrawDropdown(std::string name, std::string options[], int numChoices = 0, int* selection = &currentSelectionDropdown);
public:
	static int eyeDropperId;
	static bool fullyCancelEyeDropper;
	static bool isInEyeDropperMode;
	static void startEyeDropper(int id, int reason = 0, bool fullyCancel = true);
	static void endEyeDropper(int* port);
	static bool isEyeDropperValidAndOfType(int type = Instance::InstanceType_Base);
	static bool isEyeDropping();
	static bool isInspectorFocused;
	static bool isHierarchyFocused;
	static bool newInstanceSelected;
	static void InitializeEngineGUI();
	static void DrawAddInstanceModalNode();
	static void DrawHierarchyNode(Instance* inst);
	static void DrawInspectorNode();
	static void DrawRightClickVoidContextMenuNode();

};
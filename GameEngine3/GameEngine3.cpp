#include "GameInstance.h"
#include "Mesh.h"
#include "Camera.h"
#include "Shader.h"
#include "TextureResource.h"
#include "Scene.h"
#include "Material.h"
#include "LuaResource.h"
#include "EngineGUI.h"
#include "TextGeometry.h"
#include "SoundInstance.h"
int main() {

	Instance::registerHierarchy();
	//GRAPHICS - get the graphics instance and initialize
	Graphics* graphics = Graphics::getInstance();

	graphics->initializeWindow(800, 600, "Game Engine 3");
	graphics->matchViewportToWindow = true;
	
	LuaResource::InitializeLua();

	EngineGUI::InitializeEngineGUI();
	Audio::InitializeAudioEngine();
    if (FT_Init_FreeType(&Font::ft)) {
        std::cerr << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;
        return -1;
    }


#ifndef STUDIO_EDITOR
	Scene::loadSceneFromFile("scene.bin");
	Scene* scene = Scene::getCurrentScene();
	scene->currentCamera = scene->startCamera;
	debug_log("Loaded scene '%s' (ID %d) from file 'scene.bin'. Starting with camera ID %d.\n", scene->name.c_str(), scene->getId(), scene->currentCamera);
	scene->startCamera = -1;
#else
	Scene* scene = new Scene();
	Camera* camera = new Camera(vec3(0, 0, -2));
	camera->fov = 70.0f;
	camera->isEditorCamera = true;
	scene->currentCamera = camera->getId();
	
	UIRootNode* uir = new UIRootNode();
	uir->setParent(scene);
	scene->uiRoot = uir->getId();
	Folder* resources = new Folder();
	resources->name = "Resources";
	resources->setParent(scene);

	ShaderProgram* debug_shader = new ShaderProgram("core://shader/debug_vertex.glsl", "core://shader/debug_fragment.glsl");
	debug_shader->createProgram();
	debug_shader->setParent(resources);
	debug_shader->name = "DebugShader";
	scene->hitboxShader = debug_shader->getId();
	ShaderProgram* core_shader = new ShaderProgram("core://shader/core_vertex.glsl", "core://shader/core_fragment.glsl");
	core_shader->createProgram();
	core_shader->setParent(resources);
	core_shader->name = "MainShader";
	LuaResource* script = new LuaResource("core://scripts/program.lua");
	script->isEngineScript = true;
	script->loadScript();

	TextGeometry::loadFont("res://fonts/DroidSans.ttf");

#endif

	//Scene::loadSceneFromFile("scene.bin");

	//Scene::saveSceneToFile("scene.bin");
	//CAMERA - define camera position, planes, and FOV
	//LOOP


	while (graphics->shouldOpenWindow()) {

		if ((glfwGetTime() - graphics->lastFrameTime) < (1.0 / (float)graphics->targetFPS)) continue;
		Scene::getCurrentScene()->PreRender();

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

#ifdef _DEBUG
		{
			ImGui::Begin("Scene Hierarchy");
			
			EngineGUI::newInstanceSelected = false;
			EngineGUI::isHierarchyFocused = ImGui::IsWindowFocused();
			EngineGUI::DrawHierarchyNode(scene->getCurrentScene());
			
			ImGui::End();
		}
#endif
#ifdef STUDIO_EDITOR

		{
			ImGui::Begin("File Settings");

			if (ImGui::Button("Save Scene As...")) {
				//Add a file dialog here to select the scene file to save
				Scene::saveSceneToFile("scene.bin");
			}

			if (ImGui::Button("Load Scene From ...")) {
				//Add a file dialog here to select the scene file to load

				Scene::getCurrentScene()->destroy();
				Scene::loadSceneFromFile("scene.bin");
				if (script) {
					script->loadScript();
				}
			}
			ImGui::End();
		}
					// Loop through all instances in your world map/vector

		{
			ImGui::Begin("Inspector");
			EngineGUI::isInspectorFocused = ImGui::IsWindowFocused();
			EngineGUI::DrawInspectorNode();

			ImGui::End();
		}

		if (EngineGUI::isEyeDropping() && ImGui::IsMouseClicked(ImGuiMouseButton_Left) && !EngineGUI::isInspectorFocused && !EngineGUI::isHierarchyFocused) {
			if (!EngineGUI::fullyCancelEyeDropper) {
				EngineGUI::eyeDropperId = -1;
			}

			EngineGUI::isInEyeDropperMode = false;
			printf("Eye dropper cancelled.\n");
		}

		EngineGUI::DrawRightClickVoidContextMenuNode();
		EngineGUI::DrawAddInstanceModalNode();
#endif
		ImGui::Render(); 
		for (auto i : Instance::world) {
			if (i.second) {
				i.second->update(graphics->deltaTime, Scene::isStudio);
			}
		}
		glBindVertexArray(0);

		Scene::getCurrentScene()->RenderScene();

		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		Graphics::getInstance()->windowUpdate();
	}


	glfwTerminate();
	TextGeometry::destroyFreeType();
	Scene::getCurrentScene()->destroy();
	return 0;

}
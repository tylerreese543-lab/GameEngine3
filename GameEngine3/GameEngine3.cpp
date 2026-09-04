#include "GameInstance.h"
#include "Mesh.h"
#include "Camera.h"
#include "Shader.h"
#include "TextureResource.h"
#include "Scene.h"
#include "Material.h"
#include "LuaResource.h"

#include "EngineGUI.h"
int main() {

	Instance::registerHierarchy();
	//GRAPHICS - get the graphics instance and initialize
	Graphics* graphics = Graphics::getInstance();

	Scene* scene = new Scene();
	graphics->initializeWindow(800, 600, "Game Engine 3");
	graphics->matchViewportToWindow = true;
	
	scene->isStudio = true;
	LuaResource::InitializeLua();

	EngineGUI::InitializeEngineGUI();
	Camera* camera = new Camera(vec3(0, 0, -2));
	camera->fov = 70.0f;
	scene->addChild(camera);

	Instance* resources = new Instance();
	resources->name = "Resources";
	resources->setParent(scene);

	//SHADERS - load from file, parent to shader program and create;
	ShaderProgram* sprog = new ShaderProgram("res://shader/vertex_shader.glsl", "res://shader/fragment_shader.glsl");
	sprog->createProgram();
	sprog->setParent(resources);

	ShaderProgram* sprog2 = new ShaderProgram("res://shader/vs_hitbox.glsl", "res://shader/fs_hitbox.glsl");
	sprog2->createProgram();
	sprog2->setParent(resources);
	sprog2->name = "HitboxShader";
	scene->hitboxShader = sprog2->getId();
	//IMAGES - load from file
	ImageResource* texture0 = new ImageResource(eMaterialTexture_Diffuse, "res://textures/poinsettia.png");
	ImageResource* texture1 = new ImageResource(eMaterialTexture_AmbientOcclusion, "res://textures/suz_ao.png");
	texture0->name = "DiffuseTex";
	texture1->name = "AOTex";
	//MATERIAL - parent images/shader to material, set color
	Material* material = new Material();
	material->addChild(texture0);
	material->addChild(texture1);
	material->setShader(sprog);
	material->autoPrepare(true);
	material->name = "Materiala";
	material->diffuseColor = vec3(0.4, 0.6, 0.9);
	Material* material2 = (Material*)material->clone();
	material2->autoPrepare(true);
	material2->diffuseColor = vec3(0.9, 0.6, 0.4);
	//GEOMETRY - load model from file

	Geometry* proceduralGeometry = new Geometry();
	proceduralGeometry->loadFromFile("res://models/Cube.fbx");
	proceduralGeometry->setParent(resources);
	Mesh* model = new Mesh();
	model->addChild(material);
	model->setPosition(vec3(-1, 0, 5));
	
	Geometry* proceduralGeometry2 = new Geometry();
	proceduralGeometry2->loadFromFile("res://models/suz.fbx");
	proceduralGeometry2->setParent(resources);
	LuaResource* script = new LuaResource("res://scripts/program.lua");
	script->isEngineScript = true;
	script->setParent(resources);
	model->setParent(scene);
	model->name = "Model1";
	script->loadScript();

	Scene::saveSceneToFile("scene.bin");
	//CAMERA - define camera position, planes, and FOV
	//LOOP


	while (graphics->shouldOpenWindow()) {

		if ((glfwGetTime() - graphics->lastFrameTime) < (1.0 / (float)graphics->targetFPS)) continue;
		Scene::getCurrentScene()->PreRender();

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		{
		    ImGui::Begin("Settings Panel"); // Create a window
            
            ImGui::Text("Hello, user! Adjust settings below:"); // Text

			ImGui::End();

		}

					// Loop through all instances in your world map/vector
		{
			ImGui::Begin("Scene Hierarchy");
			
			EngineGUI::newInstanceSelected = false;
			EngineGUI::isHierarchyFocused = ImGui::IsWindowFocused();
			EngineGUI::DrawHierarchyNode(scene->getCurrentScene());
			
			ImGui::End();
		}

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
		
		ImGui::Render(); 
		for (auto i : Instance::world) {
			if (i.second) {
				i.second->update(graphics->deltaTime, Scene::getCurrentScene()->isStudio);
			}
		}
		glBindVertexArray(0);

		Scene::getCurrentScene()->RenderScene();

		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		Graphics::getInstance()->windowUpdate();
	}
	glfwTerminate();
	Scene::getCurrentScene()->destroy();
	return 0;

}
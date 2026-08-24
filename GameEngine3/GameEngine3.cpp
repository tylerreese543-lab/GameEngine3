#include "GameInstance.h"
#include "Mesh.h"
#include "Camera.h"
#include "Shader.h"
#include "TextureResource.h"
#include "Scene.h"
#include "Material.h"
#include "LuaResource.h"
int main() {

	//GRAPHICS - get the graphics instance and initialize
	Graphics* graphics = Graphics::getInstance();

	Scene* scene = new Scene();
	graphics->initializeWindow(800, 600, "Game Engine 3");
	graphics->matchViewportToWindow = true;

	LuaResource::InitializeLua();

	Camera* camera = new Camera(vec3(0, 0, -2));
	camera->fov = 70.0f;
	scene->addChild(camera);


	Instance* resources = new Instance();
	resources->name = "Resources";
	resources->setParent(scene);

	//SHADERS - load from file, parent to shader program and create;
	ShaderProgram* sprog = new ShaderProgram("shader/vertex_shader.glsl", "shader/fragment_shader.glsl");
	sprog->createProgram();
	sprog->setParent(resources);
	//IMAGES - load from file
	ImageResource* texture0 = new ImageResource(eMaterialTexture_Diffuse, "shader/poinsettia.png");
	ImageResource* texture1 = new ImageResource(eMaterialTexture_AmbientOcclusion, "shader/suz_ao.png");
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
	Geometry* mesh = new Geometry();
	mesh->loadFromFile("shader/Cube.fbx");
	mesh->setParent(resources);
	Mesh* model = new Mesh();
	model->addChild(material);
	model->geometry = mesh->getId();
	model->setPosition(vec3(-1, 0, 5));
	
	LuaResource* script = new LuaResource("shader/program.lua");
	script->setParent(resources);
	model->setParent(scene);
	model->name = "Model1";
	script->loadScript();
	//CAMERA - define camera position, planes, and FOV
	//LOOP
	while (graphics->shouldOpenWindow()) {

		if ((glfwGetTime() - graphics->lastFrameTime) < (1.0 / (float)graphics->targetFPS)) continue;
		Scene::getCurrentScene()->PreRender();

		for (auto i : Instance::world) {
			if (i.second) {
				i.second->update(graphics->deltaTime);
			}
		}
		Scene::getCurrentScene()->RenderScene();
	}
	glfwTerminate();
	Scene::getCurrentScene()->destroy();
	return 0;
}
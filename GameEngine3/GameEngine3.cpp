#include "GameInstance.h"
#include "Mesh.h"
#include "Camera.h"
#include "Shader.h"
#include "TextureResource.h"
#include "Scene.h"
int main() {


	//GRAPHICS - get the graphics instance and initialize
	Graphics* graphics = Graphics::getInstance();

	Scene* scene = new Scene();
	graphics->initializeWindow(800, 600, "Game Engine 3");
	graphics->matchViewportToWindow = true;

	Camera* camera = new Camera(0, 0, -2);
	camera->fov = 70.0f;
	scene->addChild(camera);


	Instance* resources = new Instance();
	resources->name = "Resources";

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
	material->diffuseColor = col3(0.4, 0.6, 0.9);

	Material* material2 = (Material*)material->clone();
	material2->getChildFromName("DiffuseTex")->name = "DiffuseTex2";
	material2->autoPrepare(true);
	material->diffuseColor = col3(0.9, 0.6, 0.4);
	//GEOMETRY - load model from file
	Geometry* mesh = new Geometry();
	mesh->loadFromFile("shader/suz_lodetail.fbx");
	Mesh* model = new Mesh();
	Mesh* model2 = new Mesh();
	model->material = material->getId();
	model2->material = material2->getId();
	model->geometry = mesh->getId();
	model2->geometry = mesh->getId();

	model->setPosition(vec3(-1, 0, 5));
	model2->setPosition(vec3(1, 0, 5));
	scene->addChild(model);
	scene->addChild(model2);
	//CAMERA - define camera position, planes, and FOV
	//LOOP
	while (graphics->shouldOpenWindow()) {
		scene->PreRender();
		model->rotation = vec3(PI * 0.5f, PI * 0.5f, PI * 0.5f);
		scene->RenderScene();
	}
	glfwTerminate();
	scene->destroy();
	return 0;
}
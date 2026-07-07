#include "GameInstance.h"
#include "Mesh.h"
#include "Camera.h"
#include "Shader.h"
#include "TextureResource.h"
int main() {


	//GRAPHICS - get the graphics instance and initialize
	Graphics* graphics = Graphics::getInstance();

	graphics->initializeWindow(800, 600, "Game Engine 3");
	graphics->matchViewportToWindow = true;

	//SHADERS - load from file, parent to shader program and create;

	ShaderProgram* sprog = new ShaderProgram("shader/vertex_shader.glsl", "shader/fragment_shader.glsl");
	sprog->createProgram();

	//IMAGES - load from file
	ImageResource* texture0 = new ImageResource(eMaterialTexture_Diffuse, "shader/poinsettia.png");
	ImageResource* texture1 = new ImageResource(eMaterialTexture_AmbientOcclusion, "shader/suz_ao.png");

	//MATERIAL - parent images to material, set color
	Material* material = new Material();
	material->addChild(texture0);
	material->addChild(texture1);
	material->autoSearchTextures(true);
	material->diffuseColor = col3(0.4, 0.6, 0.9);

	//GEOMETRY - load model from file
	Geometry* mesh = new Geometry();
	Mesh* model = new Mesh();

	model->addChild(mesh);
	mesh->loadFromFile("shader/suz_lodetail.fbx");

	//CAMERA - define camera position, planes, and FOV
	Camera* camera = new Camera(0, 0, -2);
	camera->fov = 70.0f;

	//LOOP
	while (graphics->shouldOpenWindow()) {
		graphics->clearWindow();
		camera->updateMatrices();
		sprog->useProgram();
		camera->aspect_ratio = graphics->getAspectRatio();
		model->rotation = vec3(PI * 0.5f, PI * 0.5f, PI * 0.5f);
		model->render(sprog, camera, material);
		graphics->windowUpdate();
	}
	glfwTerminate();
	return 0;
}
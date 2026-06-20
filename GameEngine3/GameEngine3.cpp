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
	ShaderResource vshader(GL_VERTEX_SHADER);
	ShaderResource fshader(GL_FRAGMENT_SHADER);

	vshader.loadFromFile("shader/vertex_shader.glsl");
	fshader.loadFromFile("shader/fragment_shader.glsl");

	ShaderProgram sprog;

	sprog.addChild(&vshader);
	sprog.addChild(&fshader);
	sprog.createProgram();

	//IMAGES - load from file, set texture type to diffuse (because this is a diffuse texture)
	ImageResource texture0(eMaterialTexture_Diffuse);
	texture0.loadFromFile("shader/poinsettia.png", GL_RGB);

	//MATERIAL - parent images to material, set color
	Material material;
	material.addChild(&texture0);
	material.autoSearchTextures(true);
	material.diffuseColor = col3(1.0);

	//GEOMETRY - fill VBO with data and generate VAO;
	Geometry mesh;

	Mesh model;

	model.addChild(&mesh);
	mesh.vbos[eVBO_Positions].data = new float[12] {
		-1.0, -1.0, 0.0,
		1.0, -1.0, 0.0,
		1.0, 1.0, 0.0,
		-1.0, 1.0, 0.0,
	};
	mesh.vbos[eVBO_Color].data = new float[12] {
		1.0, 0.0, 0.0, 
		0.0, 1.0, 0.0, 
		1.0, 1.0, 1.0, 
		0.0, 0.0, 1.0, 
	};
	mesh.vbos[eVBO_UV].data = new float[8] {
		 0.0, 0.0,
		 1.0, 0.0,
		 1.0, 1.0,
		 0.0, 1.0,
	};
	mesh.vbos[eVBO_Positions].calcSize(12);
	mesh.vbos[eVBO_Color].calcSize(12);
	mesh.vbos[eVBO_UV].calcSize(8);

	mesh.ebo.data = new unsigned int[6]{
		0, 1, 2,
		0, 2, 3,
	};
	mesh.ebo.calcSize(6);
	mesh.generateVAO(&sprog.layout);

	//CAMERA - define camera position, planes, and FOV

	Camera camera(0, 0, -2);
	camera.setPlanes(0.1f, 1000.0f);
	camera.fov = 70.0f;

	//LOOP

	while (graphics->shouldOpenWindow()) {
		graphics->clearWindow();
		camera.updateMatrices();
		sprog.useProgram();
		camera.aspect_ratio = graphics->getAspectRatio();
		model.rotation = vec3(0, 0.4 * glfwGetTime() * 2, 0.2 * glfwGetTime() * 2);
		model.render(&sprog, &camera, &material);
		graphics->windowUpdate();
	}
	glfwTerminate();
	return 0;
}
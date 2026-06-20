#include "GameInstance.h"
#include "Mesh.h"
#include "Camera.h"
#include "Shader.h"
#include "TextureResource.h"
int main() {

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
	//TRICK: by setting color and UV to "" we force use the miscellaneous VBO
	sprog.setDefaultVertexAttribute(eVAttribute_Color, "");
	sprog.setDefaultVertexAttribute(eVAttribute_UV, "");
	sprog.createProgram();

	//IMAGES - load from file;
	ImageResource texture0;
	texture0.loadFromFile("shader/poinsettia.png", GL_RGB);


	//GEOMETRY - define which VBOs we want to use, fill them with data and generate the VAO;
	Geometry mesh;

	Mesh model;

	model.addChild(&mesh);
	mesh.setVBOFlags(true, false, false, false, true);
	mesh.vbos[eVBO_Positions].data = new float[12] {
		-1.0, -1.0, 0.0,
		1.0, -1.0, 0.0,
		1.0, 1.0, 0.0,
		-1.0, 1.0, 0.0,
	};
	mesh.vbos[eVBO_Extra].data = new float[20] {
		1.0, 0.0, 0.0, 0.0, 0.0,
		0.0, 1.0, 0.0, 1.0, 0.0,
		1.0, 1.0, 1.0, 1.0, 1.0,
		0.0, 0.0, 1.0, 0.0, 1.0,
	};
	mesh.vbos[eVBO_Positions].calcSize(12);
	mesh.vbos[eVBO_Extra].calcSize(20);

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
		sprog.bindTexture("tex", &texture0);
		model.rotation = vec3(0, 0.4 * glfwGetTime() * 2, 0);
		model.render(&sprog, &camera);
		graphics->windowUpdate();
	}
	glfwTerminate();
	return 0;
}
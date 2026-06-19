#include "GameInstance.h"
#include "Mesh.h"
#include "Camera.h"
#include "Shader.h"
#include "TextureResource.h"
int main() {

	Graphics* graphics = Graphics::getInstance();
	graphics->initializeWindow(800, 600, "Game Engine 3");
	graphics->matchViewportToWindow = true;
	ShaderResource vshader(GL_VERTEX_SHADER);
	ShaderResource fshader(GL_FRAGMENT_SHADER);
	vshader.loadFromFile("shader/vertex_shader.glsl");
	fshader.loadFromFile("shader/fragment_shader.glsl");
	
	ImageResource texture0;
	texture0.loadFromFile("shader/poinsettia.png", GL_RGB);
	ShaderProgram sprog;
	sprog.addChild(&vshader);
	sprog.addChild(&fshader);
	sprog.createProgram();
	Mesh mesh;

	Camera camera(0, 0, -2);
	camera.setPlanes(0.1f, 1000.0f);
	camera.fov = 90.0f;

	mesh.vbo.data = new float[18]{

		0.0f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f,

		-0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 

		0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f,
	};

	mesh.vbo.size = 18 * sizeof(float);
	mesh.ebo.data = new unsigned int[3]{
		0, 1, 2
	};
	mesh.ebo.size = 3 * sizeof(unsigned int);

	mesh.generateVAO(&sprog.layout);
	while (graphics->shouldOpenWindow()) {
		graphics->clearWindow();
		camera.updateMatrices();
		sprog.useProgram();
		camera.aspect_ratio = graphics->getAspectRatio();
		camera.rotation = vec3(0, 0.4 * sinf(glfwGetTime() * 2), 0);
		sprog.setUniform3f("camera_position", camera.position);
		sprog.setUniformSMat("look", camera.look, GL_TRUE, 3);
		sprog.setUniformSMat("projection", camera.projection, GL_TRUE, 4);
		sprog.bindTexture("tex", &texture0);
		mesh.render();
		graphics->windowUpdate();
	}
	glfwTerminate();
	return 0;
}
#include "GameInstance.h"
#include "Mesh.h"
#include "Shader.h"
int main() {

	Graphics* graphics = Graphics::getInstance();
	graphics->initializeWindow(800, 600, "Game Engine 3");

	ShaderResource vshader(GL_VERTEX_SHADER);
	ShaderResource fshader(GL_FRAGMENT_SHADER);
	vshader.loadFromFile("shader/vertex_shader.glsl");
	fshader.loadFromFile("shader/fragment_shader.glsl");

	ShaderProgram sprog;
	sprog.addChild(&vshader);
	sprog.addChild(&fshader);
	sprog.createProgram();
	Mesh mesh;

	mesh.vbo.data = new float[9]{
		0.0f, 0.5f, 0.5f,
		-0.5f, -0.5f, 0.5f,
		0.5f, -0.5f, 0.5f
		};
	mesh.vbo.size = 9 * sizeof(float);
	mesh.ebo.data = new unsigned int[3]{
		0, 1, 2
	};

	mesh.ebo.size = 3 * sizeof(unsigned int);
	mesh.ebo.generate();
	mesh.vbo.generate();
	mesh.generateVAO(&sprog.layout);
	while (graphics->shouldOpenWindow()) {
		graphics->clearWindow();
		sprog.useProgram();
		mesh.render();
		graphics->windowUpdate();
	}
	glfwTerminate();
	return 0;
}
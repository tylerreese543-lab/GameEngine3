#include "Graphics.h"

Graphics* Graphics::instance = nullptr;
Graphics::Graphics() {
	viewportWidth = 800;
	viewportHeight = 600;
	vsyncEnabled = true;
	limitFPS = true;
	targetFPS = 60;
	matchViewportToWindow = true;
	window = nullptr;
	lastFrameTime = 0.0;
	currentFPS = 0.0;
}

Graphics::~Graphics() {
	if (window != nullptr) {
		glfwDestroyWindow(window);
	}
	glfwTerminate();
}

int Graphics::initializeWindow(int width, int height, const char* title) {
	if (!glfwInit()) {
		return -1;
	}
	window = glfwCreateWindow(width, height, title, nullptr, nullptr);
	if (window == nullptr) {
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	if (glewInit() != GLEW_OK) {
		return -1;
	}
	glViewport(0, 0, width, height);
	return 0;
}

void Graphics::clearWindow() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	currentFPS = 1.0 / (glfwGetTime() - lastFrameTime);
	lastFrameTime = glfwGetTime();
}
void Graphics::windowUpdate() {
	glfwSwapInterval(vsyncEnabled ? 1 : 0);
	glfwSwapBuffers(window);
	glfwPollEvents();
}

bool Graphics::shouldOpenWindow() {
	return !glfwWindowShouldClose(window);
}

Graphics* Graphics::getInstance() {
	if (instance == nullptr) {
		instance = new Graphics();
	}
	return instance;
}
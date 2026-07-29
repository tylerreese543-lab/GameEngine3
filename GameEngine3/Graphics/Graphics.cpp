#include "Graphics.h"

Graphics* Graphics::instance = nullptr;
Graphics::Graphics() {
	viewportWidth = 800;
	viewportHeight = 600;
	windowWidth = 800;
	windowHeight = 600;
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
		window = nullptr;
	}
	glfwTerminate();
}

int Graphics::initializeWindow(int width, int height, const char* title) {
	if (!glfwInit()) {
		return -1;
	}
	windowWidth = width;
	windowHeight = height;
	viewportWidth = width;
	viewportHeight = height;
	window = glfwCreateWindow(width, height, title, nullptr, nullptr);
	if (window == nullptr) {
		const char* errorDescription;
		int errorCode = glfwGetError(&errorDescription);
		printf("Failed to create GLFW window: %s (Error code: %d)\n", errorDescription, errorCode);
		return -1;
	}
	glfwMakeContextCurrent(window);
	int err = glewInit();
	if (err != GLEW_OK) {
		printf("Failed to initialize GLEW: %s\n", glewGetErrorString(err));
		return -1;
	}
	glViewport(0, 0, width, height);

	return 0;
}

void Graphics::clearWindow() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	deltaTime = (glfwGetTime() - lastFrameTime);
	currentFPS = 1.0 / deltaTime;
	lastFrameTime = glfwGetTime();
}
void Graphics::windowUpdate() {
	glfwGetWindowSize(window, &windowWidth, &windowHeight);
	if (matchViewportToWindow) {
		glViewport(0, 0, windowWidth, windowHeight);
	}
	mouse_data.update();
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

float Graphics::getAspectRatio() {
	return (float)windowHeight / (float)windowWidth;
}

MouseData::MouseData() {
	x = 0;
	y = 0;
}

void MouseData::update() {
	Graphics* g = Graphics::getInstance();
	glfwGetCursorPos(g->window, &(x), &(y));
	screenFull = vec2(x, y) - (vec2(g->windowWidth, g->windowHeight) * 0.5);
	screenNormalized = (screenFull * 2) / vec2(g->windowWidth, g->windowHeight);
}
void MouseData::setRawMouseInput(bool RawMouseInput) {
	if (RawMouseInput) {
		glfwSetInputMode(Graphics::getInstance()->window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	}
	else {
		glfwSetInputMode(Graphics::getInstance()->window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	}
}
bool MouseData::isMouseDown(int button) {
	return glfwGetMouseButton(Graphics::getInstance()->window, button) == GLFW_PRESS;
}
bool MouseData::isRawMouseInput() {
	return glfwGetInputMode(Graphics::getInstance()->window, GLFW_CURSOR) == GLFW_CURSOR_DISABLED;
}
bool KeyboardData::isKeyDown(int key) {
	return glfwGetKey(Graphics::getInstance()->window, key) == GLFW_PRESS;
}

KeyboardData::KeyboardData() {

}
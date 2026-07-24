#pragma once

#include <stdio.h>
#include "graphics_include.h"
#include "matrix.h"

class Graphics;

class MouseData {
public:
	double x;
	double y;
	vec2 screenNormalized;
	vec2 screenFull;

	MouseData();
	void update(Graphics* g);
};

class Graphics {
	static Graphics* instance;
public:

	int viewportWidth;
	int viewportHeight;
	int windowWidth;
	int windowHeight;
	//If true, the viewport will automatically match the size of the window. If false, the viewport will use the specified width and height.
	bool matchViewportToWindow;
	bool vsyncEnabled;
	bool limitFPS;
	int targetFPS;
	double currentFPS;

	double deltaTime;
	double lastFrameTime;

	MouseData* mouse_data;
	GLFWwindow* window;

	int initializeWindow(int width, int height, const char* title);
	void windowUpdate();
	void clearWindow();
	bool shouldOpenWindow();
	Graphics();
	~Graphics();
	static Graphics* getInstance();
	float getAspectRatio();
};
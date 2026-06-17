#pragma once

#include <stdio.h>
#include "graphics_include.h"

class Graphics {
	static Graphics* instance;
public:

	int viewportWidth;
	int viewportHeight;
	//If true, the viewport will automatically match the size of the window. If false, the viewport will use the specified width and height.
	bool matchViewportToWindow;
	bool vsyncEnabled;
	bool limitFPS;
	int targetFPS;
	double currentFPS;

	double lastFrameTime;
	GLFWwindow* window;

	int initializeWindow(int width, int height, const char* title);
	void windowUpdate();
	void clearWindow();
	bool shouldOpenWindow();
	Graphics();
	~Graphics();
	static Graphics* getInstance();
};
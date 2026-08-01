#pragma once

#include <stdio.h>
#include "graphics_include.h"
#include "matrix.h"
#include "../ScriptEvent.h"
class Graphics;

class MouseData {
public:
	double x;
	double y;
	vec2 screenNormalized;
	vec2 screenFull;

	MouseData();
	void update();
	void setRawMouseInput(bool rawMouseInput);
	bool isMouseDown(int button);
	bool isRawMouseInput();

	ScriptEvent on_left_click;
	ScriptEvent on_left_release;
	ScriptEvent on_right_click;
	ScriptEvent on_right_release;
	void SetButtonCallback(int button, int action, int mods);
};
class KeyboardData {
public:
	KeyboardData();
	bool isKeyDown(int key);

	ScriptEvent on_key_down;
	ScriptEvent on_key_up;
	void SetKeyCallback(int key, int scancode, int action, int mods);
};

class Gamepad {
public:
	bool isConnected;
	GLFWgamepadstate gamepad;
	int joystickId;
	const char* getName();
	Gamepad();

	vec2 getDPadVector();
	vec2 getJoystickLocation(int joystick);
	float getTriggerPress(int trigger);
	bool isButtonPressed(int button);
};

class GamepadData {
public:
	void UpdatePad(int jid);
	void UpdateGamepads();
	Gamepad gamepads[GLFW_JOYSTICK_LAST];
	Gamepad* getGamepad(int jid);

	GamepadData();

	ScriptEvent on_connect;
	ScriptEvent on_disconnect;

	void ControllerConnectCallback(int jid, int event);
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

	MouseData mouse_data;
	KeyboardData keyboard_data;
	GamepadData gamepad_data;
	GLFWwindow* window;

	int initializeWindow(int width, int height, const char* title);
	void windowUpdate();
	void clearWindow();
	bool shouldOpenWindow();
	Graphics();
	~Graphics();
	static Graphics* getInstance();
	float getAspectRatio();

	static void controllerConnectCallback(int jid, int event);
	static void mouseSetButtonCallback(GLFWwindow* window, int button, int action, int mods);
	static void keyboardSetKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
};


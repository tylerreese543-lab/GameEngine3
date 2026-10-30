#include "Graphics.h"
#include "../EngineGUI.h"
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

	glfwSetMouseButtonCallback(window, &Graphics::mouseSetButtonCallback);
	glfwSetKeyCallback(window, &Graphics::keyboardSetKeyCallback);
	glfwSetJoystickCallback(&Graphics::controllerConnectCallback);
	return 0;
}

void Graphics::clearWindow() {
	glDepthMask(GL_TRUE); 
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

void Graphics::mouseSetButtonCallback(GLFWwindow* window, int button, int action, int mods) {
	getInstance()->mouse_data.SetButtonCallback(button, action, mods);
}

void Graphics::keyboardSetKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	getInstance()->keyboard_data.SetKeyCallback(key, scancode, action, mods);
}

void Graphics::controllerConnectCallback(int jid, int event) {
	getInstance()->gamepad_data.ControllerConnectCallback(jid, event);
}
MouseData::MouseData() {
	x = 0;
	y = 0;
}

void MouseData::update() {
	Graphics* g = Graphics::getInstance();
	glfwGetCursorPos(g->window, &(x), &(y));
	vec2 s = vec2(x, y) - (vec2(g->windowWidth, g->windowHeight) * 0.5);
	delta = s - screenFull;
	screenFull = s;
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
void MouseData::SetButtonCallback(int button, int action, int mods) {
			ImGuiIO& io = ImGui::GetIO();

		if (io.WantCaptureMouse) {
			return;
			}
	if (action == GLFW_PRESS) {
		if (button == GLFW_MOUSE_BUTTON_LEFT) {
			for (sol::protected_function& f : on_left_click.connections) {
				f(mods);
			}
		} else if(button == GLFW_MOUSE_BUTTON_MIDDLE) {
			for (sol::protected_function& f : on_right_click.connections) {
				f(mods);
			}
		}
	}
	else {
		if (button == GLFW_MOUSE_BUTTON_LEFT) {
			for (sol::protected_function& f : on_left_release.connections) {
				f(mods);
			}
		} else if(button == GLFW_MOUSE_BUTTON_MIDDLE) {
			for (sol::protected_function& f : on_right_release.connections) {
				f(mods);
			}
		}
	}
}

void KeyboardData::SetKeyCallback(int key, int scancode, int action, int mods) {
		ImGuiIO& io = ImGui::GetIO();

		if (io.WantCaptureKeyboard) {
			return;
			}
			if (action == GLFW_PRESS) {
				for (sol::protected_function& f : on_key_down.connections) {
					f(key, scancode, mods);
				}
			}
			else if (action == GLFW_RELEASE) {
				for (sol::protected_function& f : on_key_up.connections) {
					f(key, scancode, mods);
				}
			}
}
bool KeyboardData::isKeyDown(int key) {
	ImGuiIO& io = ImGui::GetIO();

	if (io.WantCaptureKeyboard) {
		return false;
	}
	return glfwGetKey(Graphics::getInstance()->window, key) == GLFW_PRESS;
}

KeyboardData::KeyboardData() {

}

GamepadData::GamepadData() {

}

void GamepadData::UpdateGamepads() {
	for (int i = 0; i < GLFW_JOYSTICK_LAST; ++i) {
		UpdatePad(i);
	}
}

void GamepadData::UpdatePad(int i) {
		gamepads[i].isConnected = glfwJoystickPresent(i) && glfwJoystickIsGamepad(i);
		gamepads[i].joystickId = i;
		if (gamepads[i].isConnected) {
			glfwGetGamepadState(i, &gamepads[i].gamepad);
		}
}

Gamepad* GamepadData::getGamepad(int jid) {
	if (jid < 0 || jid >= GLFW_JOYSTICK_LAST) {
		return nullptr;
	}

	if (!gamepads[jid].isConnected) {
		UpdatePad(jid);
		if (!gamepads[jid].isConnected) {
			return nullptr;
		}
	}
	return &gamepads[jid];
}

vec2 Gamepad::getJoystickLocation(int joystick) {
	if (!isConnected) return vec2(0, 0);
	if (joystick == 0) {
		return vec2(gamepad.axes[GLFW_GAMEPAD_AXIS_LEFT_X], gamepad.axes[GLFW_GAMEPAD_AXIS_LEFT_Y]);
	}
	else if (joystick == 1) {
		return vec2(gamepad.axes[GLFW_GAMEPAD_AXIS_RIGHT_X], gamepad.axes[GLFW_GAMEPAD_AXIS_RIGHT_Y]);
	}
	else {
		return vec2(0, 0);
	}
}
float Gamepad::getTriggerPress(int trigger) {
	if (!isConnected) return 0;
	if (trigger == 0) {
		return gamepad.axes[GLFW_GAMEPAD_AXIS_LEFT_TRIGGER];
	} else if (trigger == 1) {
		return gamepad.axes[GLFW_GAMEPAD_AXIS_RIGHT_TRIGGER];
	}
	else {
		return 0;
	}
}
vec2 Gamepad::getDPadVector() {
	if (!isConnected) return vec2(0, 0);
	return vec2(
		gamepad.buttons[GLFW_GAMEPAD_BUTTON_DPAD_RIGHT] - gamepad.buttons[GLFW_GAMEPAD_BUTTON_DPAD_LEFT],
		gamepad.buttons[GLFW_GAMEPAD_BUTTON_DPAD_UP] - gamepad.buttons[GLFW_GAMEPAD_BUTTON_DPAD_DOWN]
	);
}
bool Gamepad::isButtonPressed(int button) {
	return gamepad.buttons[button] == GLFW_PRESS;
}
void GamepadData::ControllerConnectCallback(int jid, int event) {
	if (event == GLFW_CONNECTED) {
		for (sol::protected_function& f : on_connect.connections) {
			f(jid);
		}
	} else if (event == GLFW_DISCONNECTED) {
		for (sol::protected_function& f : on_disconnect.connections) {
			f(jid);
		}
	}
}
const char* Gamepad::getName() {
	if (!isConnected) {
		return nullptr;
	}
	return glfwGetGamepadName(joystickId);
}

Gamepad::Gamepad() {
	gamepad = GLFWgamepadstate();

	isConnected = false;
	joystickId = 0;
}
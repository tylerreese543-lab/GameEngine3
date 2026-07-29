#pragma once
#include "GameInstance.h"
#include "Camera.h"
#include "Mesh.h"
class Scene : public Instance
{
	static int currentScene;
public:
	int camera;
	Scene();

	static void setCurrentScene(int scene);
	static Scene* getCurrentScene();
	Camera* getCamera();
	static Camera* getCurrentCamera();
	void PreRender();
	void RenderScene();
};


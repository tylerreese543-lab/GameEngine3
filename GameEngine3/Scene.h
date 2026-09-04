#pragma once
#include "GameInstance.h"
#include "Camera.h"
#include "Mesh.h"
class Scene : public Instance
{
	static int currentScene;
	static void WriteGameFileHeader(FILE* file);
public:
	int camera;
	int hitboxShader;
	bool isStudio;
	Scene();

	static void setCurrentScene(int scene);
	static Scene* getCurrentScene();
	Camera* getCamera();
	static Camera* getCurrentCamera();
	void PreRender();
	void RenderScene();

	virtual int saveInstanceToFile(FILE* file);
	virtual int loadInstanceFromFile(void* data, int offset = 0);

	static int saveSceneToFile(const char* filepath);
	static int loadSceneFromFile(const char* filepath);
};


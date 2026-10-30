#pragma once
#include "GameInstance.h"
#include "Camera.h"
#include "Mesh.h"
class Scene : public Instance
{
	static int currentScene;
	static void WriteGameFileHeader(FILE* file);
	static bool LoadGameFileHeader(FILE* file, int* numInstances);

public:
	static const bool isStudio;
	int currentCamera;
	//The ID of the camera to load into currentCamera when this is loaded at runtime;
	int startCamera;
	int hitboxShader;
	int uiRoot;


	Scene();

	static Instance* newInstanceFromType(int type = InstanceType_Undefined);
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

	virtual int getNumSaveBytes();

	static void writeTagAndSizeToFile(int tag, int size, FILE* file);
};

class UIRootNode : public Instance {
public:
	int camera;
	Camera* getCamera();

	UIRootNode();
	virtual Instance* clone_this();
	virtual void setCamera(int id);
	virtual int getNumSaveBytes();
	virtual int saveInstanceToFile(FILE* file);
	virtual int loadInstanceFromFile(void* data, int off = 0);
	virtual void finalizeLoad(inst_conv_map* conversionMap);
};
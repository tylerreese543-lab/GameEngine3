#pragma once
#include "GameInstance.h"
#include "Camera.h"
#include "Mesh.h"
class Scene : public Instance
{
public:
	int camera;
	Scene();

	void PreRender();
	void RenderScene();
};


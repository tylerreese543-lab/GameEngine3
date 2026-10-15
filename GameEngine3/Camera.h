#pragma once
#include "GameInstance.h"
#include "Graphics/matrix.h"
class Camera : public Instance
{
public:
	vec3 position;
	vec3 rotation;

	mat4 transform;
	mat4 projection;
	mat3 look;
	//Essentially the 2D direction of the camera, ignoring the Y axis. This is used for movement and other calculations.
	mat3 moveDirection2D;

	void updateMatrices();
	
	float near_plane;
	float far_plane;
	float fov;
	float aspect_ratio;
	void setPosition(float x, float y, float z);
	void setPlanes(float near, float far);
	float fovGL();
	bool isEditorCamera;
	virtual void update(float deltaTime, bool isStudio);
	Camera();
	Camera(vec3);

	virtual bool omitFromSave();
	virtual Instance* clone_this();
	virtual int saveInstanceToFile(FILE* file);
	virtual int loadInstanceFromFile(void* data, int offset = 0);
	virtual int getNumSaveBytes();

	void debugRender();
};


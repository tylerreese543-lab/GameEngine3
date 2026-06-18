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

	void updateMatrices();
	
	float near_plane;
	float far_plane;
	float fov;
	float aspect_ratio;
	void setPosition(float x, float y, float z);
	void setPlanes(float near, float far);
	float fovGL();
	Camera();
	Camera(float x, float y, float z);
};


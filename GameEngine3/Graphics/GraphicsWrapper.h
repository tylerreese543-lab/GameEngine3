#pragma once

#include "graphics_include.h"


//basic functions made easier to use for graphics. More to come

int getSizeOfType(GLenum type);
void setMat4Row(float* array, float f0, float f1, float f2, float f3);
void setMat3Row(float* array, float f0, float f1, float f2);
void multMat4(float* arrays, float* arr0, float* r1);

//VBO indices for the 'Mesh' class
enum {
	eVBO_Positions = 0,
	eVBO_UV,
	eVBO_Normal,
	eVBO_Color,
	eVBO_Extra,
	eVBO_MAX,
};
//Indices for the ShaderProgams 'shaderAttributeNames' array
enum {
	eVAttribute_Position = 0,
	eVAttribute_UV,
	eVAttribute_Normal,
	eVAttribute_Color,
	eVAttribute_MAX,
};
//Indices for the ShaderPrograms 'shaderUniformAttributes' array
enum {
	eSUniform_CameraPosition = 0,
	eSUniform_CameraLookMatrix,
	eSUniform_ModelTransformMatrix,
	eSUniform_CameraProjectionMatrix,
	eSUniform_MAX,
};
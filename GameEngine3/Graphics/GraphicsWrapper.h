#pragma once

#include "graphics_include.h"
#include <stdlib.h>
#include <stdarg.h>
//basic functions made easier to use for graphics. More to come

int getSizeOfType(GLenum type);
void setMat4Row(float* array, float f0, float f1, float f2, float f3);
void setMat3Row(float* array, float f0, float f1, float f2);
void debug_log(const char* format, ...);
//void multMat4(float* arrays, float* arr0, float* r1); Deleted - matrix.h added

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
	eSUniform_Material_DiffuseColor,
	eSUniform_Material_Specularity,
	eSUniform_CurrentTime,
	eSUniform_MAX
};

enum {
	eMaterialTexture_Diffuse = 0,
	eMaterialTexture_AmbientOcclusion,
	eMaterialTexture_Normal,
	eMaterialTexture_Specular,
	eMaterialTexture_MAX
};
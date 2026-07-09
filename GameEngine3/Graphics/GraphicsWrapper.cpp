#include "GraphicsWrapper.h"
int getSizeOfType(GLenum type) {
	switch (type) {
	case GL_FLOAT_VEC3:
		return 12;
	case GL_FLOAT_VEC2:
		return 8;
	case GL_FLOAT_VEC4:
		return 16;
	case GL_DOUBLE:
		return 8;
	case GL_FLOAT:
	case GL_INT:
	case GL_UNSIGNED_INT:
		return 4;
	case GL_SHORT:
	case GL_UNSIGNED_SHORT:
		return 2;
	case GL_UNSIGNED_BYTE:
	case GL_BYTE:
		return 1;
	default:
		return 0;
	}
}

void setMat4Row(float* array, float f0, float f1, float f2, float f3) {
	array[0] = f0;
	array[1] = f1;
	array[2] = f2;
	array[3] = f3;
}

void setMat3Row(float* array, float f0, float f1, float f2) {
	array[0] = f0;
	array[1] = f1;
	array[2] = f2;
}

void multMat4(float* arrS, float* arr0, float* arr1) {

}

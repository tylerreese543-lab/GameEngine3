#pragma once

#include "graphics_include.h"


//basic functions made easier to use for graphics. More to come

int getSizeOfType(GLenum type);
void setMat4Row(float* array, float f0, float f1, float f2, float f3);
void setMat3Row(float* array, float f0, float f1, float f2);
void multMat4(float* arrays, float* arr0, float* r1);
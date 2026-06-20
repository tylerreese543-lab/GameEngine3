#pragma once

#include "Buffer.h"
#include "BufferLayout.h"
//TODO: add overrides for shaders n stuff
int GenVAO(GLuint* vao, Buffer* vbo, Buffer* ebo, BufferLayout* layout, int numVBO = 1);
int DrawVAOElements(GLuint vao, GLenum type, GLint count);
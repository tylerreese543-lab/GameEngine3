#pragma once

#include "graphics_include.h"
#include "GraphicsWrapper.h"
#include <vector>
#include "Buffer.h"

class BufferLayoutElement {
public:
	GLuint binding;
	GLenum type;
	GLuint count;
	GLboolean normalized;
	BufferLayoutElement();
	BufferLayoutElement(GLenum type, GLuint count);
	BufferLayoutElement(GLuint count); //Assumes GL_FLOAT and GL_FALSE for normalized
	BufferLayoutElement(GLenum type, GLuint count, GLboolean normalized);

};
class BufferLayout {
public:
	std::vector<BufferLayoutElement> elements;
	GLuint getStride(int binding = -1);
	BufferLayout() {}
	BufferLayout(std::initializer_list<BufferLayoutElement> elements);

	void addElement(GLuint count); //Assumes GL_FLOAT and GL_FALSE for normalized
	void addElement(GLenum type, GLuint count);
	void addElement(GLenum type, GLuint count, GLboolean normalized);

	void clear();
	
	void apply();
	//Apply Extended - gives more control and allows multiple VBOs to be added into different attributes separately
	void applyEX(Buffer* vbos, int numVBO = 1);
	void remove();
};
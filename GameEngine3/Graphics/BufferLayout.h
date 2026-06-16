#pragma once

#include "graphics_include.h"
#include <vector>

class BufferLayoutElement {
public:
	GLenum type;
	GLuint count;
	GLboolean normalized;
	BufferLayoutElement() {}
	BufferLayoutElement(GLenum type, GLuint count);
	BufferLayoutElement(GLuint count); //Assumes GL_FLOAT and GL_FALSE for normalized
	BufferLayoutElement(GLenum type, GLuint count, GLboolean normalized);

	static int getSizeOfType(GLenum type);

};
class BufferLayout {
public:
	std::vector<BufferLayoutElement> elements;
	GLuint getStride();
	BufferLayout() {}
	BufferLayout(std::initializer_list<BufferLayoutElement> elements);

	void addElement(GLuint count); //Assumes GL_FLOAT and GL_FALSE for normalized
	void addElement(GLenum type, GLuint count);
	void addElement(GLenum type, GLuint count, GLboolean normalized);

	void clear();
	
	void apply();
	void remove();
};
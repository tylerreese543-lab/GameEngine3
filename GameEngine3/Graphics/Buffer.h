#pragma once
#include "graphics_include.h"

class Buffer {
	public:
	GLuint id;
	GLenum target;
	GLenum usage;
	GLsizeiptr size;
	void* data;
	Buffer(GLenum target, GLsizeiptr size, GLenum usage);
	virtual ~Buffer();
	virtual int generate();
	virtual int bind();
	virtual int unbind();
	virtual int setData(GLsizeiptr size, GLenum usage);
	virtual int update();
};
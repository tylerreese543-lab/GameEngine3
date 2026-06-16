#include "Buffer.h"

Buffer::Buffer(GLenum target, GLsizeiptr size, GLenum usage) {
	this->target = target;
	this->size = size;
	this->usage = usage;
}

Buffer::~Buffer() {
	glDeleteBuffers(1, &id);
}

int Buffer::generate() {
	glGenBuffers(1, &id);
	return 0;
}

int Buffer::bind() {
	glBindBuffer(target, id);
	return 0;
}

int Buffer::unbind() {
	glBindBuffer(target, 0);
	return 0;
}

int Buffer::setData(GLsizeiptr size, GLenum usage) {
	this->size = size;
	this->usage = usage;
	glBufferData(target, size, data, usage);
	return 0;
}

int Buffer::update() {
	glBufferData(target, size, data, usage);
	return 0;
}
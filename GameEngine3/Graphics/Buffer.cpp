#include "Buffer.h"

Buffer::Buffer(GLenum target, GLsizeiptr size, GLenum usage) {
	this->target = target;
	this->size = size;
	this->usage = usage;
	id = 0;
	data = nullptr;
	element_size = 0;
}
Buffer::Buffer() {
	target = GL_ARRAY_BUFFER;
	size = 0;
	usage = GL_STATIC_DRAW;
	id = 0;
	data = nullptr;
	element_size = 0;
}
Buffer::~Buffer() {
	glDeleteBuffers(1, &id);
	if (data != nullptr) {
		free(data);
		data = nullptr;
	}
	size = 0;

}

int Buffer::generate() {
	if (id == 0) {
		glGenBuffers(1, &id);
	}
	bind();
	update();
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

void Buffer::calcSize(int count) {
	size = count * element_size;
}

int Buffer::getCount() {
	return size / element_size;
}
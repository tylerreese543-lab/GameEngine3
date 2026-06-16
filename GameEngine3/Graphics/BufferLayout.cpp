#include "BufferLayout.h"

BufferLayoutElement::BufferLayoutElement(GLenum type, GLuint count) {
	this->type = type;
	this->count = count;
	this->normalized = GL_FALSE;
}

BufferLayoutElement::BufferLayoutElement(GLuint count) {
	this->type = GL_FLOAT;
	this->count = count;
	this->normalized = GL_FALSE;
}

BufferLayoutElement::BufferLayoutElement(GLenum type, GLuint count, GLboolean normalized) {
	this->type = type;
	this->count = count;
	this->normalized = normalized;
}

int BufferLayoutElement::getSizeOfType(GLenum type) {
	switch (type) {
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

GLuint BufferLayout::getStride() {
	GLuint stride = 0;
	for (BufferLayoutElement element : elements) {
		stride += element.count * BufferLayoutElement::getSizeOfType(element.type);
	}
	return stride;
}

BufferLayout::BufferLayout(std::initializer_list<BufferLayoutElement> elements) {
	this->elements = elements;
}

void BufferLayout::addElement(GLuint count) {
	elements.push_back(BufferLayoutElement(count));
}

void BufferLayout::addElement(GLenum type, GLuint count) {
	elements.push_back(BufferLayoutElement(type, count));
}

void BufferLayout::addElement(GLenum type, GLuint count, GLboolean normalized) {
	elements.push_back(BufferLayoutElement(type, count, normalized));
}

void BufferLayout::clear() {
	elements.clear();
}
void BufferLayout::apply() {
	GLuint offset = 0;
	for (BufferLayoutElement element : elements) {
		glEnableVertexAttribArray(offset);
		glVertexAttribPointer(offset, element.count, element.type, element.normalized, getStride(), (const void*)(uintptr_t)offset);
		offset += element.count * BufferLayoutElement::getSizeOfType(element.type);
	}
}

void BufferLayout::remove() {
	for (GLuint i = 0; i < elements.size(); i++) {
		glDisableVertexAttribArray(i);
	}
}
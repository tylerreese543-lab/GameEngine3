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
BufferLayoutElement::BufferLayoutElement() {
	this->type = GL_FLOAT;
	this->count = 1;
	this->normalized = GL_FALSE;
}
BufferLayoutElement::BufferLayoutElement(GLenum type, GLuint count, GLboolean normalized) {
	this->type = type;
	this->count = count;
	this->normalized = normalized;
}
GLuint BufferLayout::getStride() {
	GLuint stride = 0;
	for (BufferLayoutElement element : elements) {
		stride += element.count * getSizeOfType(element.type);
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
	GLuint index = 0;
	for (BufferLayoutElement element : elements) {
		//printf("Ind: %d, off: %d\n",index, offset);
		glEnableVertexAttribArray(index);
		glVertexAttribPointer(index, element.count, element.type, element.normalized, getStride(), (const void*)(uintptr_t)offset);
		offset += element.count * getSizeOfType(element.type);
		++index;

	}

	printf("Str: %d, size: %d\n", getStride(), (int)elements.size());
}

void BufferLayout::remove() {
	for (GLuint i = 0; i < elements.size(); i++) {
		glDisableVertexAttribArray(i);
	}
}
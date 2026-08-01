#include "BufferLayout.h"
BufferLayoutElement::BufferLayoutElement(GLenum type, GLuint count) {
	this->type = type;
	this->count = count;
	this->normalized = GL_FALSE;
	this->binding = 0;
}

BufferLayoutElement::BufferLayoutElement(GLuint count) {
	this->type = GL_FLOAT;
	this->count = count;
	this->normalized = GL_FALSE;
	this->binding = 0;
}
BufferLayoutElement::BufferLayoutElement() {
	this->type = GL_FLOAT;
	this->count = 1;
	this->normalized = GL_FALSE;
	this->binding = 0;
}
BufferLayoutElement::BufferLayoutElement(GLenum type, GLuint count, GLboolean normalized) {
	this->type = type;
	this->count = count;
	this->normalized = normalized;
	this->binding = 0;
}
GLuint BufferLayout::getStride(int binding) {
	GLuint stride = 0;
	for (BufferLayoutElement element : elements) {
		if (binding == -1 || element.binding == binding) {
			stride += element.count * getSizeOfType(element.type);
		}
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

}

void BufferLayout::remove() {
	for (GLuint i = 0; i < elements.size(); i++) {
		glDisableVertexAttribArray(i);
	}
}

void BufferLayout::applyEX(Buffer* vbos, int numVBO) {
	GLuint index = 0;
	GLuint* offsets = new GLuint[numVBO]{};
	GLuint stride = 0; //Tightly pack individual data
	if (numVBO == 1) {
		stride = getStride(); //If there is one vbo (interleaved) then use full stride
	}
	for (BufferLayoutElement element : elements) {
		if (vbos[element.binding].id != 0) {
			vbos[element.binding].bind();
		}

		stride = 0; //Tightly pack individual data for fully separated objects
		if (numVBO == 1) {
			stride = getStride(); //If there is one vbo (interleaved) then use full stride
		} else if (element.binding == eVBO_Extra) {
			stride = getStride(eVBO_Extra); //If we are using the miscellaneous VBO which is always packed 
		}
		glEnableVertexAttribArray(index);
		glVertexAttribPointer(index, element.count, element.type, element.normalized, stride, (const void*)(uintptr_t)offsets[element.binding]);
		offsets[element.binding] += element.count * getSizeOfType(element.type);
		++index;

		vbos[element.binding].unbind();

	}

	printf("Str: %d, size: %d\n", getStride(), (int)elements.size());
}
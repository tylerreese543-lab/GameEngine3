#include "VertexArrayObject.h"

int GenVAO(GLuint* vao, Buffer* vbo, Buffer* ebo, BufferLayout* layout) {
	glGenVertexArrays(1, vao);
	glBindVertexArray(*vao);
	vbo->bind();
	if (ebo) {
		ebo->bind(); //Not all VAOs will have an EBO; if so use drawArrays instead of drawElements
	}
	layout->apply();
	glBindVertexArray(0);
	vbo->unbind();
	if (ebo) {
		ebo->unbind();
	}
	return 0;
}
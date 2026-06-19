#include "VertexArrayObject.h"
int GenVAO(GLuint* vao, Buffer* vbo, Buffer* ebo, BufferLayout* layout) {
	glGenVertexArrays(1, vao);
	glBindVertexArray(*vao);
	vbo->bind();
	layout->apply();
	if (ebo) {
		ebo->bind(); //Not all VAOs will have an EBO; if so use drawArrays instead of drawElements
	}
	glBindVertexArray(0);
	vbo->unbind();
	if (ebo) {
		ebo->unbind();
	}
	return 0;
}

int DrawVAOElements(GLuint vao, GLenum type, GLint count) {
	glBindVertexArray(vao);
	glDrawElements(GL_TRIANGLES, count, type, nullptr);
	glBindVertexArray(0);
	return 0;
}
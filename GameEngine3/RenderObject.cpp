#include "RenderObject.h"
RenderObject::RenderObject() : Instance() {
	vao = 0;
	type = InstanceType_RenderObject;
}
RenderObject::~RenderObject() {
	glDeleteVertexArrays(1, &vao);
}

void RenderObject::render() {
	if (vao != 0) {
		DrawVAOElements(vao, GL_UNSIGNED_INT, 0);
	}
}
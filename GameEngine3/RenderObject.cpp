#include "RenderObject.h"
RenderObject::RenderObject() {
	vao = 0;
	type = InstanceType_RenderObject;
}
RenderObject::~RenderObject() {
	glDeleteVertexArrays(1, &vao);
}
Instance* RenderObject::clone() {
	return new RenderObject(*this);
}

void RenderObject::render() {
	if (vao != 0) {
		DrawVAOElements(vao, GL_UNSIGNED_INT, 0);
	}
}
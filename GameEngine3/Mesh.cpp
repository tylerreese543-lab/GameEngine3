#include "Mesh.h"

Mesh::Mesh() : RenderObject() {
	name = "Mesh";
	type = InstanceType_Mesh;

	vbo.element_size = getSizeOfType(GL_FLOAT);
	ebo.element_size = getSizeOfType(GL_UNSIGNED_INT);
	vbo.target = GL_ARRAY_BUFFER;
	ebo.target = GL_ELEMENT_ARRAY_BUFFER;
}

Mesh::~Mesh() {
}

Instance* Mesh::clone() {
	return new Mesh(*this);
}

int Mesh::generateVAO(BufferLayout* layout) {
	ebo.generate();
	vbo.generate();
	return GenVAO(&vao, &vbo, &ebo, layout);
}

void Mesh::render() {
	if (vao != 0) {
		DrawVAOElements(vao, GL_UNSIGNED_INT, (int)(ebo.size / ebo.element_size));
	}
}

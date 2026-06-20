#include "Mesh.h"

Geometry::Geometry() : RenderObject() {
	name = "Mesh";
	type = InstanceType_Geometry;

	ebo.element_size = getSizeOfType(GL_UNSIGNED_INT);
	ebo.target = GL_ELEMENT_ARRAY_BUFFER;

	_blankInitVBO();
	setVBOFlags(true, false, false, false);

}

Geometry::~Geometry() {
}

Instance* Geometry::clone() {
	return new Geometry(*this);
}

int Geometry::generateVAO(BufferLayout* layout) {
	ebo.generate();
	for (int i = eVBO_Positions; i < eVBO_MAX; ++i) {
		if (vboFlags[i]) {
			vbos[i].generate();
		}
	}
	return GenVAO(&vao, vbos, &ebo, layout, eVBO_MAX);
}

void Geometry::render() {
	if (vao != 0) {
		DrawVAOElements(vao, GL_UNSIGNED_INT, (int)(ebo.size / ebo.element_size));
	}
}

void Geometry::setVBOFlags(bool hasPositions, bool hasUV, bool hasNormal, bool hasColor, bool hasExtra) {
	vboFlags[eVBO_Positions] = hasPositions;
	vboFlags[eVBO_UV] = hasUV;
	vboFlags[eVBO_Normal] = hasNormal;
	vboFlags[eVBO_Color] = hasColor;
	vboFlags[eVBO_Extra] = hasExtra;
}

void Geometry::_blankInitVBO() {
	for (int i = eVBO_Positions; i < eVBO_MAX; ++i) {
		vbos[i].element_size = getSizeOfType(GL_FLOAT);
		vbos[i].target = GL_ARRAY_BUFFER;
		vbos[i].usage = GL_STATIC_DRAW;
	}
}
Mesh::Mesh() {
	setPosition(vec3(0, 0, 0));
	setRotation(vec3(0, 0, 0));
	setScale(vec3(1));

	geometry = nullptr;
}

void Mesh::setPosition(vec3 pos) {
	position = pos;
}

void Mesh::setRotation(vec3 rot) {
	rotation = rot;
}

void Mesh::setScale(vec3 scl) {
	scale = scl;
}

void Mesh::calcTransform() {
	transform = mat4::transform(position, scale, rotation);
}

void Mesh::render(ShaderProgram* program, Camera* camera) {
	if (geometry == nullptr) {
		geometry = autoGeometry();
	}
	if (geometry == nullptr) {
		printf("Abort rendering of '%s' - has no geometry bound.", name.c_str());
		return;
	}

	if (camera == nullptr) {
		printf("Abort rendering of '%s' - has no camera attached.", name.c_str());
		return;
	}
	if (program == nullptr) {
		printf("Abort rendering of '%s' - has no shader program attached.", name.c_str());
		return;
	}

	if (geometry->vao == 0) {
		geometry->generateVAO(&program->layout);
	}
	calcTransform();
	program->setUniformSMat(program->shaderUniformNames[eSUniform_ModelTransformMatrix], transform, GL_TRUE, 4);
	program->setUniform3f(program->shaderUniformNames[eSUniform_CameraPosition], camera->position);
	program->setUniformSMat(program->shaderUniformNames[eSUniform_CameraLookMatrix], camera->look, GL_TRUE, 3);
	program->setUniformSMat(program->shaderUniformNames[eSUniform_CameraProjectionMatrix], camera->projection, GL_TRUE, 4);
	geometry->render();
	
}

Geometry* Mesh::autoGeometry() {
	for (Instance*& child : children) {
		if (child->type == InstanceType_Geometry) {
			return dynamic_cast<Geometry*>(child);
		}
	}
	return nullptr;
}
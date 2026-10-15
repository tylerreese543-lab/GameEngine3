#include "AABB.h"

#include "Graphics/graphics_include.h"
#include "Graphics/Buffer.h"
#include "Graphics/VertexArrayObject.h"
#include "Scene.h"
std::set<int> Collision3D::allCollisionBoxes;
AABB2D::AABB2D() {
	min = vec2(0.0f, 0.0f);
	max = vec2(0.0f, 0.0f);
}

AABB2D::AABB2D(const vec2& min, const vec2& max) {
	this->min = min;
	this->max = max;
}

bool AABB2D::intersects(const AABB2D& other) const {
	return (min.X <= other.max.X && max.X >= other.min.X) &&
		(min.Y <= other.max.Y && max.Y >= other.min.Y);
}

bool AABB2D::contains(const vec2& point) const {
	printf("Checking if point (%f, %f) is inside AABB2D with min (%f, %f) and max (%f, %f)\n", point.X, point.Y, min.X, min.Y, max.X, max.Y);
	return (point.X >= min.X && point.X <= max.X) &&
		(point.Y >= min.Y && point.Y <= max.Y);
}

AABB3D::AABB3D() {
	min = vec3(0.0f, 0.0f, 0.0f);
	max = vec3(0.0f, 0.0f, 0.0f);
}

AABB3D::AABB3D(const vec3& min, const vec3& max) {
	this->min = min;
	this->max = max;
}

bool AABB3D::intersects(const AABB3D& other) const {
	return (min.X <= other.max.X && max.X >= other.min.X) &&
		(min.Y <= other.max.Y && max.Y >= other.min.Y) &&
		(min.Z <= other.max.Z && max.Z >= other.min.Z);
}

bool AABB3D::contains(const vec3& point) const {
	return (point.X >= min.X && point.X <= max.X) &&
		(point.Y >= min.Y && point.Y <= max.Y) &&
		(point.Z >= min.Z && point.Z <= max.Z);
}

Collision2D::Collision2D() : Instance() {

	name = "AABBCollider2D";
	type = InstanceType_Collision2D;

	position = vec2(0.0f, 0.0f);
	size = vec2(1.0f, 1.0f);
	boundingBox = AABB2D(position, position + size);
}

bool Collision2D::checkCollision(Collision2D* other) const {
	return boundingBox.intersects(other->boundingBox);
}

void Collision2D::setPosition(const vec2& pos) {
	position = pos;
	boundingBox = AABB2D(position, position + size);
}

void Collision2D::setSize(const vec2& newSize) {
	size = newSize;
	boundingBox = AABB2D(position, position + size);
}

vec2 Collision2D::getPosition() const {
	return position;
}

vec2 Collision2D::getSize() const {
	return size;
}

vec2 Collision2D::getCenter() const {
	return (position) + size * 0.5f;
}

Collision3D::Collision3D() : Instance()
{
	name = "AABBCollider3D";
	type = InstanceType_Collision3D;
	position = vec3(0.0f, 0.0f, 0.0f);
	size = vec3(1.0f, 1.0f, 1.0f);
	boundingBox = AABB3D(position, position + size);
	allCollisionBoxes.insert(getId());
}

bool Collision3D::checkCollision(Collision3D* other) const {
	return boundingBox.intersects(other->boundingBox);
}

void Collision3D::setPosition(const vec3& pos) {
	position = pos;
	boundingBox = AABB3D(position, position + size);
}

void Collision3D::setSize(const vec3& newSize) {
	size = newSize;
	boundingBox = AABB3D(position, position + size);
}

vec3 Collision3D::getPosition() const {
	return position;
}

vec3 Collision3D::getSize() const {
	return size;
}

vec3 Collision3D::getCenter() const {
	return (position) + size * 0.5f;
}
bool Collision2D::isInside(vec2 point) const {
	return boundingBox.contains(point);
}

bool Collision3D::isInside(vec3 point) const {
	return boundingBox.contains(point);
}

Instance* Collision2D::clone_this() {
	Collision2D* newCollider = (Collision2D*)registerInstance(new Collision2D(), true);
	newCollider->setPosition(this->getPosition());
	newCollider->setSize(this->getSize());
	return newCollider;
}

Instance* Collision3D::clone_this() {
	Collision3D* newCollider = (Collision3D*)registerInstance(new Collision3D(), true);
	newCollider->setPosition(this->getPosition());
	newCollider->setSize(this->getSize());
	return newCollider;
}

int Collision2D::saveInstanceToFile(FILE* file) {
	Instance::saveInstanceToFile(file);
	fwrite(&position, sizeof(vec2), 1, file);
	fwrite(&size, sizeof(vec2), 1, file);
	return 0; //TODO: return the size of the data written to the file
}

int Collision2D::loadInstanceFromFile(void* data, int off) {
	int o = off;
	o += Instance::loadInstanceFromFile(data, o); //Load the base instance data first
	position = *(vec2*)offset(data, o);
	o += sizeof(vec2);
	size = *(vec2*)offset(data, o);
	o += sizeof(vec2);
	boundingBox = AABB2D(position, position + size);
	return o - off; // Return the number of bytes read
}

int Collision3D::saveInstanceToFile(FILE* file) {
	Instance::saveInstanceToFile(file);
	fwrite(&position, sizeof(vec3), 1, file);
	fwrite(&size, sizeof(vec3), 1, file);
	return 0; //TODO: return the size of the data written to the file
}

int Collision3D::loadInstanceFromFile(void* data, int off) {
	int o = off;
	o += Instance::loadInstanceFromFile(data, o); //Load the base instance data first
	position = *(vec3*)offset(data, o);
	o += sizeof(vec3);
	size = *(vec3*)offset(data, o);
	o += sizeof(vec3);
	boundingBox = AABB3D(position, position + size);
	return o - off; // Return the number of bytes read
}

void Collision3D::debugRender() {
	//GL_LINES
	Buffer vertexBuffer;

	vertexBuffer.data = new vec3[24]{
		//Back
		vec3(0, 0, 0),
		vec3(1, 0, 0),

		vec3(1, 0, 0),
		vec3(1, 1, 0),

		vec3(1, 1, 0),
		vec3(0, 1, 0),

		vec3(0, 1, 0),
		vec3(0, 0, 0),

		//Front
		vec3(0, 0, 1),
		vec3(1, 0, 1),

		vec3(1, 0, 1),
		vec3(1, 1, 1),

		vec3(1, 1, 1),
		vec3(0, 1, 1),

		vec3(0, 1, 1),
		vec3(0, 0, 1),

		//Cross
		vec3(0, 0, 0),
		vec3(0, 0, 1),

		vec3(1, 0, 0),
		vec3(1, 0, 1),

		vec3(1, 1, 0),
		vec3(1, 1, 1),

		vec3(0, 1, 0),
		vec3(0, 1, 1),
	};

	vertexBuffer.element_size = sizeof(vec3);
	vertexBuffer.calcSize(24);
	unsigned int vao = 0;

	ShaderProgram* program = (ShaderProgram*)getInstanceById(Scene::getCurrentScene()->hitboxShader);
	program->useProgram();
	vertexBuffer.generate();
	GenVAO(&vao, &vertexBuffer, nullptr, &program->layout);

	Camera* camera = Scene::getCurrentScene()->getCamera();

	mat4 transform = mat4::transform(position, size, vec3(0.0));
	program->setUniformSMat(program->shaderUniformNames[eSUniform_ModelTransformMatrix], transform, GL_TRUE, 4);
	program->setUniform3f(program->shaderUniformNames[eSUniform_CameraPosition], camera->position);
	program->setUniformSMat(program->shaderUniformNames[eSUniform_CameraLookMatrix], camera->look, GL_TRUE, 3);
	program->setUniformSMat(program->shaderUniformNames[eSUniform_CameraProjectionMatrix], camera->projection, GL_TRUE, 4);
	program->setUniform1f(program->shaderUniformNames[eSUniform_CurrentTime], Graphics::getInstance()->lastFrameTime);

	glBindVertexArray(vao);
	glDrawArrays(GL_LINES, 0, 24);
	glBindVertexArray(0);
	glDeleteVertexArrays(1, &vao);

}

void Collision3D::RenderAllCollisionBoxesDebug() {
	for (int i : allCollisionBoxes) {
		//printf("Rendering %d\n", i);
		((Collision3D*)getInstanceById(i))->debugRender();
	}
}
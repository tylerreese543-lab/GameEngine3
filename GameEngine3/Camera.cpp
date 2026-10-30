#include "Camera.h"

#include "Graphics/GraphicsWrapper.h"
#include "Graphics/graphics_include.h"
#include "Graphics/Buffer.h"
#include "Graphics/VertexArrayObject.h"
#include "Scene.h"
Camera::Camera() : Instance() {

	near_plane = 0;
	far_plane = 0;
	aspect_ratio = 1;
	name = "Camera";
	type = InstanceType_Camera;
	setPlanes(0.1, 1000.0f);
	fov = 90;
	isEditorCamera = false;
	isOrthographic = false;
}


Camera::Camera(vec3 v) : Camera() {
	setPosition(v.X, v.Y, v.Z);
}
void Camera::updateMatrices() {

	if (isOrthographic) {
		setMat4Row(projection.m[0],
			fovGL() * aspect_ratio, 0, 0, 0
		);
		setMat4Row(projection.m[1],
			0, fovGL(), 0, 0
		);
		setMat4Row(projection.m[2],
			0, 0, 1.0 / (far_plane - near_plane), -near_plane / (far_plane - near_plane)
		);
		setMat4Row(projection.m[3],
			0, 0, 0, 1.0f
		);
	}
	else {
		setMat4Row(projection.m[0],
			aspect_ratio * fovGL(), 0, 0, 0
		);
		setMat4Row(projection.m[1],
			0, fovGL(), 0, 0
		);
		setMat4Row(projection.m[2],
			0, 0, far_plane / (far_plane - near_plane), -(far_plane * near_plane) / (far_plane - near_plane)
		);
		setMat4Row(projection.m[3],
			0, 0, 1, 0
		);
	}
	look = mat3::yxz(-rotation.X, -rotation.Y, -rotation.Z);
	moveDirection2D = mat3::yrot(-rotation.Y);
}

float Camera::fovGL() {
	return 1 / tanf(fov * (PI / 180.0f) * 0.5f);
}

void Camera::setPlanes(float near, float far) {
	near_plane = near;
	far_plane = far;
}

void Camera::setPosition(float x, float y, float z) {
	position = vec3(x, y, z);
}

int Camera::saveInstanceToFile(FILE* file) {
	Instance::saveInstanceToFile(file);
	fwrite(&position, sizeof(vec3), 1, file);
	fwrite(&rotation, sizeof(vec3), 1, file);
	fwrite(&near_plane, sizeof(float), 1, file);
	fwrite(&far_plane, sizeof(float), 1, file);
	fwrite(&fov, sizeof(float), 1, file);
	fwrite(&aspect_ratio, sizeof(float), 1, file);
	fwrite(&isOrthographic, sizeof(bool), 1, file);
	return 0; //TODO: return the size of the data written to the file
}

int Camera::loadInstanceFromFile(void* data, int off) {
	int o = off;
	o += Instance::loadInstanceFromFile(data, o); //Load the base instance data first
	position = *(vec3*)offset(data, o);
	o += sizeof(vec3);
	rotation = *(vec3*)offset(data, o);
	o += sizeof(vec3);
	near_plane = *(float*)offset(data, o);
	o += sizeof(float);
	far_plane = *(float*)offset(data, o);
	o += sizeof(float);
	fov = *(float*)offset(data, o);
	o += sizeof(float);
	aspect_ratio = *(float*)offset(data, o);
	o += sizeof(float);
	isOrthographic = *(bool*)offset(data, o);
	o += sizeof(bool);
	updateMatrices();
	return o - off; // Return the number of bytes read
}

Instance* Camera::clone_this() {
	Camera* newCamera = (Camera*)registerInstance(new Camera(), true);
	newCamera->position = this->position;
	newCamera->rotation = this->rotation;
	newCamera->near_plane = this->near_plane;
	newCamera->far_plane = this->far_plane;
	newCamera->fov = this->fov;
	newCamera->aspect_ratio = this->aspect_ratio;
	newCamera->isEditorCamera = this->isEditorCamera;
	newCamera->isOrthographic = this->isOrthographic;
	newCamera->updateMatrices();
	return newCamera;
}

void Camera::update(float deltaTime, bool isStudio) {
	updateMatrices();
	aspect_ratio = Graphics::getInstance()->getAspectRatio();
}

int Camera::getNumSaveBytes() {
	return sizeof(vec3) * 2 + sizeof(float) * 4 + Instance::getNumSaveBytes() + sizeof(bool);
}

bool Camera::omitFromSave() {
	return isEditorCamera == true;
}

void Camera::debugRender() {
		//GL_LINES
	Buffer vertexBuffer;

	vertexBuffer.data = new vec3[16]{
		//Back
		vec3(0, 0, 0),
		vec3(1, -1, 1),

		vec3(0, 0, 0),
		vec3(1, 1, 1),

		vec3(0, 0, 0),
		vec3(-1, 1, 1),

		vec3(0, 0, 0),
		vec3(-1, -1, 1),

		vec3(1, -1, 1),
		vec3(1, 1, 1),

		vec3(1, 1, 1),
		vec3(-1, 1, 1),

		vec3(-1, 1, 1),
		vec3(-1, -1, 1),

		vec3(-1, -1, 1),
		vec3(1, -1, 1),
	};

	vertexBuffer.element_size = sizeof(vec3);
	vertexBuffer.calcSize(16);
	unsigned int vao = 0;

	ShaderProgram* program = (ShaderProgram*)getInstanceById(Scene::getCurrentScene()->hitboxShader);
	program->useProgram();
	vertexBuffer.generate();
	GenVAO(&vao, &vertexBuffer, nullptr, &program->layout);

	Camera* camera = Scene::getCurrentScene()->getCamera();

	mat4 transform = mat4::transform(position, vec3(1, 1, 1), rotation);
	program->setUniformSMat(program->shaderUniformNames[eSUniform_ModelTransformMatrix], transform, GL_TRUE, 4);
	program->setUniform3f(program->shaderUniformNames[eSUniform_CameraPosition], camera->position);
	program->setUniformSMat(program->shaderUniformNames[eSUniform_CameraLookMatrix], camera->look, GL_TRUE, 3);
	program->setUniformSMat(program->shaderUniformNames[eSUniform_CameraProjectionMatrix], camera->projection, GL_TRUE, 4);
	program->setUniform1f(program->shaderUniformNames[eSUniform_CurrentTime], Graphics::getInstance()->lastFrameTime);

	glBindVertexArray(vao);
	glDrawArrays(GL_LINES, 0, 16);
	glBindVertexArray(0);
	glDeleteVertexArrays(1, &vao);


}
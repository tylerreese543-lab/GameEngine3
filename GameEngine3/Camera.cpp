#include "Camera.h"

#include "Graphics/GraphicsWrapper.h"
Camera::Camera() {

	near_plane = 0;
	far_plane = 0;
	aspect_ratio = 0;

	fov = 90;
}

Camera::Camera(float x, float y, float z) : Camera() {
	setPosition(x, y, z);
}
void Camera::updateMatrices() {
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

	look = mat3::yxz(rotation.X, rotation.Y, rotation.Z);

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
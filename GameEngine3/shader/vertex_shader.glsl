#version 330 core

layout (location = 0) in vec3 aPos;

uniform mat3 look;
uniform vec3 camera_position;
uniform mat4 projection;

void main() {
	gl_Position = projection * vec4(look * (aPos - camera_position), 1.0);
}
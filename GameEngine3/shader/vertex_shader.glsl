#version 330 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aColor;
uniform mat3 look;
uniform vec3 camera_position;
uniform mat4 projection;

out vec3 out_aColor;
void main() {
	out_aColor = aColor;
	gl_Position = projection * vec4(look * (aPos - camera_position), 1.0);
}
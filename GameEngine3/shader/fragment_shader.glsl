#version 330 core

in vec3 out_aColor;
out vec4 color;

void main() {
	color = vec4(out_aColor, 1.0);
}
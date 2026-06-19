#version 330 core

in vec3 out_aColor;
out vec4 color;

uniform sampler2D tex;
void main() {
	color = texture(tex, out_aColor.xy);
}
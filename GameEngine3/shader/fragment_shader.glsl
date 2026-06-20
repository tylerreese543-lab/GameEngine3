#version 330 core

in vec3 out_color;
in vec2 out_texCoord;
out vec4 color;

uniform sampler2D tex;
void main() {
	color = vec4(out_color * texture(tex, out_texCoord).rgb, 1.0);
}
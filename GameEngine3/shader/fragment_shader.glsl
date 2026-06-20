#version 330 core

in vec3 out_color;
in vec2 out_texCoord;
out vec4 color;

uniform sampler2D t_Diffuse;
uniform vec3 m_DiffuseColor;
void main() {
	color = vec4(m_DiffuseColor * texture(t_Diffuse, out_texCoord).rgb, 1.0);
}
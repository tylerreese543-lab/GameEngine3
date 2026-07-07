#version 330 core

in vec4 out_color;
in vec2 out_texCoord;
in vec3 out_normal;
out vec4 color;

uniform sampler2D t_Diffuse;
uniform sampler2D t_AmbientOcclusion;
uniform vec3 m_DiffuseColor;
void main() {
	color = vec4(m_DiffuseColor * texture(t_AmbientOcclusion, out_texCoord).r, 1.0);
}
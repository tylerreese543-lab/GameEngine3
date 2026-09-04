#version 330 core

out vec4 color;

uniform sampler2D t_Diffuse;
uniform sampler2D t_AmbientOcclusion;
uniform vec3 m_DiffuseColor;
void main() {
	
	color = vec4(1.);
}
#version 330 core

out vec4 color;

uniform sampler2D t_Diffuse;
uniform sampler2D t_AmbientOcclusion;
uniform vec3 m_DiffuseColor;
void main() {
	//-315 -206 PORTAL
	//-286 -25 WARPED
	color = vec4(1.);
}
#version 330 core

in vec4 out_color;
in vec2 out_texCoord;
in vec3 out_normal;
in float out_layer;
out vec4 color;

uniform sampler2D t_Diffuse;
uniform sampler2D t_AmbientOcclusion;
uniform vec3 m_DiffuseColor;
uniform sampler2DArray tx_TextGlyphs;
void main() {

	
	vec3 uvw = vec3(out_texCoord, out_layer);
	float lum = texture(tx_TextGlyphs, uvw).r;
	color = vec4(m_DiffuseColor, lum);
}
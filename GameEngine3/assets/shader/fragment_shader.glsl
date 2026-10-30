#version 330 core

in vec4 out_color;
in vec2 out_texCoord;
in vec3 out_normal;
out vec4 color;

uniform sampler2D t_Diffuse;
uniform bool m_HasDiffuseTexture;
uniform sampler2D t_AmbientOcclusion;
uniform bool m_HasAmbientOcclusionTexture;
uniform vec3 m_DiffuseColor;
void main() {
	
	vec3 col = m_DiffuseColor;
	if (m_HasDiffuseTexture) {
		col = m_DiffuseColor * texture(t_Diffuse, out_texCoord).rgb;
	}
	float AOValue = 1.;
	if (m_HasAmbientOcclusionTexture) {
		AOValue = texture(t_AmbientOcclusion, out_texCoord).r;
	}
	color = vec4(col * AOValue, 0.1);
}
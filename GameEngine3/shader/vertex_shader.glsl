#version 330 core
in vec3 m_Color;
in vec3 m_Position;
in vec2 m_TexCoord;


uniform mat3 c_LookMatrix;
uniform vec3 c_Position;
uniform mat4 c_ProjectionMatrix;
uniform mat4 m_Transform;
out vec3 out_color;
out vec2 out_texCoord;
void main() {
	out_color = m_Color;
	out_texCoord = m_TexCoord;
	gl_Position = c_ProjectionMatrix * vec4(c_LookMatrix * ((m_Transform * vec4(m_Position, 1.0)).xyz - c_Position), 1.0);
}
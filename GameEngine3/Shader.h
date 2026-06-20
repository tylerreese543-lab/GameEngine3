#pragma once
#include <string>
#include <stdio.h>

#include "Graphics/Graphics.h"
#include "Graphics/BufferLayout.h"
#include "Graphics/matrix.h"
#include "Graphics/GraphicsWrapper.h"
#include "TextureResource.h"
#include "Resource.h"
class ShaderResource : public Resource {
public:
	GLuint id;
	GLenum shaderType;
	char* source;
	std::string filePath;
	int compileResult;
	bool compiled;
	int loadFromFile(const char* filepath);
	int compileShader();
	bool isCompiled();
	bool isValid();
	ShaderResource();
	ShaderResource(GLenum type);
	ShaderResource(GLenum type, const char* source);

};//aBus was here

class ShaderProgram : public Resource {
private:
	int U(std::string uniform);
public:
	std::string shaderAttributeNames[eVAttribute_MAX];
	std::string shaderUniformNames[eSUniform_MAX];
	std::string shaderTextureNames[eMaterialTexture_MAX];
	bool loadInterleaved;
	//All the shaders that these programs will use will actually be the instance's children.
	GLuint id;
	BufferLayout layout; //Generated from the vertex shader's input layout
	int attachShader(ShaderResource* shader);
	int linkProgram();
	int createProgram();
	int useProgram();

	void setDefaultVertexAttribute(int index, std::string name);
	void setDefaultShaderUniform(int index, std::string name);
	void setDefaultMaterialTexture(int index, std::string name);
	//Uniform
	void bindTexture(std::string uniform, ImageResource* tex);
	void setUniform1f(std::string uniform, float f0);
	void setUniform2f(std::string uniform, float f0, float f1);
	void setUniform2f(std::string uniform, vec2 v);
	void setUniform3f(std::string uniform, float f0, float f1, float f2);
	void setUniform3f(std::string uniform, vec3 v);
	void setUniform4f(std::string uniform, float f0, float f1, float f2, float f3);
	void setUniform4f(std::string uniform, vec4 v);
	void setUniformSMat(std::string uniform, const float* fv, bool transposed, int size);
	~ShaderProgram();
	ShaderProgram();
};
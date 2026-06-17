#include "Shader.h"



ShaderResource::ShaderResource() : Resource() {
	name = "Shader";
	type = InstanceType_ShaderResource;
	id = 0;
	compiled = false;
	compileResult = 0;
	source = nullptr;
}

ShaderResource::ShaderResource(GLenum type) : ShaderResource() {
	this->shaderType = type;
}
ShaderResource::ShaderResource(GLenum type, const char* source) : ShaderResource() {
	this->shaderType = type;
	this->source = (char*)source;
}

int ShaderResource::loadFromFile(const char* filepath) {
	compiled = false;
	filePath = filepath;
	FILE* file;
	fopen_s(&file, filepath, "rb");
	if (!file) {
		printf("ERROR: Failed to open shader file '%s'.\n", filepath);
		return -1;
	}
	fseek(file, 0, SEEK_END);
	size_t size = ftell(file);
	fseek(file, 0, SEEK_SET);
	source = new char[size + 1];
	fread(source, sizeof(char), size, file);
	source[size] = '\0';
	fclose(file);
	return 0;
}


int ShaderResource::compileShader() {
	compiled = true;
	id = glCreateShader(shaderType);
	glShaderSource(id, 1, &source, nullptr);
	glCompileShader(id);
	glGetShaderiv(id, GL_COMPILE_STATUS, &compileResult);
	if (compileResult != GL_TRUE) {
		int infoLogLength;
		glGetShaderiv(id, GL_INFO_LOG_LENGTH, &infoLogLength);
		char* message = new char[infoLogLength];
		glGetShaderInfoLog(id, infoLogLength, &infoLogLength, message);
		printf("ERROR: Failed to compile shader '%s': %s\n", filePath.c_str(), message);
		delete[] message;
		return -1;
	}
	return 0;
}

bool ShaderResource::isCompiled() {
	return compiled;
}

bool ShaderResource::isValid() {
	return compiled && (compileResult == GL_TRUE);
}

ShaderProgram::ShaderProgram() : Resource() {
	name = "ShaderProgram";
	type = InstanceType_ShaderProgram;
	id = 0;
}
ShaderProgram::~ShaderProgram() {
	glDeleteProgram(id);
}

int ShaderProgram::attachShader(ShaderResource* shader) {
	if (!shader->isValid()) {
		printf("ERROR: Cannot attach invalid shader '%s' to program '%s'.\n", shader->name.c_str(), name.c_str());
		return -1;
	}
	else {
		printf("Shader '%s' is valid.\n", shader->name.c_str());
	}
	glAttachShader(id, shader->id);
	return 0;
}

int ShaderProgram::linkProgram() {
	glLinkProgram(id);
	GLint linkResult;
	glGetProgramiv(id, GL_LINK_STATUS, &linkResult);
	if (linkResult != GL_TRUE) {
		int infoLogLength;
		glGetProgramiv(id, GL_INFO_LOG_LENGTH, &infoLogLength);
		char* message = new char[infoLogLength];
		glGetProgramInfoLog(id, infoLogLength, &infoLogLength, message);
		printf("ERROR: Failed to link shader program [ID %d] '%s': %s\n",id, name.c_str(), message);
		delete[] message;
		return -1;
	}
	return 0;
}

int ShaderProgram::createProgram() {
	id = glCreateProgram();
	std::vector<ShaderResource*> shaders;
	for (Instance* child : children) {
		if (child->type != InstanceType_ShaderResource) {
			continue;
		}
		shaders.push_back(dynamic_cast<ShaderResource*>(child));
	}

	for (ShaderResource* shader : shaders) {
		if (!shader->isCompiled()) shader->compileShader();
		attachShader(shader);
	}
	linkProgram();
	for (ShaderResource* shader : shaders) {
		glDeleteShader(shader->id);
		shader->id = -1;
	}

	GLint numActiveAttribs = 0;
	glGetProgramiv(id, GL_ACTIVE_ATTRIBUTES, &numActiveAttribs);

// Query the maximum name length to allocate an appropriately sized buffer
	GLint maxNameLength = 0;
	glGetProgramiv(id, GL_ACTIVE_ATTRIBUTE_MAX_LENGTH, &maxNameLength);
// Create a buffer for the attribute names
	char* nameBuffer = new char[maxNameLength + 1];
	layout.clear();
	layout.elements.assign(numActiveAttribs, BufferLayoutElement());
	for (GLint i = 0; i < numActiveAttribs; ++i) {
		GLsizei actualLength = 0;
		GLint size = 0;
		GLenum type = 0;
		int count = 1;
    // Fetch attribute metadata (type, size, and text name)
		glGetActiveAttrib(id, i, maxNameLength, &actualLength, &size, &type, nameBuffer);
    // Fetch the actual layout/location index bound to this attribute
		GLint location = glGetAttribLocation(id, nameBuffer);
		if (type == GL_FLOAT_VEC3) {
			type = GL_FLOAT;
			count = 3;
		}

		layout.elements[location] = BufferLayoutElement(type, count);
	}
	delete[] nameBuffer;
	return 0;
}

int ShaderProgram::useProgram() {
	glUseProgram(id);
	return 0;
}
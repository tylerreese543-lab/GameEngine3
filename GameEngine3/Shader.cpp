#include "Shader.h"


ShaderResource::ShaderResource() : Resource() {
	name = "Shader";
	type = InstanceType_ShaderResource;
	id = 0;
	compiled = false;
	compileResult = 0;
	source = nullptr;
	shaderType = GL_VERTEX_SHADER;

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
	FILE* file = openResourceFile(filepath);
	if (!file) return -1;
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

	if (id != 0) {
		glDeleteShader(id);
		id = 0;
	}
	id = glCreateShader(shaderType);
	if (!source) {
		printf("ERROR: No source code provided for shader '%s'.\n", filePath.c_str());
		return -1;
	}
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
	loadInterleaved = false;
	
	setDefaultVertexAttribute(eVAttribute_Position,					"m_Position");
	setDefaultVertexAttribute(eVAttribute_UV,						"m_TexCoord");
	setDefaultVertexAttribute(eVAttribute_Normal,					"m_Normal");
	setDefaultVertexAttribute(eVAttribute_Color,					"m_Color");

	setDefaultShaderUniform(eSUniform_CameraLookMatrix,				"c_LookMatrix");
	setDefaultShaderUniform(eSUniform_CameraPosition,				"c_Position");
	setDefaultShaderUniform(eSUniform_ModelTransformMatrix,			"m_Transform");
	setDefaultShaderUniform(eSUniform_CameraProjectionMatrix,		"c_ProjectionMatrix");
	setDefaultShaderUniform(eSUniform_Material_DiffuseColor,		"m_DiffuseColor");
	setDefaultShaderUniform(eSUniform_Material_Specularity,			"m_Specularity");
	setDefaultShaderUniform(eSUniform_CurrentTime,					"u_CurrentTime");

	setDefaultMaterialTexture(eMaterialTexture_Diffuse,				"t_Diffuse");
	setDefaultMaterialTexture(eMaterialTexture_AmbientOcclusion,	"t_AmbientOcclusion");
	setDefaultMaterialTexture(eMaterialTexture_Normal,				"t_Normal");
	setDefaultMaterialTexture(eMaterialTexture_Specular,			"t_Specular");
}

ShaderProgram::ShaderProgram(std::string vs, std::string fs) : ShaderProgram() {
	ShaderResource* vertex_shader = new ShaderResource(GL_VERTEX_SHADER);
	vertex_shader->loadFromFile(vs.c_str());
	vertex_shader->name = "Vertex Shader";
	ShaderResource* fragment_shader = new ShaderResource(GL_FRAGMENT_SHADER);
	fragment_shader->loadFromFile(fs.c_str());
	fragment_shader->name = "Fragment Shader";
	addChild(vertex_shader);
	addChild(fragment_shader);
}
ShaderProgram::~ShaderProgram() {
	glDeleteProgram(id);
}

int ShaderProgram::attachShader(ShaderResource* shader) {
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

	if (id != 0) {
		glDeleteProgram(id);
		id = 0;
	}
	id = glCreateProgram();
	printf("Creating program...\n");
	std::vector<ShaderResource*> shaders;
	for (Instance* child : children) {
		if (child->type != InstanceType_ShaderResource) {
			continue;
		}
		shaders.push_back(dynamic_cast<ShaderResource*>(child));
	}

	for (ShaderResource* shader : shaders) {
		shader->compileShader();
		attachShader(shader);
	}
	linkProgram();
	for (ShaderResource* shader : shaders) {
		glDeleteShader(shader->id);
		shader->id = 0;
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
		if (type == GL_FLOAT_VEC4) {
			type = GL_FLOAT;
			count = 4;
		}
		if (type == GL_FLOAT_VEC3) {
			type = GL_FLOAT;
			count = 3;
		}
		if (type == GL_FLOAT_VEC2) {
			type = GL_FLOAT;
			count = 2;
		}
		if (location >= 0) {
			layout.elements[location] = BufferLayoutElement(type, count);
			if (loadInterleaved) {
				layout.elements[location].binding = eVBO_Positions;
			}
			else {
				//Set bindings to hard-coded indexes in the Mesh's VBO;
				if (strcmp(nameBuffer, shaderAttributeNames[eVAttribute_Position].c_str()) == 0) {
					layout.elements[location].binding = eVBO_Positions;
				}
				else if (strcmp(nameBuffer, shaderAttributeNames[eVAttribute_UV].c_str()) == 0) {
					layout.elements[location].binding = eVBO_UV;
				}
				else if (strcmp(nameBuffer, shaderAttributeNames[eVAttribute_Normal].c_str()) == 0) {
					layout.elements[location].binding = eVBO_Normal;
				}
				else if (strcmp(nameBuffer,  shaderAttributeNames[eVAttribute_Color].c_str()) == 0) {
					layout.elements[location].binding = eVBO_Color;
				}
				else {
					layout.elements[location].binding = eVBO_Extra;
				}
			}
			//printf("Element '%s' bound to VBO %d at location %d.\n", nameBuffer, layout.elements[location].binding, location);
		}
	
		else {
			printf("WARNING: attribute '%s' has a binding of %d.\n", nameBuffer, location);
		}
	}
	delete[] nameBuffer;
	return 0;
}

int ShaderProgram::useProgram() {
	glUseProgram(id);
	return 0;
}

int ShaderProgram::U(std::string uniform) {
	if (glGetUniformLocation(id, uniform.c_str()) == -1) {
		//Removed - developer allowed to intentionally exclude uniforms from shaders
		//printf("Uniform '%s' does not exist, or is not used in the shader program.\n", uniform.c_str());
	}
	return glGetUniformLocation(id, uniform.c_str());
}
void ShaderProgram::setUniform1f(std::string uniform, float f0) {
	glUniform1f(U(uniform), f0);
}
void ShaderProgram::setUniform2f(std::string uniform, float f0, float f1) {
	glUniform2f(U(uniform), f0, f1);
}
void ShaderProgram::setUniform3f(std::string uniform, float f0, float f1, float f2) {
	glUniform3f(U(uniform), f0, f1, f2);
}
void ShaderProgram::setUniform4f(std::string uniform, float f0, float f1, float f2, float f3) {
	glUniform4f(U(uniform), f0, f1, f2, f3);
}
void ShaderProgram::setUniform2f(std::string uniform, vec2 v) {
	glUniform2f(U(uniform), v.X, v.Y);
}
void ShaderProgram::setUniform3f(std::string uniform, vec3 v) {
	glUniform3f(U(uniform), v.X, v.Y, v.Z);
}
void ShaderProgram::setUniform4f(std::string uniform, vec4 v) {
	glUniform4f(U(uniform), v.X, v.Y, v.Z, v.W);
}
void ShaderProgram::setUniformSMat(std::string uniform, const float* fv, bool transposed, int size) {
	switch (size) {
	case 4:
		glUniformMatrix4fv(U(uniform), 1, transposed, fv);
		break;
	case 3:
		glUniformMatrix3fv(U(uniform), 1, transposed, fv);
		break;
	case 2:
		glUniformMatrix2fv(U(uniform), 1, transposed, fv);
	default:
		printf("Not a valid square matrix size.\n");
		break;
	}
}

void ShaderProgram::bindTexture(std::string uniform, ImageResource* tex, int type) {
	int textureSlot = -1;
	if (U(uniform) == -1) return;
	//printf("%s -%d\n", uniform.c_str(), U(uniform));
	textureSlot = type;
	if (tex && textureSlot != -1) {
		if (tex->textureType == type || type == -1) {
			tex->BindSlot(textureSlot);
			glUniform1i(U(uniform), textureSlot);
		}
	}
}
void ShaderProgram::unbindTexture(std::string uniform, int type) {
	int textureSlot = -1;
	if (U(uniform) == -1) return;
	glGetUniformiv(id, U(uniform), &textureSlot);
	glActiveTexture(GL_TEXTURE0 + type);
	glBindTexture(GL_TEXTURE_2D, 0);
}
void ShaderProgram::setDefaultVertexAttribute(int index, std::string name) {
	if (index >= 0 && index < eVAttribute_MAX) {
		shaderAttributeNames[index] = name;
	}
}

void ShaderProgram::setDefaultShaderUniform(int index, std::string name) {
	if (index >= 0 && index < eSUniform_MAX) {
		shaderUniformNames[index] = name;
	}
}

void ShaderProgram::setDefaultMaterialTexture(int index, std::string name) {
	if (index >= 0 && index < eMaterialTexture_MAX) {
		shaderTextureNames[index] = name;
	}
}

Instance* ShaderResource::clone_this() {
	ShaderResource* s = (ShaderResource*)registerInstance(new ShaderResource(*this), true);
	s->id = 0;
	s->compiled = false;
	s->compileResult = 0;
	s->filePath = filePath;
	if (source != nullptr) {
		size_t sourceLength = strlen(source);
		s->source = new char[sourceLength + 1];
		strcpy_s(s->source, sourceLength + 1, source);
	}
	else {
		s->source = nullptr;
	}
	return s;
}

ShaderResource::~ShaderResource() {
	if (source != nullptr) {
		delete[] source;
		source = nullptr;
	}
	if (id != 0) {
		glDeleteShader(id);
		id = 0;
	}
}

int ShaderResource::saveInstanceToFile(FILE* file) {
	int offset = 0;
	offset += Resource::saveInstanceToFile(file);
	offset += fwrite(&shaderType, sizeof(GLenum), 1, file);
	offset += fwrite(filePath.data(), filePath.size() + 1, 1, file);
	offset += fwrite(&compileResult, sizeof(int), 1, file);
	offset += fwrite(&compiled, sizeof(bool), 1, file);
	if (source != nullptr) {
		size_t sourceLength = strlen(source);
		offset += fwrite(source, sizeof(char), sourceLength + 1, file);
	}
	else {
		char emptySource = '\0';
		offset += fwrite(&emptySource, sizeof(char), 1, file);
	}
	return offset;
}

int ShaderResource::loadInstanceFromFile(void* data, int off) {
	int o = off;
	o += Resource::loadInstanceFromFile(data, o);
	memcpy(&shaderType, offset(data, o), sizeof(GLenum));
	o += sizeof(GLenum);
	char filePathBuffer[256];
	strcpy_s(filePathBuffer, sizeof(filePathBuffer), (char*)offset(data, o));
	filePath = std::string(filePathBuffer);
	o += strlen(filePathBuffer) + 1;
	memcpy(&compileResult, offset(data, o), sizeof(int));
	o += sizeof(int);
	memcpy(&compiled, offset(data, o), sizeof(bool));
	o += sizeof(bool);
	if (source != nullptr) {
		delete[] source;
		source = nullptr;
	}
	size_t sourceLength = strlen((char*)offset(data, o));
	if (sourceLength > 0) {
		source = new char[sourceLength + 1];
		strcpy_s(source, sourceLength + 1, (char*)offset(data, o));
	}
	else {
		source = nullptr;
	}
	o += sourceLength + 1;
	return o - off;
}

int ShaderProgram::saveInstanceToFile(FILE* file) {
	int offset = 0;
	offset += Resource::saveInstanceToFile(file);
	offset += fwrite(&loadInterleaved, sizeof(bool), 1, file);
	for(int i = 0; i < eVAttribute_MAX; ++i) {
		offset += fwrite(shaderAttributeNames[i].data(), shaderAttributeNames[i].size() + 1, 1, file);
	}
	for(int i = 0; i < eSUniform_MAX; ++i) {
		offset += fwrite(shaderUniformNames[i].data(), shaderUniformNames[i].size() + 1, 1, file);
	}
	for(int i = 0; i < eMaterialTexture_MAX; ++i) {
		offset += fwrite(shaderTextureNames[i].data(), shaderTextureNames[i].size() + 1, 1, file);
	}
	return offset;
}

int ShaderProgram::loadInstanceFromFile(void* data, int off) {
	int o = off;
	for (int i = 0; i < eVAttribute_MAX; ++i) {
		shaderAttributeNames[i] = std::string((char*)offset(data, o));
		o += shaderAttributeNames[i].size() + 1;
	}
	for (int i = 0; i < eSUniform_MAX; ++i) {
		shaderUniformNames[i] = std::string((char*)offset(data, o));
		o += shaderUniformNames[i].size() + 1;
	}
	for (int i = 0; i < eMaterialTexture_MAX; ++i) {
		shaderTextureNames[i] = std::string((char*)offset(data, o));
		o += shaderTextureNames[i].size() + 1;
	}
	return o - off;
}
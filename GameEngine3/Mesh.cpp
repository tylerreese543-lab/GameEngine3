#include "Mesh.h"
std::set<Mesh*> Mesh::render_objects;
Geometry::Geometry() : Resource() {
	name = "Geometry";
	type = InstanceType_Geometry;
	isGeometryInterleaved = false;
	clearBuffers();
	setVBOFlags(false, false, false, false, false);
}

Geometry::~Geometry() {
	//TODO: delete vbos
}

Instance* Geometry::clone_this() {
	Geometry* g = (Geometry*)registerInstance(new Geometry(*this), true);
	g->vao = 0;
	g->clearBuffers();
	memcpy(g->vboFlags, vboFlags, eVBO_MAX);
	for (int i = 0; i < eVBO_Extra; ++i) {
		g->vbos[i].size = vbos[i].size;
		g->vbos[i].target = vbos[i].target;
		g->vbos[i].usage = vbos[i].usage;
		g->vbos[i].element_size = vbos[i].element_size;
		if (vbos[i].data != nullptr) {
			g->vbos[i].data = malloc(vbos[i].size);
			memcpy(g->vbos[i].data, vbos[i].data, vbos[i].size);
		}
	}
	if (ebo.data != nullptr) {
		g->ebo.size = ebo.size;
		g->ebo.target = ebo.target;
		g->ebo.usage = ebo.usage;
		g->ebo.element_size = ebo.element_size;
		g->ebo.data = malloc(ebo.size);
		memcpy(g->ebo.data, ebo.data, ebo.size);
	}
	return g;
}
int Geometry::generateVAO(BufferLayout* layout) {
	autoSearchVBO(); //AutoSearchVBO SHOULD be fine to auto-call because there would be no other way to initialize them...
	ebo.generate();
	for (int i = eVBO_Positions; i < eVBO_MAX; ++i) {
		if (vboFlags[i]) {
			vbos[i].generate();
		}
	}

	if (ebo.data == nullptr) {
		return GenVAO(&vao, vbos, nullptr, layout, eVBO_MAX);
	}
	else {
		return GenVAO(&vao, vbos, &ebo, layout, eVBO_MAX);
	}
}

void Geometry::render() {
	if (vao != 0) {
		DrawVAOElements(vao, GL_UNSIGNED_INT, (int)(ebo.size / ebo.element_size));
	}
}

void ProceduralGeometry::render() {
	if (vao != 0) {
		if (drawElements) {
			DrawVAOElements(vao, GL_UNSIGNED_INT, (int)(ebo.size / ebo.element_size));
		} else {
			DrawVAOArrays(vao, 0, (int)vertices.size());
		}
	}
}

void Geometry::setVBOFlags(bool hasPositions, bool hasUV, bool hasNormal, bool hasColor, bool hasExtra) {
	vboFlags[eVBO_Positions] = hasPositions;
	vboFlags[eVBO_UV] = hasUV;
	vboFlags[eVBO_Normal] = hasNormal;
	vboFlags[eVBO_Color] = hasColor;
	vboFlags[eVBO_Extra] = hasExtra;
}

void Geometry::clearBuffers() {
	for (int i = 0; i < eVBO_MAX; ++i) {
		vbos[i].data = nullptr;
		vbos[i].element_size = getSizeOfType(GL_FLOAT);
		vbos[i].target = GL_ARRAY_BUFFER;
		vbos[i].usage = GL_STATIC_DRAW;
		vbos[i].size = 0;
	}

	ebo.element_size = getSizeOfType(GL_UNSIGNED_INT);
	ebo.target = GL_ELEMENT_ARRAY_BUFFER;
	ebo.usage = GL_STATIC_DRAW;
	ebo.data = nullptr;
	ebo.size = 0;
}
void ProceduralGeometry::clearBuffers() {
	for (int i = 0; i < eVBO_MAX; ++i) {
		vbos[i].data = nullptr;
		vbos[i].element_size = getSizeOfType(GL_FLOAT);
		vbos[i].target = GL_ARRAY_BUFFER;
		vbos[i].usage = bufferUpdateMode;
		vbos[i].size = 0;
	}

	ebo.element_size = getSizeOfType(GL_UNSIGNED_INT);
	ebo.target = GL_ELEMENT_ARRAY_BUFFER;
	ebo.usage = bufferUpdateMode;
	ebo.data = nullptr;
	ebo.size = 0;
}
Mesh::Mesh() : Instance() {
	setPosition(vec3(0, 0, 0));
	setRotation(vec3(0, 0, 0));
	setScale(vec3(1));
	
	type = InstanceType_Mesh;
	name = "Mesh";
	geometry = -1;
	material = -1;
	visible = true;

	render_objects.insert(this);
}

void Mesh::setPosition(vec3 pos) {
	position = pos;
}

void Mesh::setRotation(vec3 rot) {
	rotation = rot;
}

void Mesh::setScale(vec3 scl) {
	scale = scl;
}

void Mesh::calcTransform() {
	transform = mat4::transform(position, scale, rotation);
}

void Mesh::render(Camera* camera) {
	
	if (!visible) return;
	if (!isIdValid(geometry)) {
		//printf("Finding geometry\n");
		geometry = getChildIdFromType(InstanceType_Geometry);
	}
	if (!isIdValid(geometry)) {
		//printf("Finding procedural geometry\n");
		geometry = getChildIdFromType(InstanceType_ProceduralGeometry);
	}
	if (!isIdValid(material)) {
		material = getChildIdFromType(InstanceType_Material);
	}
	if (geometry == -1) {
		//printf("Abort rendering of '%s' - has no geometry bound.\n", name.c_str());
		return;
	}
	if (material == -1) {
		//printf("Abort rendering of '%s' - has no material bound.\n", name.c_str());
		return;
	}
	if (camera == nullptr) {
		//printf("Abort rendering of '%s' - has no camera attached.\n", name.c_str());
		return;
	}
	Material* m = (Material*)getInstanceById(material);
	ShaderProgram* program = (ShaderProgram*)getInstanceById(m->getShaderInstanceId());
	if (!program) {
		printf("Abort rendering of '%s' - material has no shader program attached.\n", name.c_str());
		return;
	}

	Geometry* g = (Geometry*)getInstanceById(geometry);

	if (!isInstanceOf(g->type, InstanceType_Geometry)) return;
	if (g->vao == 0) {
		g->generateVAO(&program->layout);
	}
	//printf("VAO for ID: %d: %d\n", geometry, g->vao);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_FRONT);
	calcTransform();
	m->applyMaterial();
	program->setUniformSMat(program->shaderUniformNames[eSUniform_ModelTransformMatrix], transform, GL_TRUE, 4);
	program->setUniform3f(program->shaderUniformNames[eSUniform_CameraPosition], camera->position);
	program->setUniformSMat(program->shaderUniformNames[eSUniform_CameraLookMatrix], camera->look, GL_TRUE, 3);
	program->setUniformSMat(program->shaderUniformNames[eSUniform_CameraProjectionMatrix], camera->projection, GL_TRUE, 4);
	program->setUniform1f(program->shaderUniformNames[eSUniform_CurrentTime], Graphics::getInstance()->lastFrameTime);
	g->render();
	
}

void Geometry::autoSearchVBO() {
	for (int i = 0; i < eVBO_MAX; ++i) {
		vboFlags[i] = (vbos[i].data != nullptr);
	}
}

void Geometry::loadFromFile(std::string filepath, int meshID, int ImporterFlags) {
	Assimp::Importer importer;
	filePath = filepath;
	const aiScene* scene = importer.ReadFile(resolveFilePath(filepath), ImporterFlags);
	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
		std::cout << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
		return;
	}

	if (meshID < 0 || meshID >= scene->mNumMeshes) meshID = 0;
	aiMesh* mesh = scene->mMeshes[meshID];

	int sizePos = mesh->mNumVertices * 3;
	int sizeUV = mesh->mNumVertices * 2;
	int sizeNormal = mesh->mNumVertices * 3;
	int sizeColor = mesh->mNumVertices * 3;
	int sizeIndices = mesh->mNumFaces * 3;

	//TODO: add interleaved load capabilties
	if (mesh->HasPositions()) {
		vbos[eVBO_Positions].data = malloc(sizePos * sizeof(float));
		vbos[eVBO_Positions].calcSize(sizePos);
		memcpy(vbos[eVBO_Positions].data, mesh->mVertices, vbos[eVBO_Positions].size);
	}
	//Automatically use the 1st texture set
	if (mesh->HasTextureCoords(0)) {
		vbos[eVBO_UV].data = malloc(sizeUV * sizeof(float));
		vbos[eVBO_UV].calcSize(sizeUV);
		
		float* float_data = (float*)vbos[eVBO_UV].data;
		for (int i = 0; i < mesh->mNumVertices; ++i) {
			float_data[(i * 2) + 0] = mesh->mTextureCoords[0][i].x;
			float_data[(i * 2) + 1] = mesh->mTextureCoords[0][i].y;
		}
	}
	if (mesh->HasNormals()) {
		vbos[eVBO_Normal].data = malloc(sizeNormal * sizeof(float));
		vbos[eVBO_Normal].calcSize(sizeNormal);
		memcpy(vbos[eVBO_Normal].data, mesh->mNormals, vbos[eVBO_Normal].size);
	}
	if (mesh->HasVertexColors(0)) {
		vbos[eVBO_Color].data = malloc(sizeColor * sizeof(float));
		vbos[eVBO_Color].calcSize(sizeColor);
		memcpy(vbos[eVBO_Color].data, mesh->mColors[0], vbos[eVBO_Color].size);
	}

	if (mesh->HasFaces()) {
		ebo.data = malloc(sizeIndices * sizeof(unsigned int));
		ebo.calcSize(sizeIndices);

		unsigned int* int_data = (unsigned int*)ebo.data;
		for (int i = 0; i < mesh->mNumFaces; ++i) {
			int_data[(i * 3) + 0] = mesh->mFaces[i].mIndices[0];
			int_data[(i * 3) + 1] = mesh->mFaces[i].mIndices[1];
			int_data[(i * 3) + 2] = mesh->mFaces[i].mIndices[2];
		}
	}

	if (mesh->HasTangentsAndBitangents()) {
		//TODO: add support for tangents/bitangents in the import in the extra VBO
	}
	/*
	TODO: Actually expand these dynamically to fit any extra UV / Color data in the mesh file.
	Gives more creative freedom to developers in shaders
	*/

	autoSearchVBO();
}

int Geometry::saveInstanceToFile(FILE* file) {
	int offset = 0;
	offset += Resource::saveInstanceToFile(file);
	offset += fwrite(filePath.data(), filePath.size() + 1, 1, file);
	offset += fwrite(&isGeometryInterleaved, sizeof(bool), 1, file);
	offset += fwrite(&vboFlags, sizeof(bool), eVBO_MAX, file);
	return offset;
}

int Geometry::loadInstanceFromFile(void* data, int off) {
	int o = off;
	o += Resource::loadInstanceFromFile(data, o);
	char filePathBuffer[128];
	strcpy_s(filePathBuffer, sizeof(filePathBuffer), (char*)offset(data, o));
	filePath = std::string(filePathBuffer);
	o += strlen(filePathBuffer) + 1;
	memcpy(&isGeometryInterleaved, offset(data, o), sizeof(bool));
	o += sizeof(bool);
	memcpy(&vboFlags, offset(data, o), sizeof(bool) * eVBO_MAX);
	o += sizeof(bool) * eVBO_MAX;
	//TODO: store/load importer flags/mesh ID but for now just use the defaults
	loadFromFile(filePath);
	return  o - off;
}
void Mesh::renderAllMeshes(Camera* camera) {
	if (!camera) return;
	for (Mesh* m : render_objects) {
		m->render(camera);
	}
}

int Mesh::destroy() {
	render_objects.erase(this);
	return Instance::destroy();
}

Instance* Mesh::clone_this() {
	Mesh* i = new Mesh(*this);
	render_objects.insert(i);
	return registerInstance(i, true);
}

int Mesh::saveInstanceToFile(FILE* file) {
	int offset = 0;
	offset += Instance::saveInstanceToFile(file);
	offset += fwrite(&position, sizeof(vec3), 1, file);
	offset += fwrite(&rotation, sizeof(vec3), 1, file);
	offset += fwrite(&scale, sizeof(vec3), 1, file);
	offset += fwrite(&geometry, sizeof(int), 1, file);
	offset += fwrite(&material, sizeof(int), 1, file);
	offset += fwrite(&visible, sizeof(bool), 1, file);
	return offset;
}

int Mesh::loadInstanceFromFile(void* data, int off) {

	int o = off;
	o += Instance::loadInstanceFromFile(data, off);
	position = *(vec3*)offset(data, o);
	o += sizeof(vec3);
	rotation = *(vec3*)offset(data, o);
	o += sizeof(vec3);
	scale = *(vec3*)offset(data, o);
	o += sizeof(vec3);
	geometry = *(int*)offset(data, o);
	o += sizeof(int);
	material = *(int*)offset(data, o);
	o += sizeof(int);
	visible = *(bool*)offset(data, o);
	o += sizeof(bool);
	return o - off;
}

ProceduralGeometry::ProceduralGeometry() : Geometry() {
	name = "ProceduralGeometry";
	type = InstanceType_ProceduralGeometry;
	numExtraFloats = 0;
	bufferUpdateMode = GL_DYNAMIC_DRAW;
}

ProceduralGeometry::~ProceduralGeometry() {
	clearData();
}

ProceduralVertex::ProceduralVertex(vec3 position, vec3 normal, vec2 uv, vec3 color) {
	this->position = position;
	this->normal = normal;
	this->uv = uv;
	this->color = color;
}
void ProceduralGeometry::addVertex(ProceduralVertex vertex) {
	vertices.push_back(vertex);
	if (numExtraFloats > 0) {
		extraFloatData.push_back((float*)malloc(numExtraFloats * sizeof(float)));
	}
	buffersDirty = true;
}

void ProceduralGeometry::addTriangle(unsigned int a, unsigned int b, unsigned int c) {
	indices.push_back(a);
	indices.push_back(b);
	indices.push_back(c);
	buffersDirty = true;
}
void ProceduralGeometry::clearData(bool onlyFloats) {
	buffersDirty = true;
	for (float* data : extraFloatData) {
		free(data);
	}
	extraFloatData.clear();
	if (onlyFloats) return;
	vertices.clear();
	indices.clear();
}

void ProceduralGeometry::setVertexPosition(int index, vec3 position) {
	if (index < 0 || index >= vertices.size()) return;
	vertices[index].position = position;

	if (!isBuffersDirty()) {
		float* Data = (float*)vbos[eVBO_Positions].data;
		Data[index * 3 + 0] = position.X;
		Data[index * 3 + 1] = position.Y;
		Data[index * 3 + 2] = position.Z;
		
		printf("Updating vertex %d position to (%f, %f, %f)\n", index, position.X, position.Y, position.Z);
		vbos[eVBO_Positions].setDataSub(index * 3 * sizeof(float), 3 * sizeof(float), &Data[index * 3]);
	}
	else {
		printf("Buffers are dirty, not updating vertex %d position to (%f, %f, %f)\n", index, position.X, position.Y, position.Z);

	}
	//buffersDirty = true;
}

void ProceduralGeometry::setVertexNormal(int index, vec3 normal) {
	if (index < 0 || index >= vertices.size()) return;
	vertices[index].normal = normal;

	if (!isBuffersDirty()) {
	float* Data = (float*)vbos[eVBO_Normal].data;
	Data[index * 3 + 0] = normal.X;
	Data[index * 3 + 1] = normal.Y;
	Data[index * 3 + 2] = normal.Z;

	vbos[eVBO_Normal].setDataSub(index * 3 * sizeof(float), 3 * sizeof(float), &Data[index * 3]);
	}
	//buffersDirty = true;
}

void ProceduralGeometry::setVertexUV(int index, vec2 uv) {
	if (index < 0 || index >= vertices.size()) return;
	vertices[index].uv = uv;
	
	/*
	Dont update the buffers if they are dirty, as that means there is a possibility that
	the data for this vertex does not exist yet in the VBO
	*/
	if (!isBuffersDirty()) {
		float* Data = (float*)vbos[eVBO_UV].data;
		Data[index * 2 + 0] = uv.X;
		Data[index * 2 + 1] = uv.Y;

		vbos[eVBO_UV].setDataSub(index * 2 * sizeof(float), 2 * sizeof(float), &Data[index * 2]);
	}
	//buffersDirty = true;
}

void ProceduralGeometry::setVertexColor(int index, vec3 color) {
	if (index < 0 || index >= vertices.size()) return;
	vertices[index].color = color;
	/*
	Dont update the buffers if they are dirty, as that means there is a possibility that
	the data for this vertex does not exist yet in the VBO
	*/
	if (!isBuffersDirty()) {
		float* Data = (float*)vbos[eVBO_Color].data;
		Data[index * 3 + 0] = color.X;
		Data[index * 3 + 1] = color.Y;
		Data[index * 3 + 2] = color.Z;
	
		vbos[eVBO_Color].setDataSub(index * 3 * sizeof(float), 3 * sizeof(float), &Data[index * 3]);
	}
	//buffersDirty = true;
}

void ProceduralGeometry::setVertexExtraFloatData(int index, float* data) {
	if (index < 0 || index >= vertices.size()) return;
	if (numExtraFloats <= 0) return;
	memcpy(extraFloatData[index], data, numExtraFloats * sizeof(float));

	if (!isBuffersDirty()) {
		//Update the VBO with the new data
		float* Data = (float*)vbos[eVBO_Extra].data;
		memcpy(Data + (index * numExtraFloats), data, numExtraFloats * sizeof(float));

		vbos[eVBO_Extra].setDataSub(index * numExtraFloats * sizeof(float), numExtraFloats * sizeof(float), &Data[index * numExtraFloats]);
	}
	//buffersDirty = true;
}

bool ProceduralGeometry::isBuffersDirty() {
	return buffersDirty;
}

bool ProceduralGeometry::isEmpty() {
	return vertices.empty() || (drawElements && indices.empty());
}

void ProceduralGeometry::generateBuffers() {

	if (!isBuffersDirty()) return;
	if (isEmpty()) return;
	//Generate VBOs

	clearBuffers(); 

	if (isGeometryInterleaved) {

	}
	else {
		vbos[eVBO_Positions].data = malloc(vertices.size() * 3 * sizeof(float));
		vbos[eVBO_Positions].calcSize(vertices.size() * 3);
		float* posData = (float*)vbos[eVBO_Positions].data;
		for (size_t i = 0; i < vertices.size(); ++i) {
			printf("Setting vertex %zu position: (%f, %f, %f)\n", i, vertices[i].position.X, vertices[i].position.Y, vertices[i].position.Z);
			posData[i * 3 + 0] = vertices[i].position.X;
			posData[i * 3 + 1] = vertices[i].position.Y;
			posData[i * 3 + 2] = vertices[i].position.Z;
		}
		vbos[eVBO_Normal].data = malloc(vertices.size() * 3 * sizeof(float));
		vbos[eVBO_Normal].calcSize(vertices.size() * 3);
		float* normData = (float*)vbos[eVBO_Normal].data;
		for (size_t i = 0; i < vertices.size(); ++i) {
			normData[i * 3 + 0] = vertices[i].normal.X;
			normData[i * 3 + 1] = vertices[i].normal.Y;
			normData[i * 3 + 2] = vertices[i].normal.Z;
		}
		vbos[eVBO_UV].data = malloc(vertices.size() * 2 * sizeof(float));
		vbos[eVBO_UV].calcSize(vertices.size() * 2);
		float* uvData = (float*)vbos[eVBO_UV].data;
		for (size_t i = 0; i < vertices.size(); ++i) {
			uvData[i * 2 + 0] = vertices[i].uv.X;
			uvData[i * 2 + 1] = vertices[i].uv.Y;
		}
		vbos[eVBO_Color].data = malloc(vertices.size() * 3 * sizeof(float));
		vbos[eVBO_Color].calcSize(vertices.size() * 3);
		float* colorData = (float*)vbos[eVBO_Color].data;
		for (size_t i = 0; i < vertices.size(); ++i) {
			colorData[i * 3 + 0] = vertices[i].color.X;
			colorData[i * 3 + 1] = vertices[i].color.Y;
			colorData[i * 3 + 2] = vertices[i].color.Z;
		}

		for (size_t i = 0; i < extraFloatData.size(); ++i) {
			vbos[eVBO_Extra].data = malloc(vertices.size() * numExtraFloats * sizeof(float));
			vbos[eVBO_Extra].calcSize(vertices.size() * numExtraFloats);
			float* extraData = (float*)vbos[eVBO_Extra].data;
			memcpy(extraData + (i * numExtraFloats), extraFloatData[i], numExtraFloats * sizeof(float));
		}

		//Generate EBO

		if (drawElements && !indices.empty()) {
			ebo.data = malloc(indices.size() * sizeof(unsigned int));
			ebo.calcSize(indices.size());
			memcpy(ebo.data, indices.data(), ebo.size);
		}
	}

	buffersDirty = false;
}

void ProceduralGeometry::setNumExtraFloats(int num) {
	if (num < 0 or num >= MAX_EXTRA_FLOATS) return;
	numExtraFloats = num;
}

Instance* ProceduralGeometry::clone_this() {
	ProceduralGeometry* g = (ProceduralGeometry*)registerInstance(new ProceduralGeometry(*this), true);
	g->vao = 0;
	g->clearBuffers();
	g->clearData(true);
	g->vertices = vertices;
	g->setNumExtraFloats(numExtraFloats);
	g->indices = indices;
	memcpy(g->vboFlags, vboFlags, eVBO_MAX);
	for (int i = 0; i < eVBO_Extra; ++i) {
		g->vbos[i].size = vbos[i].size;
		g->vbos[i].target = vbos[i].target;
		g->vbos[i].usage = vbos[i].usage;
		g->vbos[i].element_size = vbos[i].element_size;
		if (vbos[i].data != nullptr) {
			g->vbos[i].data = malloc(vbos[i].size);
			memcpy(g->vbos[i].data, vbos[i].data, vbos[i].size);
		}
	}
	if (ebo.data != nullptr) {
		g->ebo.size = ebo.size;
		g->ebo.target = ebo.target;
		g->ebo.usage = ebo.usage;
		g->ebo.element_size = ebo.element_size;
		g->ebo.data = malloc(ebo.size);
		memcpy(g->ebo.data, ebo.data, ebo.size);
	}
	return g;
}

int ProceduralGeometry::generateVAO(BufferLayout* bufferLayout) {
	printf("Generating VAO for ProceduralGeometry '%s' with %zu vertices and %zu indices.\n", name.c_str(), vertices.size(), indices.size());
	generateBuffers();

	return Geometry::generateVAO(bufferLayout);
}

int ProceduralGeometry::saveInstanceToFile(FILE* file) {
	int offset = 0;
	offset += Geometry::saveInstanceToFile(file);
	offset += fwrite(&numExtraFloats, sizeof(int), 1, file);
	offset += fwrite(&drawElements, sizeof(bool), 1, file);
	offset += fwrite(&bufferUpdateMode, sizeof(GLenum), 1, file);
	offset += fwrite(&vertices, sizeof(ProceduralVertex), vertices.size(), file);
	offset += fwrite(&indices, sizeof(int), indices.size(), file);
	for (float* data : extraFloatData) {
		offset += fwrite(data, sizeof(float), numExtraFloats, file);
	}
	return offset;
}

int ProceduralGeometry::loadInstanceFromFile(void* data, int off) {
	int o = off;
	o += Geometry::loadInstanceFromFile(data, off);
	memcpy(&numExtraFloats, offset(data, o), sizeof(int));
	o += sizeof(int);
	memcpy(&drawElements, offset(data, o), sizeof(bool));
	o += sizeof(bool);
	memcpy(&bufferUpdateMode, offset(data, o), sizeof(GLenum));
	o += sizeof(GLenum);
	int numVertices = *(int*)offset(data, o);
	o += sizeof(int);
	vertices.resize(numVertices);
	memcpy(vertices.data(), offset(data, o), sizeof(ProceduralVertex) * numVertices);
	o += sizeof(ProceduralVertex) * numVertices;
	int numIndices = *(int*)offset(data, o);
	o += sizeof(int);
	indices.resize(numIndices);
	memcpy(indices.data(), offset(data, o), sizeof(int) * numIndices);
	o += sizeof(ivec3) * numIndices;
	extraFloatData.resize(vertices.size());
	for (size_t i = 0; i < vertices.size(); ++i) {
		extraFloatData[i] = (float*)malloc(numExtraFloats * sizeof(float));
		memcpy(extraFloatData[i], (char*)offset(data, o), sizeof(float) * numExtraFloats);
		o += sizeof(float) * numExtraFloats;
	}
	return o - off;
}
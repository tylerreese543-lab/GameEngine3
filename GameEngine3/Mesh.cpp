#include "Mesh.h"
std::set<Mesh*> Mesh::render_objects;
Geometry::Geometry() : RenderObject() {
	name = "Mesh";
	type = InstanceType_Geometry;

	ebo.element_size = getSizeOfType(GL_UNSIGNED_INT);
	ebo.target = GL_ELEMENT_ARRAY_BUFFER;

	_blankInitVBO();
	setVBOFlags(true, false, false, false);

}

Geometry::~Geometry() {
}

Instance* Geometry::clone() {
	return new Geometry(*this);
}

int Geometry::generateVAO(BufferLayout* layout) {
	autoSearchVBO(); //AutoSearchVBO SHOULD be fine to auto-call because there would be no other way to initialize them...
	ebo.generate();
	for (int i = eVBO_Positions; i < eVBO_MAX; ++i) {
		if (vboFlags[i]) {
			vbos[i].generate();
		}
	}
	return GenVAO(&vao, vbos, &ebo, layout, eVBO_MAX);
}

void Geometry::render() {
	if (vao != 0) {
		DrawVAOElements(vao, GL_UNSIGNED_INT, (int)(ebo.size / ebo.element_size));
	}
}

void Geometry::setVBOFlags(bool hasPositions, bool hasUV, bool hasNormal, bool hasColor, bool hasExtra) {
	vboFlags[eVBO_Positions] = hasPositions;
	vboFlags[eVBO_UV] = hasUV;
	vboFlags[eVBO_Normal] = hasNormal;
	vboFlags[eVBO_Color] = hasColor;
	vboFlags[eVBO_Extra] = hasExtra;
}

void Geometry::_blankInitVBO() {
	for (int i = eVBO_Positions; i < eVBO_MAX; ++i) {
		vbos[i].element_size = getSizeOfType(GL_FLOAT);
		vbos[i].target = GL_ARRAY_BUFFER;
		vbos[i].usage = GL_STATIC_DRAW;
	}
}
Mesh::Mesh() : Instance() {
	setPosition(vec3(0, 0, 0));
	setRotation(vec3(0, 0, 0));
	setScale(vec3(1));
	
	type = InstanceType_Mesh;
	name = "Mesh";
	geometry = -1;

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
		geometry = getChildIdFromType(InstanceType_Geometry);
	}
	if (!isIdValid(material)) {
		material = getChildIdFromType(InstanceType_Material);
	}
	if (geometry == -1) {
		printf("Abort rendering of '%s' - has no geometry bound.\n", name.c_str());
		return;
	}
	if (material == -1) {
		printf("Abort rendering of '%s' - has no material bound.\n", name.c_str());
		return;
	}
	if (camera == nullptr) {
		printf("Abort rendering of '%s' - has no camera attached.\n", name.c_str());
		return;
	}
	Material* m = (Material*)getInstanceById(material);
	ShaderProgram* program = (ShaderProgram*)getInstanceById(m->getShaderInstanceId());
	if (!program) {
		printf("Abort rendering of '%s' - material has no shader program attached.\n", name.c_str());
		return;
	}

	Geometry* g = (Geometry*)getInstanceById(geometry);
	if (g->vao == 0) {
		g->generateVAO(&program->layout);
	}

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
	g->render();
	
}

void Geometry::autoSearchVBO() {
	for (int i = 0; i < eVBO_MAX; ++i) {
		vboFlags[i] = (vbos[i].data != nullptr);
	}
}

void Geometry::loadFromFile(std::string filepath, int meshID, int ImporterFlags) {
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(filepath, ImporterFlags);
	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
		std::cout << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
		return;
	}

	if (meshID < 0 || meshID > scene->mNumMeshes) meshID = 0;
	aiMesh* mesh = scene->mMeshes[meshID];

	int sizePos = mesh->mNumVertices * 3;
	int sizeUV = mesh->mNumVertices * 2;
	int sizeNormal = mesh->mNumVertices * 3;
	int sizeColor = mesh->mNumVertices * 3;
	int sizeIndices = mesh->mNumFaces * 3;
	if (mesh->HasPositions()) {
		vbos[eVBO_Positions].data = new float[sizePos];
		vbos[eVBO_Positions].calcSize(sizePos);
		memcpy(vbos[eVBO_Positions].data, mesh->mVertices, vbos[eVBO_Positions].size);
	}
	//Automatically use the 1st texture set
	if (mesh->HasTextureCoords(0)) {
		vbos[eVBO_UV].data = new float[sizeUV];
		vbos[eVBO_UV].calcSize(sizeUV);
		
		float* float_data = (float*)vbos[eVBO_UV].data;
		for (int i = 0; i < mesh->mNumVertices; ++i) {
			float_data[(i * 2) + 0] = mesh->mTextureCoords[0][i].x;
			float_data[(i * 2) + 1] = mesh->mTextureCoords[0][i].y;
		}
	}
	if (mesh->HasNormals()) {
		vbos[eVBO_Normal].data = new float[sizeNormal];
		vbos[eVBO_Normal].calcSize(sizeNormal);
		memcpy(vbos[eVBO_Normal].data, mesh->mNormals, vbos[eVBO_Normal].size);
	}
	if (mesh->HasVertexColors(0)) {
		vbos[eVBO_Color].data = new float[sizeColor];
		vbos[eVBO_Color].calcSize(sizeColor);
		memcpy(vbos[eVBO_Color].data, mesh->mColors[0], vbos[eVBO_Color].size);
	}

	if (mesh->HasFaces()) {
		ebo.data = new unsigned int[sizeIndices];
		ebo.calcSize(sizeIndices);

		unsigned int* int_data = (unsigned int*)ebo.data;
		for (int i = 0; i < mesh->mNumFaces; ++i) {
			int_data[(i * 3) + 0] = mesh->mFaces[i].mIndices[0];
			int_data[(i * 3) + 1] = mesh->mFaces[i].mIndices[1];
			int_data[(i * 3) + 2] = mesh->mFaces[i].mIndices[2];
		}
	}
	/*
	TODO: Actually expand these dynamically to fit any extra UV / Color data in the mesh file.
	Gives more creative freedom to developers in shaders
	*/

	autoSearchVBO();
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
#pragma once
#include "Graphics/Buffer.h"
#include "RenderObject.h"
#include "Graphics/matrix.h"
#include "Shader.h"
#include "Camera.h"
#include "Material.h"
#include "Graphics/GraphicsWrapper.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
class Geometry : public Resource {
private:
public:

	GLuint vao;
	virtual void clearBuffers();
	Buffer vbos[eVBO_MAX];
	//Whether the geometry contains multiple attributes in a single buffer or is stored separately in multiple;
	bool vboFlags[eVBO_MAX];
	bool isGeometryInterleaved; 
	Buffer ebo;
	virtual int generateVAO(BufferLayout* layout);
	Geometry();
	virtual ~Geometry();

	virtual Instance* clone_this();
	virtual void updateBuffers();
	virtual void render();
	virtual void setVBOFlags(bool hasPositions, bool hasUV, bool hasNormal, bool hasColor, bool hasExtra = false);
	virtual void autoSearchVBO();
	//genTangentsToExtra will generate the tangent/bi-tangent and place them in the extra VBO.
	void loadFromFile(std::string filepath, int meshID = 0, int ImporterFlags = aiProcess_Triangulate);

	virtual int saveInstanceToFile(FILE* file);
	virtual int loadInstanceFromFile(void* data, int offset = 0);

	virtual int getNumSaveBytes();
};

class ProceduralVertex {
public:
	vec3 position;
	vec3 normal;
	vec2 uv;
	vec3 color;

	ProceduralVertex(vec3 position = vec3(0, 0, 0), vec3 normal = vec3(0, 0, 0), vec2 uv = vec2(0, 0), vec3 color = vec3(1, 1, 1));
};
class ProceduralGeometry : public Geometry {
	std::vector<ProceduralVertex> vertices;
	std::vector<float*> extraFloatData;
	std::vector<int> indices;
	bool buffersDirty;
	int numExtraFloats;

	static const int MAX_EXTRA_FLOATS = 4;
public:
	GLenum bufferUpdateMode; //GL_STATIC_DRAW / GL_DYNAMIC_DRAW / GL_STREAM_DRAW
	//Whether to use glDrawElements or glDrawArrays when rendering. If true, the EBO will be used, otherwise the VBOs will be used directly.
	bool drawElements;
	ProceduralGeometry();

	virtual void clearBuffers();
	//Calling any of these will trigger a regeneration of the buffers, so don't call them too often (unless they are tiny);
	virtual void addVertex(ProceduralVertex vertex);
	virtual void addTriangle(unsigned int a, unsigned int b, unsigned int c);

	virtual Instance* clone_this();
	virtual void clearData(bool onlyFloats = false);
	//Changing the data is OK because we can call glBufferSubData instead of repopulating the entire buffer.
	virtual void setVertexPosition(int index, vec3 position);
	virtual void setVertexNormal(int index, vec3 normal);
	virtual void setVertexUV(int index, vec2 uv);
	virtual void setVertexColor(int index, vec3 color);
	virtual void setVertexExtraFloatData(int index, float* data);

	virtual int generateVAO(BufferLayout* layout);
	bool isBuffersDirty();
	bool isEmpty();

	virtual ~ProceduralGeometry();
	//Will only work if buffers are empty.
	virtual void setNumExtraFloats(int num);
	virtual void generateBuffers();

	virtual void render();

	virtual int getNumSaveBytes();
	virtual int saveInstanceToFile(FILE* file);
	virtual int loadInstanceFromFile(void* data, int offset = 0);
};

class Mesh : public Instance {
	//Finds the geometry based on this Mesh's children.
protected:
	virtual void setupRenderSettings(bool transparent = false, bool backfaceCulling = true);
	virtual void calcTransform();
	static std::set<Mesh*> render_objects;
	static int distanceQsortCompareFunction(const void* a, const void* b);
	static vec3 cameraPosition;
public:
	const static std::string renderLayerNames[eRenderLayer_MAX];
	const static int renderLayerEnum[eRenderLayer_MAX];
	vec3 rotation;
	vec3 position;
	vec3 scale;
	bool visible;
	int geometry;
	int material;
	mat4 transform;
	int renderLayer;
	bool cull;
	//Force the VAO to be regenerated due to shader change/resrt
	bool VAODirty;
	virtual void render(Camera* camera, bool transparent = false, int renderLayerTarget = eRenderLayer_World);


	Mesh();

	virtual int prepareMaterial(ShaderProgram** shader_out);

	void setPosition(vec3 position);
	void setRotation(vec3 rotation);
	void setScale(vec3 scale);

	static void renderAllMeshes(Camera* camera_world, Camera* camera_ui);

	virtual int destroy();

	virtual Instance* clone_this();

	virtual int saveInstanceToFile(FILE* file);
	virtual int loadInstanceFromFile(void* data, int offset = 0);
	virtual int getNumSaveBytes();

	virtual void finalizeLoad(inst_conv_map* conversionMap);
};
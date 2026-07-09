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
class Geometry : public RenderObject {
private:
public:
	void clearBuffers();
	Buffer vbos[eVBO_MAX];
	//Whether the geometry contains multiple attributes in a single buffer or is stored separately in multiple;
	bool vboFlags[eVBO_MAX];
	bool isGeometryInterleaved; 
	Buffer ebo;
	int generateVAO(BufferLayout* layout);
	Geometry();
	virtual ~Geometry();

	virtual Instance* clone_this();

	virtual void render();
	virtual void setVBOFlags(bool hasPositions, bool hasUV, bool hasNormal, bool hasColor, bool hasExtra = false);
	virtual void autoSearchVBO();
	//genTangentsToExtra will generate the tangent/bi-tangent and place them in the extra VBO.
	virtual void loadFromFile(std::string filepath, int meshID = 0, int ImporterFlags = aiProcess_Triangulate);

};

class Mesh : public Instance {
	//Finds the geometry based on this Mesh's children.
	virtual void calcTransform();
	static std::set<Mesh*> render_objects;
public:

	vec3 rotation;
	vec3 position;
	vec3 scale;
	bool visible;
	int geometry;
	int material;
	mat4 transform;
	virtual void render(Camera* camera);


	Mesh();
	void setPosition(vec3 position);
	void setRotation(vec3 rotation);
	void setScale(vec3 scale);

	static void renderAllMeshes(Camera* camera);

	virtual int destroy();

	virtual Instance* clone_this();
};
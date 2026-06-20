#pragma once
#include "Graphics/Buffer.h"
#include "RenderObject.h"
#include "Graphics/matrix.h"
#include "Shader.h"
#include "Camera.h"
#include "Material.h"
class Geometry : public RenderObject {
private:
	void _blankInitVBO();
public:
	Buffer vbos[5];
	//Whether the geometry contains multiple attributes in a single buffer or is stored separately in multiple;
	bool vboFlags[5];
	bool isGeometryInterleaved; 
	Buffer ebo;
	int generateVAO(BufferLayout* layout);
	Geometry();
	virtual ~Geometry();
	virtual Instance* clone();
	virtual void render();
	virtual void setVBOFlags(bool hasPositions, bool hasUV, bool hasNormal, bool hasColor, bool hasExtra = false);
	virtual void autoSearchVBO();
};

class Mesh : public Instance {
	//Finds the geometry based on this Mesh's children.
	virtual Geometry* autoGeometry();
	virtual void calcTransform();
public:
	vec3 position;
	vec3 rotation;
	vec3 scale;
	Geometry* geometry;
	mat4 transform;
	virtual void render(ShaderProgram* program, Camera* camera, Material* material);


	Mesh();
	void setPosition(vec3 position);
	void setRotation(vec3 rotation);
	void setScale(vec3 scale);
};
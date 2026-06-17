#pragma once
#include "Graphics/Buffer.h"
#include "RenderObject.h"

class Mesh : public RenderObject {
public:
	Buffer vbo;
	Buffer ebo;
	int generateVAO(BufferLayout* layout);
	Mesh();
	virtual ~Mesh();
	virtual Instance* clone();
	virtual void render();
};
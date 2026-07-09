#pragma once
#include "Graphics/Graphics.h"
#include "Graphics/VertexArrayObject.h"
#include "GameInstance.h"

class RenderObject : public Instance {
public:
	GLuint vao;
	RenderObject();
	virtual ~RenderObject();
	virtual void render();
};
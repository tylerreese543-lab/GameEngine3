#pragma once
#include "GameInstance.h"
#include "Graphics/matrix.h"
class AABB2D
{
public:
	vec2 min;
	vec2 max;

	AABB2D();
	AABB2D(const vec2& min, const vec2& max);

	bool intersects(const AABB2D& other) const;
	bool contains(const vec2& point) const;

};

class AABB3D
{
	public:
	vec3 min;
	vec3 max;
	AABB3D();
	AABB3D(const vec3& min, const vec3& max);
	bool intersects(const AABB3D& other) const;
	bool contains(const vec3& point) const;
};

class Collision3D : public Instance
{
	vec3 position;
	vec3 size;
	AABB3D boundingBox;

	static std::set<int> allCollisionBoxes;
public:

	static void RenderAllCollisionBoxesDebug();
	virtual Instance* clone_this();
	bool checkCollision(Collision3D* other) const;
	bool isInside(vec3 point) const;
	void setPosition(const vec3& pos);
	void setSize(const vec3& size);

	vec3 getPosition() const;
	vec3 getSize() const;

	vec3 getCenter() const;
	Collision3D();
	virtual int destroy();
	virtual int saveInstanceToFile(FILE* file);
	virtual int loadInstanceFromFile(void* data, int offset = 0);

	virtual int getNumSaveBytes();

	void debugRender();
};

class Collision2D : public Instance
{
	vec2 position;
	vec2 size;
	AABB2D boundingBox;
public:

	virtual Instance* clone_this();
	bool checkCollision(Collision2D* other) const;
	bool isInside(vec2 point) const;
	void setPosition(const vec2& pos);
	void setSize(const vec2& size);
	vec2 getPosition() const;
	vec2 getSize() const;

	vec2 getCenter() const;

	virtual int saveInstanceToFile(FILE* file);
	virtual int loadInstanceFromFile(void* data, int offset = 0);


	Collision2D();
};

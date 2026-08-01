#pragma once
#include "GameInstance.h"
#include "Graphics/matrix.h"
class Value : public Instance {
public:
	virtual Instance* clone_this();

	Value();
};

class IntegerValue : public Value {
public:
	virtual Instance* clone_this();
	int value;
	IntegerValue();
};
class FloatValue : public Value {
public:
	virtual Instance* clone_this();
	float value;
	FloatValue();
};
class BooleanValue : public Value {
public:
	virtual Instance* clone_this();
	bool value;
	BooleanValue();
};

class StringValue : public Value {
public:
	virtual Instance* clone_this();
	std::string value;
	StringValue();
};

class InstanceValue: public Value {
public:
	virtual Instance* clone_this();
	int value;
	Instance* get();
	InstanceValue();
};

class Vector3Value : public Value {
	public:
	virtual Instance* clone_this();
	vec3 value;
	Vector3Value();
};
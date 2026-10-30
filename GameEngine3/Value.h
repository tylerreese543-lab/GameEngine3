#pragma once
#include "GameInstance.h"
#include "Graphics/matrix.h"
class Value : public Instance {
public:
	virtual Instance* clone_this();

	Value();

	virtual int getNumSaveBytes();
	virtual int saveInstanceToFile(FILE* file);
	virtual int loadInstanceFromFile(void* data, int off = 0);
};

class IntegerValue : public Value {
public:
	virtual Instance* clone_this();
	int value;
	IntegerValue();

	virtual int getNumSaveBytes();
	virtual int saveInstanceToFile(FILE* file);
	virtual int loadInstanceFromFile(void* data, int off = 0);
};
class FloatValue : public Value {
public:
	virtual Instance* clone_this();
	float value;
	FloatValue();

	virtual int getNumSaveBytes();
	virtual int saveInstanceToFile(FILE* file);
	virtual int loadInstanceFromFile(void* data, int off = 0);
};
class BooleanValue : public Value {
public:
	virtual Instance* clone_this();
	bool value;
	BooleanValue();

	virtual int getNumSaveBytes();
	virtual int saveInstanceToFile(FILE* file);
	virtual int loadInstanceFromFile(void* data, int off = 0);
};

class StringValue : public Value {
public:
	virtual Instance* clone_this();
	std::string value;
	StringValue();

	virtual int getNumSaveBytes();
	virtual int saveInstanceToFile(FILE* file);
	virtual int loadInstanceFromFile(void* data, int off = 0);
};
class InstanceValue: public Value {
public:
	virtual Instance* clone_this();
	int value;
	Instance* get();
	InstanceValue();

	virtual int getNumSaveBytes();
	virtual int saveInstanceToFile(FILE* file);
	virtual int loadInstanceFromFile(void* data, int off = 0);
	virtual void finalizeLoad(inst_conv_map* conversionMap);
};

class Vector3Value : public Value {
	public:
	virtual Instance* clone_this();
	vec3 value;
	Vector3Value();

	virtual int getNumSaveBytes();
	virtual int saveInstanceToFile(FILE* file);
	virtual int loadInstanceFromFile(void* data, int off = 0);
};
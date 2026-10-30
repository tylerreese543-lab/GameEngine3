#include "Value.h"

Value::Value() {
	type = InstanceType_Value;
	name = "Value";
}

Instance* Value::clone_this() {
	return registerInstance(new Value(*this), true);
}

IntegerValue::IntegerValue() : Value() {
	type = InstanceType_IntegerValue;
	value = 0;
}

Instance* IntegerValue::clone_this() {
	return registerInstance(new IntegerValue(*this), true);
}

BooleanValue::BooleanValue() : Value() {
	type = InstanceType_BooleanValue;
	value = false;
}

Instance* BooleanValue::clone_this() {
	return registerInstance(new BooleanValue(*this), true);
}

StringValue::StringValue() : Value() {
	type = InstanceType_StringValue;
	value = "";
}

Instance* StringValue::clone_this() {
	return registerInstance(new StringValue(*this), true);
}

InstanceValue::InstanceValue() : Value() {
	type = InstanceType_InstanceValue;
	value = -1;
}

Instance* InstanceValue::clone_this() {
	return registerInstance(new InstanceValue(*this), true);
}

Instance* InstanceValue::get() {
	return getInstanceById(value);
}
FloatValue::FloatValue() : Value() {
	type = InstanceType_FloatValue;
	value = 0.0f;
}

Instance* FloatValue::clone_this() {
	return registerInstance(new FloatValue(*this), true);
}

Vector3Value::Vector3Value() : Value() {
	type = InstanceType_Vector3Value;
	value = vec3(0, 0, 0);
}

Instance* Vector3Value::clone_this() {
	return registerInstance(new Vector3Value(*this), true);
}

int IntegerValue::saveInstanceToFile(FILE* file) {
	int offset = 0;
	offset += fwrite(&value, sizeof(int), 1, file);

	return offset;
}

int StringValue::saveInstanceToFile(FILE* file) {
	int offset = 0;
	offset += fwrite(value.c_str(), sizeof(char), value.length() + 1, file);

	return offset;
}

int BooleanValue::saveInstanceToFile(FILE* file) {
	int offset = 0;
	offset += fwrite(&value, sizeof(bool), 1, file);

	return offset;
}

int FloatValue::saveInstanceToFile(FILE* file) {
	int offset = 0;
	offset += fwrite(&value, sizeof(float), 1, file);

	return offset;
}

int Vector3Value::saveInstanceToFile(FILE* file) {
	int offset = 0;
	offset += fwrite(&value, sizeof(vec3), 1, file);

	return offset;
}

int InstanceValue::saveInstanceToFile(FILE* file) {
	int offset = 0;
	offset += fwrite(&value, sizeof(int), 1, file);

	return offset;
}

int IntegerValue::getNumSaveBytes() { return sizeof(int); }
int StringValue::getNumSaveBytes() { return sizeof(char) *(value.length() + 1); }
int BooleanValue::getNumSaveBytes() { return sizeof(bool); }
int FloatValue::getNumSaveBytes() { return sizeof(float); }
int Vector3Value::getNumSaveBytes() { return sizeof(vec3); }
int InstanceValue::getNumSaveBytes() { return sizeof(int); }

int IntegerValue::loadInstanceFromFile(void* data, int off) {
	int o = off;
	memcpy(&value, offset(data, o), sizeof(int));
	o += sizeof(int);
	return o - off;
}
int StringValue::loadInstanceFromFile(void* data, int off) {
	int o = off;
	value = std::string((char*)offset(data, o));
	o += value.length() + 1;
	return o - off;
}
int BooleanValue::loadInstanceFromFile(void* data, int off) {
	int o = off;
	memcpy(&value, offset(data, o), sizeof(bool));
	o += sizeof(bool);
	return o - off;
}
int Vector3Value::loadInstanceFromFile(void* data, int off) {
	int o = off;
	memcpy(&value, offset(data, o), sizeof(vec3));
	o += sizeof(vec3);
	return o - off;
}
int InstanceValue::loadInstanceFromFile(void* data, int off) {
	int o = off;
	memcpy(&value, offset(data, o), sizeof(int));
	o += sizeof(int);
	return o - off;
}

void InstanceValue::finalizeLoad(inst_conv_map* conversionMap) {
	//Update the reference to its new id
	Instance::finalizeLoad(conversionMap);
	this->value = conversionMap->at(this->value);
}
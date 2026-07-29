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


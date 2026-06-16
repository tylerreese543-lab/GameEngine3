#include "GameInstance.h"

const std::string Instance::instanceTypeNames[Instance::numInstanceTypes] = {
	"Undefined",
	"Base"
};
Instance::Instance() {
	id = 0;
	name = "Instance";
	parent = nullptr;
	type = InstanceType_Undefined;
}

Instance::~Instance() {}

Instance* Instance::clone() {
	return new Instance(*this);
}

int Instance::destroy() {
	for (Instance* child : children) {
		child->destroy();
	}
	delete this;
	return 0;
}

int Instance::addChild(Instance* child) {
	//Check if the child is null
	if (child == nullptr) {
		return -1; // Invalid child
	}
	// Check if the child is already in the children vector
	for (Instance* c : children) {
		if (c == child) {
			return 0; // Child already exists
		}
	}
	children.push_back(child);
	child->setParent(this);
	return 0;
}

int Instance::setParent(Instance* p) {
	if (p == nullptr) {
		return -1; // Invalid parent
	}
	if (parent == p) {
		return 0; // Already has the same parent
	}
	if (parent != nullptr) {
		parent->children.erase(std::remove(parent->children.begin(), parent->children.end(), this), parent->children.end());
	}
	parent = p;
	p->addChild(this);
	return 0;
}

Instance* Instance::getChildFromName(const std::string& name) {
	for (Instance* child : children) {
		if (child->name == name) {
			return child;
		}
	}
	printf("ERROR: Child with name '%s' not found.\n", name.c_str());
	return nullptr; // Child not found
}

Instance* Instance::getChildFromId(int id) {
	for (Instance* child : children) {
		if (child->id == id) {
			return child;
		}
	}
	printf("ERROR: Child with ID '%d' not found.\n", id);
	return nullptr; // Child not found
}

Instance* Instance::getChildFromIndex(int index) {
	if (index < 0 || index >= children.size()) {
		printf("ERROR: Invalid child index '%d'.\n", index);
		return nullptr; // Index out of bounds
	}
	return children[index];
}

std::string Instance::getInstanceNameFromType(int type) {
	if (type < 0 || type >= numInstanceTypes) {
		return "Unknown"; // Invalid type
	}
	return instanceTypeNames[type];
}
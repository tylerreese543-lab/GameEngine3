#include "GameInstance.h"

int Instance::currentinstanceID = 0;
std::unordered_map<int, Instance*> Instance::world;
const std::string Instance::instanceTypeNames[Instance::InstanceType_Count] = {
	"Undefined",
	"Base",
	"RenderObject",
	"Mesh",
	"Resource",
	"ShaderResource",
	"ShaderProgram"

};
Instance::Instance() {
	instanceId = 0;
	name = "Instance";
	parent = nullptr;
	type = InstanceType_Undefined;

	registered = false;
	registerInstance(this);
}

Instance::~Instance() {}

Instance* Instance::clone() {
	return new Instance(*this);
}

int Instance::destroy() {
	for (Instance* child : children) {
		child->destroy();
	}
	
	world[instanceId] = nullptr;
	instanceId = -1;
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
	child->parent = this;
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
		if (child->instanceId == id) {
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
	if (type < 0 || type >= InstanceType_Count) {
		return "Unknown"; // Invalid type
	}
	return instanceTypeNames[type];
}

int Instance::save(FILE* file) {
	fwrite(&type, sizeof(int), 1, file);
	fwrite(&instanceId, sizeof(int), 1, file);
	size_t nameLength = name.size();
	fwrite(&nameLength, sizeof(size_t), 1, file);
	fwrite(name.c_str(), sizeof(char), nameLength, file);
	return 0; //TODO: return the size of the data written to the file
}

int Instance::load(void* data) {
	int o = 0;
	type = *(int*)offset(data, o);
	o += sizeof(int);
	instanceId = *(int*)offset(data, o);
	o += sizeof(int);
	size_t nameLength = *(size_t*)offset(data, o);
	o += sizeof(size_t);
	char* nameData = (char*)offset(data, o);
	name = std::string(nameData, nameLength);
	o += nameLength * sizeof(char);

	return o;
}

void* offset(void* ptr, size_t offset) {
	return (void*)((char*)ptr + offset);
}

void Instance::registerInstance(Instance* inst) {
	if (inst->registered == false) {
		inst->registered = true;
		inst->instanceId = ++currentinstanceID;
		world[currentinstanceID] = inst;
	}
}
bool Instance::isIdValid(int ID) {
	return world.count(ID) > 0;
}
bool Instance::isRegistered(Instance* inst) {
	return (world[inst->getId()] == inst) && (inst->registered == true) && isIdValid(inst->getId());
}

Instance* Instance::getInstanceById(int ID) {
	if (!isIdValid(ID)) return nullptr;
	return world[ID];
}

int Instance::getId() {
	return instanceId;
}
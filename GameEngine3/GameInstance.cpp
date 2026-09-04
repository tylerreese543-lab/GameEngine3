#include "GameInstance.h"

int Instance::currentinstanceID = 0;
std::map<int, Instance*> Instance::world;
std::set<Instance*> Instance::destroyQueue;
std::map<unsigned int, unsigned int> Instance::instanceHierarchyMap;
void Instance::registerHierarchy() {
	instanceHierarchyMap = {
		{-1, -1},
	{InstanceType_Undefined, -1},
	{InstanceType_Base, -1},
	{InstanceType_RenderObject, InstanceType_Base},
	{InstanceType_Resource, InstanceType_Base},
	{InstanceType_ShaderProgram, InstanceType_Base},
	{InstanceType_Camera, InstanceType_Base},
	{InstanceType_Mesh, InstanceType_Base},
	{InstanceType_Scene, InstanceType_Base},
	{InstanceType_Material, InstanceType_Base},
	{InstanceType_Geometry, InstanceType_Resource},
	{InstanceType_LuaResource, InstanceType_Resource},
	{InstanceType_ShaderResource, InstanceType_Resource},
	{InstanceType_ImageResource, InstanceType_Resource},
	{InstanceType_Value, InstanceType_Base},

	{InstanceType_IntegerValue, InstanceType_Value},
	{InstanceType_BooleanValue, InstanceType_Value},
	{InstanceType_FloatValue, InstanceType_Value},
	{InstanceType_StringValue, InstanceType_Value},
	{InstanceType_InstanceValue, InstanceType_Value},
	{InstanceType_Vector3Value, InstanceType_Value},

	{InstanceType_Collision2D, InstanceType_Base},
	{InstanceType_Collision3D, InstanceType_Base},
	{ InstanceType_ProceduralGeometry, InstanceType_Geometry}
	};
}

bool Instance::isInstanceOf(unsigned int classId, unsigned int targetId) {
	if (classId == -1) {
		return false;
	}
	return classId == targetId || isInstanceOf(instanceHierarchyMap[classId], targetId);
}

int Instance::getTypeTooltip() {
	return IDS_TOOLTIP_INSTANCE;
}
const std::string Instance::instanceTypeNames[Instance::InstanceType_Count] = {
	"Undefined",
	"Instance",
	"RenderObject",
	"Geometry",
	"Resource",
	"ShaderResource",
	"ImageResource",
	"ShaderProgram",
	"Camera",
	"Mesh",
	"Scene",
	"Material",
	"Script",
	"Value",
	"IntValue",
	"BoolValue",
	"FloatValue",
	"StringValue",
	"InstanceValue",
	"Vector3Value",
	"AABBCollision2D",
	"AABBCollision3D",
	"ProceduralGeometry"
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

Instance* Instance::clone_this() {
	return registerInstance(new Instance(*this), true);
}
Instance* Instance::clone() {
	Instance* i = this->clone_this();

	i->children.clear();

	for (Instance* child : children) {
		child->clone()->setParent(i);
	}

	i->setParent(parent);
	return i;
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
	if (child->parent != nullptr) {
		child->parent->children.erase(child);
	}

	if (child->children.count(this) > 0) {
		child->children.erase(this);
		child->parent->addChild(this);
	}
	// Check if the child is already in the children vector
	children.insert(child);
	child->parent = this;
	return 0;
}

int Instance::setParent(Instance* p) {
	if (p == nullptr) {
		return -1; // Invalid parent
	}
	p->addChild(this);
	return 0;
}

Instance* Instance::getChildFromName(const std::string& name) {
	for (Instance* child : children) {
		if (child->name == name) {
			return child;
		}
	}
	//printf("ERROR: Child with name '%s' not found.\n", name.c_str());
	return nullptr; // Child not found
}
Instance* Instance::getChildFromClass(const std::string& className) {
	for (Instance* child : children) {
		if (child->getInstanceNameFromType(child->type) == className) {
			return child;
		}
	}
	//printf("ERROR: Child with name '%s' not found.\n", name.c_str());
	return nullptr; // Child not found
}
Instance* Instance::getChildFromType(int typ) {
	for (Instance* child : children) {
		if (child->type == typ) {
			return child;
		}
	}
	//printf("ERROR: Child with type '%s' not found.\n", getInstanceNameFromType(typ).c_str());
	return nullptr; // Child not found
}
int Instance::getChildIdFromType(int typ) {
	for (Instance* child : children) {
		if (child->type == typ) {
			return child->getId();
		}
	}
	//printf("ERROR: Child with type '%s' not found.\n", getInstanceNameFromType(typ).c_str());
	return -1; // Child not found
}
Instance* Instance::getChildFromId(int id) {
	for (Instance* child : children) {
		if (child->instanceId == id) {
			return child;
		}
	}
	//printf("ERROR: Child with ID '%d' not found.\n", id);
	return nullptr; // Child not found
}

Instance* Instance::getChildFromIndex(int index) {
		return nullptr; //Deprecated
	if (index < 0 || index >= children.size()) {
		printf("ERROR: Invalid child index '%d'.\n", index);
		return nullptr; // Index out of bounds
	}
	return nullptr;
}

std::string Instance::getInstanceNameFromType(int type) {
	if (type < 0 || type >= InstanceType_Count) {
		return "Unknown"; // Invalid type
	}
	return instanceTypeNames[type];
}

int Instance::loadInstanceFromFile(void* data, int off) {
	int o = off;
	type = *(int*)offset(data, o);
	o += sizeof(int);
	instanceId = *(int*)offset(data, o);
	o += sizeof(int);
	size_t nameLength = *(size_t*)offset(data, o);
	o += sizeof(size_t);
	char* nameData = (char*)offset(data, o);
	name = std::string(nameData, nameLength);
	o += nameLength * sizeof(char);

	return o - off;
}

void* offset(void* ptr, size_t offset) {
	return (void*)((char*)ptr + offset);
}

Instance* Instance::registerInstance(Instance* inst, bool clone) {
	if (inst->registered == false || clone) {
		inst->registered = true;
		inst->instanceId = ++currentinstanceID;
		world[currentinstanceID] = inst;
		//printf("Registering new Instance: ID = %d\n", inst->getId());
	}

	return inst;
}
bool Instance::isIdValid(int ID) {
	return world.count(ID) > 0;
}
bool Instance::isRegistered(Instance* inst) {
	if (inst == nullptr) return false;
	return (world[inst->getId()] == inst) && (inst->registered == true) && isIdValid(inst->getId());
}

Instance* Instance::getInstanceById(int ID) {
	if (!isIdValid(ID)) return nullptr;
	if (!isRegistered(world[ID])) return nullptr;
	return world[ID];
}

int Instance::getId() {
	return instanceId;
}

void Instance::listChildren() {

	printf("--- Printing children for instance '%s' ---\n", name.c_str());
	int i = 0;
	for (Instance* child : children) {
		printf("[%d] Name: '%s', ID: %d, Type: %s\n", i, child->name.c_str(), child->getId(), getInstanceNameFromType(child->type).c_str());
		++i;
	}
}

void Instance::update(float deltaTime, bool isStudio) {}

int Instance::saveInstanceToFile(FILE* file) {
	int tag = LOAD_INSTANCE_TAG;
	fwrite(&tag, sizeof(int), 1, file);
	fwrite(&type, sizeof(int), 1, file);
	fwrite(&instanceId, sizeof(int), 1, file);
	fwrite(name.c_str(), sizeof(char), name.size() + 1, file);
	return 0; //TODO: return the size of the data written to the file, may not be needed however
}

std::string Instance::getEngineGUIHash() {
	return name + "###" + std::to_string(getId());
}

void Instance::handleDestroyQueue() {
	for (Instance* inst : destroyQueue) {

		destroyQueue.erase(inst);
		inst->parent->children.erase(inst);
		inst->destroy();

	}
};
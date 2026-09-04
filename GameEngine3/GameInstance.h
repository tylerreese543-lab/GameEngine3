#pragma once
#include <string>
#include <vector>
#include <map>
#include <set>

#define GAME_VERSION_MAJOR 0
#define GAME_VERSION_MINOR 1
#define GAME_VERSION_PATCH 0

#define GAME_VERSION_STRING "0.1.0"

#define LOAD_INSTANCE_TAG 10

#include <queue>

#include "Engine/tooltips.h"
class Instance {
	static int currentinstanceID;
	static std::map<unsigned int, unsigned int> instanceHierarchyMap;
public:

	//types of instances and their roots (for isInstanceOf())
	static void registerHierarchy();
	static bool isInstanceOf(unsigned int classId, unsigned int targetId);
	static Instance* registerInstance(Instance* instance, bool clone = false);
	static bool isRegistered(Instance* instance);
	static bool isIdValid(int ID);
	static Instance* getInstanceById(int ID);
	static std::map<int, Instance*> world;
	enum {
		InstanceType_Undefined = 0,
		InstanceType_Base,
		InstanceType_RenderObject,
		InstanceType_Geometry,
		InstanceType_Resource,
		InstanceType_ShaderResource,
		InstanceType_ImageResource,
		InstanceType_ShaderProgram,
		InstanceType_Camera,
		InstanceType_Mesh,
		InstanceType_Scene,
		InstanceType_Material,
		InstanceType_LuaResource,
		InstanceType_Value,
		InstanceType_IntegerValue,
		InstanceType_BooleanValue,
		InstanceType_FloatValue,
		InstanceType_StringValue,
		InstanceType_InstanceValue,
		InstanceType_Vector3Value,
		InstanceType_Collision2D,
		InstanceType_Collision3D,
		InstanceType_ProceduralGeometry,
		InstanceType_Count
	};
	static const std::string instanceTypeNames[InstanceType_Count];
	static std::set<Instance*> destroyQueue;
	Instance();
	virtual ~Instance();
	//Clones an individual instance
	virtual Instance* clone_this();
	//Clones this instance's entire hierarchy
	virtual Instance* clone();
	virtual int destroy();
	int instanceId;
	int type;
	bool registered;
	std::string name;
	Instance* parent;
	std::set<Instance*> children;
	int getId();
	virtual int addChild(Instance* child);
	virtual int setParent(Instance* parent);
	virtual Instance* getChildFromName(const std::string& name);
	virtual Instance* getChildFromId(int id);
	virtual Instance* getChildFromIndex(int index);
	virtual Instance* getChildFromType(int type);
	virtual Instance* getChildFromClass(const std::string& className);
	virtual void update(float deltaTime, bool isStudio);
	virtual int getChildIdFromType(int type);
	virtual int getTypeTooltip();
	virtual std::string getEngineGUIHash();
	void listChildren();

	static std::string getInstanceNameFromType(int type);
	static void handleDestroyQueue();
	virtual int saveInstanceToFile(FILE* file); //Returns a pointer to the data representing this instance. The caller is responsible for freeing this memory.
	virtual int loadInstanceFromFile(void* data, int offset = 0); //Loads the instance from the given data. The caller is responsible for ensuring that the data is valid and properly formatted.
};

void* offset(void* ptr, size_t offset);

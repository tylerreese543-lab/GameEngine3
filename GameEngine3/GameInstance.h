#pragma once
#include <string>
#include <vector>
#include <unordered_map>
class Instance {
public:
	static int currentinstanceID;

	static void registerInstance(Instance* instance);
	static bool isRegistered(Instance* instance);
	static bool isIdValid(int ID);
	static Instance* getInstanceById(int ID);
	static std::unordered_map<int, Instance*> world;
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
		InstanceType_Count
	};
	static const std::string instanceTypeNames[InstanceType_Count];
	Instance();
	virtual ~Instance();
	virtual Instance* clone();
	virtual int destroy();
	int instanceId;
	int type;
	bool registered;
	std::string name;
	Instance* parent;
	std::vector<Instance*> children;
	int getId();
	virtual int addChild(Instance* child);
	virtual int setParent(Instance* parent);
	virtual Instance* getChildFromName(const std::string& name);
	virtual Instance* getChildFromId(int id);
	virtual Instance* getChildFromIndex(int index);
	static std::string getInstanceNameFromType(int type);

	virtual int save(FILE* file); //Returns a pointer to the data representing this instance. The caller is responsible for freeing this memory.
	virtual int load(void* data); //Loads the instance from the given data. The caller is responsible for ensuring that the data is valid and properly formatted.
};

void* offset(void* ptr, size_t offset);
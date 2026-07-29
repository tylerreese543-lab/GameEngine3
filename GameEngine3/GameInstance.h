#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <set>
class Instance {
	static int currentinstanceID;
public:

	static Instance* registerInstance(Instance* instance, bool clone = false);
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
		InstanceType_Scene,
		InstanceType_Material,
		InstanceType_LuaResource,
		InstanceType_Value,
		InstanceType_IntegerValue,
		InstanceType_BooleanValue,
		InstanceType_FloatValue,
		InstanceType_StringValue,
		InstanceType_InstanceValue,
		InstanceType_Count
	};
	static const std::string instanceTypeNames[InstanceType_Count];
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
	std::vector<Instance*> children;
	int getId();
	virtual int addChild(Instance* child);
	virtual int setParent(Instance* parent);
	virtual Instance* getChildFromName(const std::string& name);
	virtual Instance* getChildFromId(int id);
	virtual Instance* getChildFromIndex(int index);
	virtual Instance* getChildFromType(int type);
	virtual void update(float deltaTime);
	virtual int getChildIdFromType(int type);

	void listChildren();

	static std::string getInstanceNameFromType(int type);

	virtual int save(FILE* file); //Returns a pointer to the data representing this instance. The caller is responsible for freeing this memory.
	virtual int load(void* data); //Loads the instance from the given data. The caller is responsible for ensuring that the data is valid and properly formatted.
};

void* offset(void* ptr, size_t offset);
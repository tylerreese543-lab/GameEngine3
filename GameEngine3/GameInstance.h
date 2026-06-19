#pragma once
#include <string>
#include <vector>
class Instance {
public:
	enum {
		InstanceType_Undefined = 0,
		InstanceType_Base,
		InstanceType_RenderObject,
		InstanceType_Mesh,
		InstanceType_Resource,
		InstanceType_ShaderResource,
		InstanceType_ImageResource,
		InstanceType_ShaderProgram,
		InstanceType_Camera,
		InstanceType_Count
	};
	static const std::string instanceTypeNames[InstanceType_Count];
	Instance();
	virtual ~Instance();
	virtual Instance* clone();
	virtual int destroy();
	int instanceId;
	int type;
	std::string name;
	Instance* parent;
	std::vector<Instance*> children;

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
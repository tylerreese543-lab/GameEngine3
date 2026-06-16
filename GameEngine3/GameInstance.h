#pragma once
#include <string>
#include <vector>
class Instance {
public:
	enum {
		InstanceType_Undefined = 0,
		InstanceType_Base,
	};
	static const int numInstanceTypes = 2;
	static const std::string instanceTypeNames[numInstanceTypes];
	Instance();
	virtual ~Instance();
	virtual Instance* clone();
	virtual int destroy();
	int id;
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
};
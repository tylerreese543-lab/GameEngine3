#pragma once
#include <string>
#include <vector>
class Instance {
public:
	Instance();
	virtual ~Instance();
	virtual Instance* clone();
	virtual int destroy();
	int id;
	std::string name;
	Instance* parent;
	std::vector<Instance*> children;

	virtual int addChild(Instance* child);
	virtual int setParent(Instance* parent);
	virtual Instance* getChildFromName(const std::string& name);
	virtual Instance* getChildFromId(int id);
	virtual Instance* getChildFromIndex(int index);
};
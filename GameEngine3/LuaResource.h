#pragma once
#include "GameInstance.h"
#include "Resource.h"
#include "ScriptEvent.h"

class LuaResource : public Resource {
public:
	static sol::state lua;
	static void InitializeLua();
	static sol::object InstanceToSolObj(sol::this_state ts, Instance* i);
	static sol::table SolTableFromIV(sol::this_state ts, std::set<Instance*>* instances);
	LuaResource(std::string filepath = "");
	virtual ~LuaResource();
	
	sol::environment env;
	sol::load_result loaded_script;
	sol::protected_function tick_function;

	bool isEngineScript;
	bool has_tick;
	const char* source;
	bool loaded;
	void loadFromFile(std::string filepath);
	void loadScript();
	virtual void update(float deltaTime, bool isStudio);

	static sol::object __luaIndex(Instance* self, sol::this_state ts, const std::string& key);

	virtual bool omitFromSave();
	virtual int saveInstanceToFile(FILE* file);
	virtual int loadInstanceFromFile(void* data, int off = 0);
	virtual int getNumSaveBytes();
	virtual void finalizeLoad(inst_conv_map* conversionMap);
};


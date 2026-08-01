#pragma once
#include "GameInstance.h"
#include "Resource.h"
#include "ScriptEvent.h"

class LuaResource : public Resource {
public:
	static sol::state lua;
	static void InitializeLua();
	static sol::object InstanceToSolObj(sol::this_state ts, Instance* i);
	static sol::table SolTableFromIV(sol::this_state ts, std::vector<Instance*>* instances);
	LuaResource(std::string filepath = "");
	~LuaResource();
	
	std::string filePath;
	sol::environment env;
	sol::load_result loaded_script;
	sol::protected_function tick_function;

	bool has_tick;
	const char* source;
	bool loaded;
	void loadFromFile(std::string filepath);
	void loadScript();
	virtual void update(float deltaTime);

	static sol::object __luaIndex(Instance* self, sol::this_state ts, const std::string& key);
};


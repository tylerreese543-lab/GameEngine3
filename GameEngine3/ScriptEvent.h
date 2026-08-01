#pragma once
#define SOL_ALL_SAFETIES_ON 1
#include <sol/sol.hpp>
class ScriptEvent
{
public:
	std::vector<sol::protected_function> connections;

	ScriptEvent();

	void connect(sol::protected_function func);
};
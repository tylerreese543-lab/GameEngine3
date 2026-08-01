#include "ScriptEvent.h"


ScriptEvent::ScriptEvent() {

}

void ScriptEvent::connect(sol::protected_function func) {
	connections.push_back(func);
}
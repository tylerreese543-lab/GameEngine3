#include "LuaResource.h"
#include "Graphics/Graphics.h"
#include "Camera.h"
#include "Mesh.h"
#include "Scene.h"
#include "Material.h"
#include "Shader.h"
sol::state LuaResource::lua;
LuaResource::LuaResource(std::string filepath) : Resource() {
	name = "Script";
	type = InstanceType_LuaResource;

	filePath = filepath;
	source = "";
	has_tick = false;
	loadFromFile(filepath);
}

sol::object LuaResource::InstanceToSolObj(sol::this_state ts, Instance* i) {
	switch (i->type) {
	case InstanceType_LuaResource:
		return sol::make_object(ts, (LuaResource*)i);
	case InstanceType_Camera:
		return sol::make_object(ts, (Camera*)i);
	default:
		//Base instance class - no casting necessary
		return sol::make_object(ts, i);
	}
}
void LuaResource::InitializeLua() {
	lua.open_libraries(sol::lib::base);

	lua.new_usertype<vec3>("Vector3",
		// Allow creating vectors via: Vector3.new() or Vector3.new(x, y, z)
		sol::constructors<vec3(), vec3(float, float, float), vec3(float)>(),

		// Map the properties so Lua can read/write them
		"X", &vec3::X,
		"Y", &vec3::Y,
		"Z", &vec3::Z
	);

	lua.new_usertype<vec2>("Vector2",
		sol::constructors<vec2(), vec2(float, float)>(),

		// Map the properties so Lua can read/write them
		"X", &vec2::X,
		"Y", &vec2::Y
	);

	lua.new_usertype<MouseData>("MouseData",
		"Position", sol::readonly(&MouseData::screenFull)
	);
	// Register the GameInstance usertype
  //TODO: eventually abstract this to only have access to ID
	auto InstanceType = lua.new_usertype<Instance>("Instance",
		// Expose variables
		"Name", &Instance::name,
		"ID", sol::property(&Instance::getId),
		"Parent", sol::property(
			[](Instance* self, sol::this_state ts) {
				if (self && self->parent) {
					return InstanceToSolObj(ts, self->parent);
				}
				return sol::make_object(ts, sol::nil);
			},
			[](Instance* self, sol::this_state ts, sol::object value) {
				self->setParent(value.as<Instance*>());
			}
		)
		);

	lua.new_usertype<LuaResource>("Script",
		sol::base_classes, sol::bases<Instance>()
	);

	lua.new_usertype<Camera>("Camera",
		sol::base_classes, sol::bases<Instance>(),
		"FOV", &Camera::fov,
		"Position", &Camera::position,
		"Rotation", &Camera::rotation
	);
}
LuaResource::~LuaResource() {

}
void LuaResource::loadFromFile(std::string filepath) {
	loaded = true;
	filePath = filepath;
	if (filePath == "") return;

	FILE* file;
	fopen_s(&file, filepath.c_str(), "rb");
	if (!file) {
		printf("ERROR: Failed to open shader file '%s'.\n", filepath.c_str());
		return;
	}
	fseek(file, 0, SEEK_END);
	size_t size = ftell(file);
	fseek(file, 0, SEEK_SET);
	source = new char[size + 1];
	fread((void*)source, sizeof(char), size, file);
	((char*)source)[size] = '\0';
	fclose(file);
	
}

void LuaResource::loadScript() {
	env = sol::environment(lua, sol::create, lua.globals());
	loaded_script = lua.load(source);
	if (!loaded_script.valid()) {
		sol::error err = loaded_script;
		printf("Failed to load script: %s\n", err.what());
		return;
	}
	
	sol::protected_function script_func = loaded_script;

	// 3. ATTACH the environment sandbox directly to the function instance
	env.set_on(script_func);
	env["this"] = InstanceToSolObj(sol::this_state(lua), this);
	env["MouseData"] = Graphics::getInstance()->mouse_data;
	// 4. EXECUTE the attached function once to register your "Tick" block inside 'env'
	sol::protected_function_result run_result = script_func();

	if (!run_result.valid()) {
		sol::error err = run_result;
		printf("Failed to execute script startup: %s\n", err.what());
		return;
	}

	has_tick = false;
	sol::optional<sol::protected_function> optional_tick = env["Tick"];
	if (optional_tick.has_value()) {
		has_tick = true;
		tick_function = optional_tick.value();
		printf("Has tick \n");
	}
	else {
		printf("No tick :(\n");
	}
}

void LuaResource::update(float dt) {
	if (!has_tick) return;

	auto result = tick_function(dt);
	if (!result.valid()) {
		sol::error err = result;
		printf("LuaRuntimeError: %s\n", err.what());
	}

}
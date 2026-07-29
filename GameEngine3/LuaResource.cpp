#include "LuaResource.h"
#include "Graphics/Graphics.h"
#include "Camera.h"
#include "Mesh.h"
#include "Scene.h"
#include "Material.h"
#include "Shader.h"
#include "Value.h"
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
	case InstanceType_Mesh:
		return sol::make_object(ts, (Mesh*)i);
	case InstanceType_Material:
		return sol::make_object(ts, (Material*)i);
	case InstanceType_ShaderProgram:
		return sol::make_object(ts, (ShaderProgram*)i);
	case InstanceType_ShaderResource:
		return sol::make_object(ts, (ShaderResource*)i);
	case InstanceType_ImageResource:
		return sol::make_object(ts, (ImageResource*)i);
	case InstanceType_Resource:
		return sol::make_object(ts, (Resource*)i);
	case InstanceType_Scene:
		return sol::make_object(ts, (Scene*)i);
	case InstanceType_Value:
		return sol::make_object(ts, (Value*)i);
	case InstanceType_IntegerValue:
		return sol::make_object(ts, (IntegerValue*)i);
	case InstanceType_BooleanValue:
		return sol::make_object(ts, (BooleanValue*)i);
	case InstanceType_StringValue:
		return sol::make_object(ts, (StringValue*)i);
	case InstanceType_FloatValue:
		return sol::make_object(ts, (FloatValue*)i);
	case InstanceType_InstanceValue:
		return sol::make_object(ts, (InstanceValue*)i);
	default:
		//Base instance class - no casting necessary
		return sol::make_object(ts, i);
	}
}
void LuaResource::InitializeLua() {
	lua.open_libraries(sol::lib::base);

	sol::table enumRoot = lua.create_named_table("Enum");
	enumRoot.new_enum("KeyCode",
		"Q", GLFW_KEY_Q,
		"W", GLFW_KEY_W,
		"E", GLFW_KEY_E,
		"A", GLFW_KEY_A,
		"S", GLFW_KEY_S,
		"D", GLFW_KEY_D,
		"F", GLFW_KEY_F,
		"G", GLFW_KEY_G,
		"H", GLFW_KEY_H,
		"J", GLFW_KEY_J,
		"K", GLFW_KEY_K,
		"L", GLFW_KEY_L,
		"Up", GLFW_KEY_UP,
		"Down", GLFW_KEY_DOWN,
		"Left", GLFW_KEY_LEFT,
		"Right", GLFW_KEY_RIGHT,
		"Spacebar", GLFW_KEY_SPACE
	);
	enumRoot.new_enum("MouseButton",
		"Left", GLFW_MOUSE_BUTTON_LEFT,
		"Right", GLFW_MOUSE_BUTTON_RIGHT,
		"Middle", GLFW_MOUSE_BUTTON_MIDDLE
	);

	enumRoot.new_enum("ShaderType",
		"Vertex", GL_VERTEX_SHADER,
		"Fragment", GL_FRAGMENT_SHADER
	);
	lua.new_usertype<mat3>("Matrix3"
		//Mat3 not accessible to lua yet, placeholder
	);

	lua.new_usertype<vec3>("Vector3",
		// Allow creating vectors via: Vector3.new() or Vector3.new(x, y, z)
		sol::constructors<vec3(), vec3(float, float, float), vec3(float)>(),

		// Map the properties so Lua can read/write them
		"X", &vec3::X,
		"Y", &vec3::Y,
		"Z", &vec3::Z,

		sol::meta_function::addition, &vec3::operator+,
		sol::meta_function::subtraction, sol::overload(
			[](const vec3& a, const vec3& b) { return a - b; },
			[](const vec3& a) { return -a; }
		),
		sol::meta_function::multiplication, sol::overload(
			[](const vec3& a, const vec3& b) { return a * b; },
			[](const vec3& a, float b) { return a * b; },
			[](const vec3& a, const mat3& b) { return b * a; }
		)
	);

	lua.new_usertype<vec2>("Vector2",
		sol::constructors<vec2(), vec2(float, float)>(),

		// Map the properties so Lua can read/write them
		"X", &vec2::X,
		"Y", &vec2::Y
	);

	lua.new_usertype<MouseData>("MouseData",
		"Position", sol::readonly(&MouseData::screenFull),
		"SetMouseInputMode", &MouseData::setRawMouseInput,
		"IsMouseDown", &MouseData::isMouseDown,
		"IsRawMouseInput", &MouseData::isRawMouseInput
	);
	lua.new_usertype<KeyboardData>("KeyboardData",
		"IsKeyDown", &KeyboardData::isKeyDown
	);
	// Register the GameInstance usertype
  //TODO: eventually abstract this to only have access to ID
	auto InstanceType = lua.new_usertype<Instance>("Instance",
		// Expose variables
		"Name", &Instance::name,
		"ID", sol::property(&Instance::getId),
		"Type", sol::property([](Instance* self, sol::this_state ts) {
				if (self) {
					return getInstanceNameFromType(self->type);
				}
			}),
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
		),
    "FindFirstChild", [](Instance* self, sol::this_state ts, const std::string& name) -> sol::object {
        if (!self) return sol::make_object(ts, sol::nil);
        Instance* child = self->getChildFromName(name);
        if (!child) return sol::make_object(ts, sol::nil);
        // Convert to the correct derived userdata before returning
        return InstanceToSolObj(ts, child);
    }
		);
	
	auto ResourceType = lua.new_usertype<Resource>("Resource",
		sol::base_classes, sol::bases<Instance>()
	);
	
	auto LuaResourceType = lua.new_usertype<LuaResource>("Script",
		sol::base_classes, sol::bases<Resource, Instance>()
	);
	auto CameraType = lua.new_usertype<Camera>("Camera",
		sol::base_classes, sol::bases<Instance>(),
		"FOV", &Camera::fov,
		"Position", &Camera::position,
		"Rotation", &Camera::rotation,
		"MoveDirection2D", &Camera::moveDirection2D
	);
	auto SceneType = lua.new_usertype<Scene>("Scene",
		sol::base_classes, sol::bases<Instance>(),
		"CurrentCamera", sol::property(
			[](Scene* self, sol::this_state ts) {
				return InstanceToSolObj(ts, self->getCamera());
			},
			[](Scene* self, sol::this_state ts, sol::object value) {
				self->camera = (value.as<Camera*>()->getId());
			}
		)
	);
	auto MeshType = lua.new_usertype<Mesh>("Mesh",
		sol::base_classes, sol::bases<Instance>(),
		"Position", &Mesh::position,
		"Visible", &Mesh::visible,
		"Rotation", &Mesh::rotation,
		"Scale", &Mesh::scale,

		"Material", sol::property(
			[](Mesh* self, sol::this_state ts) {
				return InstanceToSolObj(ts, getInstanceById(self->material));
			},
			[](Mesh* self, sol::this_state ts, sol::object value) {
				self->material = value.as<Material*>()->getId();
			}
		)
	);

	auto MaterialType = lua.new_usertype<Material>("Material",
		sol::base_classes, sol::bases<Instance>(),
		"Color", &Material::diffuseColor,
		"Specularity", &Material::specularity,
		"Shader", sol::property(
			[](Material* self, sol::this_state ts) {
				return InstanceToSolObj(ts, getInstanceById(self->getShaderInstanceId()));
			},
			[](Material* self, sol::this_state ts, sol::object value) {
				self->setShader(value.as<ShaderProgram*>());
			}
		),
		"SetTexture", [](Material *self, sol::this_state ts, sol::object img) {
			self->setTexture(img.as<ImageResource*>());
		},
		"GetTexture", [](Material *self, sol::this_state ts, int target) {
			return (ImageResource*)getInstanceById(self->getTextureInstanceId(target));
		}
	);
	auto ShaderProgramType = lua.new_usertype<ShaderProgram>("ShaderProgram",
		sol::base_classes, sol::bases<Resource, Instance>()
	);

	auto ShaderResourceType = lua.new_usertype<ShaderResource>("ShaderResource",
		sol::base_classes, sol::bases<Resource, Instance>(),
		"ShaderType", sol::readonly(&ShaderResource::shaderType),

		"IsCompiled", &ShaderResource::isValid
	);
	auto ImageResourceType = lua.new_usertype<ImageResource>("ImageResource",
		sol::base_classes, sol::bases<Resource, Instance>(),
		"Width", &ImageResource::getWidth,
		"Height", &ImageResource::getHeight,
		"Channels", &ImageResource::getNumChannels,
		"Size", &ImageResource::getImageBufferLength
	);

	auto ValueType = lua.new_usertype<Value>("ValueBase",
		sol::base_classes, sol::bases<Instance>()
	);

	//values :( at least they work...
	auto IntValueType = lua.new_usertype<IntegerValue>("IntValue",
		sol::base_classes, sol::bases<Value, Instance>(),
		"Value", &IntegerValue::value
	);

	auto FloatValueType = lua.new_usertype<FloatValue>("FloatValue",
		sol::base_classes, sol::bases<Value, Instance>(),
		"Value", &FloatValue::value
	);

	auto StringValueType = lua.new_usertype<StringValue>("StringValue",
		sol::base_classes, sol::bases<Value, Instance>(),
		"Value", &StringValue::value
	);
	auto BooleanValueType = lua.new_usertype<StringValue>("BoolValue",
		sol::base_classes, sol::bases<Value, Instance>(),
		"Value", &BooleanValue::value
	);
	auto InstanceValueType = lua.new_usertype<InstanceValue>("Reference",
		sol::base_classes, sol::bases<Value, Instance>(),
		"Value", sol::property(
			[](InstanceValue* self, sol::this_state ts) {
				return InstanceToSolObj(ts, self->get());
			},
			[](InstanceValue* self, sol::this_state ts, sol::object obj) {
				self->value = obj.as<Instance*>()->getId();
			}
		)
	);
	//This freaking blows, but it works i guess
	InstanceType[sol::meta_function::index] =			&LuaResource::__luaIndex;
	ResourceType[sol::meta_function::index] =			&LuaResource::__luaIndex;
	CameraType[sol::meta_function::index] =				&LuaResource::__luaIndex;
	LuaResourceType[sol::meta_function::index] =		&LuaResource::__luaIndex;
	MeshType[sol::meta_function::index] =				&LuaResource::__luaIndex;
	MaterialType[sol::meta_function::index] =			&LuaResource::__luaIndex;
	ImageResourceType[sol::meta_function::index] =		&LuaResource::__luaIndex;
	ShaderProgramType[sol::meta_function::index] =		&LuaResource::__luaIndex;
	ShaderResourceType[sol::meta_function::index] =		&LuaResource::__luaIndex;
	SceneType[sol::meta_function::index] =				&LuaResource::__luaIndex;
	ValueType[sol::meta_function::index] =				&LuaResource::__luaIndex;
	IntValueType[sol::meta_function::index] =			&LuaResource::__luaIndex;
	FloatValueType[sol::meta_function::index] =			&LuaResource::__luaIndex;
	StringValueType[sol::meta_function::index] =		&LuaResource::__luaIndex;
	BooleanValueType[sol::meta_function::index] =		&LuaResource::__luaIndex;
	InstanceValueType[sol::meta_function::index] =		&LuaResource::__luaIndex;

}
LuaResource::~LuaResource() {

}
sol::object LuaResource::__luaIndex(Instance* self, sol::this_state ts, const std::string& key) {
	if (lua[getInstanceNameFromType(self->type)][key].valid()) {
		//printf("Found pre method %s\n", key.c_str());
		return lua[getInstanceNameFromType(self->type)][key];
	}

	//otherwise query children

	if (self->getChildFromName(key) != nullptr) {
		//printf("Found child %s\n", key.c_str());
		return InstanceToSolObj(ts, self->getChildFromName(key));
	}
	//if all else fails , return nil

	return sol::make_object(ts, sol::nil);
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
	env["me"] = InstanceToSolObj(sol::this_state(lua), this);
	env["MouseData"] = &(Graphics::getInstance()->mouse_data);
	env["KeyboardData"] = &(Graphics::getInstance()->keyboard_data);
	env["time"] = &glfwGetTime;
	env["scene"] = InstanceToSolObj(sol::this_state(lua), Scene::getCurrentScene());
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
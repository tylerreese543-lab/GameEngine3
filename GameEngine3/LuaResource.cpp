#include "LuaResource.h"
#include "Graphics/Graphics.h"
#include "Camera.h"
#include "Mesh.h"
#include "Scene.h"
#include "Material.h"
#include "Shader.h"
#include "Value.h"
#include "AABB.h"
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
	if (!i) {
		return sol::make_object(ts, nullptr);
	}
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
	case InstanceType_Vector3Value:
		return sol::make_object(ts, (Vector3Value*)i);
	case InstanceType_Collision2D:
		return sol::make_object(ts, (Collision2D*)i);
	case InstanceType_Collision3D:
		return sol::make_object(ts, (Collision3D*)i);
	case InstanceType_ProceduralGeometry:
		return sol::make_object(ts, (ProceduralGeometry*)i);
	default:
		//Base instance class - no casting necessary
		return sol::make_object(ts, i);
	}
}
void LuaResource::InitializeLua() {
	lua.open_libraries(sol::lib::base, sol::lib::math);

	sol::table enumRoot = lua.create_named_table("Enum");
	enumRoot.new_enum("KeyCode",
		"Q", GLFW_KEY_Q,
		"W", GLFW_KEY_W,
		"E", GLFW_KEY_E,
		"R", GLFW_KEY_R,
		"T", GLFW_KEY_T,
		"Y", GLFW_KEY_Y,
		"U", GLFW_KEY_U,
		"I", GLFW_KEY_I,
		"O", GLFW_KEY_O,
		"P", GLFW_KEY_P,

		"A", GLFW_KEY_A,
		"S", GLFW_KEY_S,
		"D", GLFW_KEY_D,
		"F", GLFW_KEY_F,
		"G", GLFW_KEY_G,
		"H", GLFW_KEY_H,
		"J", GLFW_KEY_J,
		"K", GLFW_KEY_K,
		"L", GLFW_KEY_L,

		"M", GLFW_KEY_M,
		"N", GLFW_KEY_N,
		"B", GLFW_KEY_B,
		"V", GLFW_KEY_V,
		"C", GLFW_KEY_C,
		"X", GLFW_KEY_X,
		"Z", GLFW_KEY_Z,

		"Up", GLFW_KEY_UP,
		"Down", GLFW_KEY_DOWN,
		"Left", GLFW_KEY_LEFT,
		"Right", GLFW_KEY_RIGHT,

		"Spacebar", GLFW_KEY_SPACE,
		"LeftShift", GLFW_KEY_LEFT_SHIFT,
		"RightShift", GLFW_KEY_RIGHT_SHIFT,
		"LeftAlt", GLFW_KEY_LEFT_ALT,
		"RightAlt", GLFW_KEY_RIGHT_ALT,
		"LeftControl", GLFW_KEY_LEFT_CONTROL,
		"RightControl", GLFW_KEY_LEFT_CONTROL
	);
	enumRoot.new_enum("MouseButton",
		"Left", GLFW_MOUSE_BUTTON_LEFT,
		"Right", GLFW_MOUSE_BUTTON_RIGHT,
		"Middle", GLFW_MOUSE_BUTTON_MIDDLE
	);

	enumRoot.new_enum("GamepadButton",
		"ACross", GLFW_GAMEPAD_BUTTON_A,
		"BCircle", GLFW_GAMEPAD_BUTTON_B,
		"XSquare", GLFW_GAMEPAD_BUTTON_X,
		"YTriangle", GLFW_GAMEPAD_BUTTON_Y,

		"DPadLeft", GLFW_GAMEPAD_BUTTON_DPAD_LEFT,
		"DPadRight", GLFW_GAMEPAD_BUTTON_DPAD_RIGHT,
		"DPadUp", GLFW_GAMEPAD_BUTTON_DPAD_UP,
		"DPadDown", GLFW_GAMEPAD_BUTTON_DPAD_DOWN,

		"HatLeft", GLFW_GAMEPAD_BUTTON_LEFT_THUMB,
		"HatRight", GLFW_GAMEPAD_BUTTON_RIGHT_THUMB,
		"BumperLeft", GLFW_GAMEPAD_BUTTON_LEFT_BUMPER,
		"BumperRight", GLFW_GAMEPAD_BUTTON_RIGHT_BUMPER,

		"Start", GLFW_GAMEPAD_BUTTON_START,
		"Back", GLFW_GAMEPAD_BUTTON_BACK
	);

	enumRoot.new_enum("ShaderType",
		"Vertex", GL_VERTEX_SHADER,
		"Fragment", GL_FRAGMENT_SHADER
	);

	enumRoot.new_enum("BufferUpdateMode",
		"Static", GL_STATIC_DRAW,
		"Dynamic", GL_DYNAMIC_DRAW,
		"Stream", GL_STREAM_DRAW
	);
	enumRoot.new_enum("PolygonDrawType",
		"Triangles", GL_TRIANGLES,
		"Lines", GL_LINES,
		"TriangleFan", GL_TRIANGLE_FAN,
		"TriangleStrip", GL_TRIANGLE_STRIP,
		"Points", GL_POINTS
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

	lua.new_usertype<ScriptEvent>("ScriptEvent",
		"Connect", &ScriptEvent::connect
	);
	lua.new_usertype<MouseData>("MouseData",
		"Position", sol::readonly(&MouseData::screenFull),
		"SetMouseInputMode", &MouseData::setRawMouseInput,
		"IsMouseDown", &MouseData::isMouseDown,
		"IsRawMouseInput", &MouseData::isRawMouseInput,
		"OnLeftClick", &MouseData::on_left_click,
		"OnLeftRelease", &MouseData::on_left_release,
		"OnRightClick", &MouseData::on_right_click,
		"OnRightRelease", &MouseData::on_right_release
	);
	lua.new_usertype<Gamepad>("Gamepad",
		"LeftJoystick", sol::property(
			[](Gamepad* self, sol::this_state ts) {
				return self->getJoystickLocation(0);
			}
		),
		"RightJoystick", sol::property(
			[](Gamepad* self, sol::this_state ts) {
				return self->getJoystickLocation(1);
			}
		),
		"LeftTrigger", sol::property(
			[](Gamepad* self, sol::this_state ts) {
				return self->getTriggerPress(1);
			}
		),
		"RightTrigger", sol::property(
			[](Gamepad* self, sol::this_state ts) {
				return self->getTriggerPress(1);
			}
		),
		"Dpad", sol::property(
			[](Gamepad* self, sol::this_state ts) {
				return self->getDPadVector();
			}
		),
		"IsButtonDown", &Gamepad::isButtonPressed
	);

	lua.new_usertype<GamepadData>("GamepadData",
		"Get", &GamepadData::getGamepad,
		"OnConnect", &GamepadData::on_connect,
		"OnDisconnect", &GamepadData::on_disconnect
	);
	lua.new_usertype<KeyboardData>("KeyboardData",
		"IsKeyDown", &KeyboardData::isKeyDown,
		"OnKeyDown", &KeyboardData::on_key_down,
		"OnKeyUp", &KeyboardData::on_key_up
	);
	// Register the GameInstance usertype
  //TODO: eventually abstract this to only have access to ID
	auto InstanceType = lua.new_usertype<Instance>("Instance",
		// Expose variables
		"Name", &Instance::name,
		"ID", sol::property(&Instance::getId),
		"ClassName", sol::property([](Instance* self, sol::this_state ts) {
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
		},
		"FindFirstChildOfClass", [](Instance* self, sol::this_state ts, const std::string& className) -> sol::object {
			if (!self) return sol::make_object(ts, sol::nil);
			Instance* child = self->getChildFromClass(className);
			if (!child) return sol::make_object(ts, sol::nil);
			// Convert to the correct derived userdata before returning
			return InstanceToSolObj(ts, child);
		},
		"GetChildren", [](Instance* self, sol::this_state ts) {
			return SolTableFromIV(ts, &(self->children));
		},
		"Clone", [](Instance* self, sol::this_state ts) {
			Instance* clone = self->clone();
			return InstanceToSolObj(ts, clone);
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
		"new", sol::factories([](sol::this_state ts) {
			return InstanceToSolObj(ts, new Camera());
		},
		[](sol::this_state ts, vec3 v) {
			return InstanceToSolObj(ts, new Camera(v));
		}),
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

	auto GeometryType = lua.new_usertype<Geometry>("Geometry",
		sol::base_classes, sol::bases<RenderObject, Instance>(),
		"new", sol::factories([](sol::this_state ts) {
			return InstanceToSolObj(ts, new Geometry());
		})
	);
	auto MeshType = lua.new_usertype<Mesh>("Mesh",
		sol::base_classes, sol::bases<Instance>(),
		"new", sol::factories([](sol::this_state ts) {
			return InstanceToSolObj(ts, new Mesh());
		}),
		"Position", &Mesh::position,
		"Visible", &Mesh::visible,
		"Rotation", &Mesh::rotation,
		"Scale", &Mesh::scale,

		"Material", sol::property(
			[](Mesh* self, sol::this_state ts) {
				return InstanceToSolObj(ts, getInstanceById(self->material));
			},
			[](Mesh* self, sol::this_state ts, sol::object value) {
				self->material = value.as<Instance*>()->getId();
			}
		),
		"Geometry", sol::property(
			[](Mesh* self, sol::this_state ts) {
				return InstanceToSolObj(ts, getInstanceById(self->geometry));
			},
			[](Mesh* self, sol::this_state ts, sol::object value) {
				self->geometry = value.as<Instance*>()->getId();
			}
		)
	);

	auto MaterialType = lua.new_usertype<Material>("Material",
		sol::base_classes, sol::bases<Instance>(),
		"new", sol::factories([](sol::this_state ts) {
			return InstanceToSolObj(ts, new Material());
		}),
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
		sol::base_classes, sol::bases<Resource, Instance>(),
		"Create", &ShaderProgram::createProgram
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
		"Size", &ImageResource::getImageBufferLength,
		"Target", &ImageResource::textureType,
		"LoadFromFile", &ImageResource::loadFromFile
	);

	auto ValueType = lua.new_usertype<Value>("ValueBase",
		sol::base_classes, sol::bases<Instance>()
	);

	//values :( at least they work...
	auto IntValueType = lua.new_usertype<IntegerValue>("IntValue",
		sol::base_classes, sol::bases<Value, Instance>(),
		"new", sol::factories([](sol::this_state ts) {
			return InstanceToSolObj(ts, new IntegerValue());
		}),
		"Value", &IntegerValue::value
	);

	auto FloatValueType = lua.new_usertype<FloatValue>("FloatValue",
		sol::base_classes, sol::bases<Value, Instance>(),
		"new", sol::factories([](sol::this_state ts) {
			return InstanceToSolObj(ts, new FloatValue());
		}),
		"Value", &FloatValue::value
	);

	auto StringValueType = lua.new_usertype<StringValue>("StringValue",
		sol::base_classes, sol::bases<Value, Instance>(),
		"new", sol::factories([](sol::this_state ts) {
			return InstanceToSolObj(ts, new StringValue());
		}),
		"Value", &StringValue::value
	);
	auto BooleanValueType = lua.new_usertype<BooleanValue>("BoolValue",
		sol::base_classes, sol::bases<Value, Instance>(),
		"new", sol::factories([](sol::this_state ts) {
			return InstanceToSolObj(ts, new BooleanValue());
		}),
		"Value", &BooleanValue::value
	);
	auto Vector3ValueType = lua.new_usertype<Vector3Value>("Vector3Value",
		sol::base_classes, sol::bases<Value, Instance>(),
		"new", sol::factories([](sol::this_state ts) {
			return InstanceToSolObj(ts, new Vector3Value());
		}),
		"Value", &Vector3Value::value
	);
	auto InstanceValueType = lua.new_usertype<InstanceValue>("InstanceValue",
		sol::base_classes, sol::bases<Value, Instance>(),
		"new", sol::factories([](sol::this_state ts) {
			return InstanceToSolObj(ts, new InstanceValue());
		}),
		"Value", sol::property(
			[](InstanceValue* self, sol::this_state ts) {
				return InstanceToSolObj(ts, self->get());
			},
			[](InstanceValue* self, sol::this_state ts, sol::object obj) {
				self->value = obj.as<Instance*>()->getId();
			}
		)
	);

	auto Collision2DType = lua.new_usertype<Collision2D>("Collision2D",
		sol::base_classes, sol::bases<Instance>(),
		"new", sol::factories([](sol::this_state ts) {
			return InstanceToSolObj(ts, new Collision2D());
		}),
		"Position", sol::property(
			[](Collision2D* self, sol::this_state ts) {
				return self->getPosition();
			},
			[](Collision2D* self, sol::this_state ts, vec2 pos) {
				self->setPosition(pos);
			}
		),

		"Size", sol::property(
			[](Collision2D* self, sol::this_state ts) {
				return self->getSize();
			},
			[](Collision2D* self, sol::this_state ts, vec2 size) {
				self->setSize(size);
			}
		),

		"Center", sol::property(
			[](Collision2D* self, sol::this_state ts) {
				return self->getCenter();
			}
		),

		"IsTouching", [](Collision2D* self, sol::this_state ts, Collision2D* other) {
			return self->checkCollision(other);
		},

		"IsInside", [](Collision2D* self, sol::this_state ts, vec2 point) {
			return self->isInside(point);
		}
	);

	auto Collision3DType = lua.new_usertype<Collision3D>("Collision3D",
		sol::base_classes, sol::bases<Instance>(),
		"new", sol::factories([](sol::this_state ts) {
			return InstanceToSolObj(ts, new Collision3D());
		}),
		"Position", sol::property(
			[](Collision3D* self, sol::this_state ts) {
				return self->getPosition();
			},
			[](Collision3D* self, sol::this_state ts, vec3 pos) {
				self->setPosition(pos);
			}
		),

		"Size", sol::property(
			[](Collision3D* self, sol::this_state ts) {
				return self->getSize();
			},
			[](Collision3D* self, sol::this_state ts, vec3 size) {
				self->setSize(size);
			}
		),

		"Center", sol::property(
			[](Collision3D* self, sol::this_state ts) {
				return self->getCenter();
			}
		),

		"IsTouching", [](Collision3D* self, sol::this_state ts, Collision3D* other) {
			return self->checkCollision(other);
		},

		"IsInside", [](Collision3D* self, sol::this_state ts, vec3 point) {
			return self->isInside(point);
		}
	);

	auto ProceduralGeometryType = lua.new_usertype<ProceduralGeometry>("ProceduralGeometry",
		sol::base_classes, sol::bases<Geometry, RenderObject, Instance>(),
		"new", sol::factories([](sol::this_state ts) {
			return InstanceToSolObj(ts, new ProceduralGeometry());
			}),
		"AddVertex", [](ProceduralGeometry* self, sol::this_state ts, sol::optional<vec3> position, sol::optional<vec2> uv, sol::optional<vec3> normal, sol::optional<vec3> color) {

			self->addVertex(ProceduralVertex(
				position.value_or(vec3()),
				normal.value_or(vec3()),
				uv.value_or(vec2()),
				color.value_or(vec3())
			));
		},
		"Dirty", sol::property(&ProceduralGeometry::isBuffersDirty),
		"Empty", sol::property(&ProceduralGeometry::isEmpty),
		"AddTriangle", &ProceduralGeometry::addTriangle,
		"SetVertexPosition", &ProceduralGeometry::setVertexPosition,
		"SetVertexUV", &ProceduralGeometry::setVertexUV,
		"SetVertexNormal", &ProceduralGeometry::setVertexNormal,
		"SetVertexColor", &ProceduralGeometry::setVertexColor,

		"SetVertexFloatData", [](ProceduralGeometry* self, sol::this_state ts, int index, std::vector<float> data) {
			self->setVertexExtraFloatData(index, data.data());
		},

		"SetNumExtraFloats", &ProceduralGeometry::setNumExtraFloats,

		"BufferUpdateMode", &ProceduralGeometry::bufferUpdateMode,
		"IsDrawElements", &ProceduralGeometry::drawElements
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
	Vector3ValueType[sol::meta_function::index] =		&LuaResource::__luaIndex;
	Collision2DType[sol::meta_function::index] =		&LuaResource::__luaIndex;
	Collision3DType[sol::meta_function::index] =		&LuaResource::__luaIndex;
	ProceduralGeometryType[sol::meta_function::index] =	&LuaResource::__luaIndex;
	GeometryType[sol::meta_function::index] =			&LuaResource::__luaIndex;
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
	env["GamepadData"] = &(Graphics::getInstance()->gamepad_data);
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

sol::table LuaResource::SolTableFromIV(sol::this_state ts, std::vector<Instance*>* instances) {
	sol::table objects = lua.create_table();
	for (Instance*& i : *instances) {
		objects.add(InstanceToSolObj(ts, i));
	}

	return objects;
}
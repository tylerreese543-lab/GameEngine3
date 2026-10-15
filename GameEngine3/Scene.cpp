#include "Scene.h"
#include "AABB.h"
#include "LuaResource.h"
#include "Shader.h"

#ifdef STUDIO_EDITOR
const bool Scene::isStudio = true;
#else
const bool Scene::isStudio = false;
#endif
int Scene::currentScene = -1;
Scene::Scene() : Instance() {
	name = "Scene";
	type = InstanceType_Scene;
	currentCamera = -1;

	if (currentScene == -1) {
		currentScene = getId();
	}
}

void Scene::PreRender() {
	Graphics* g = Graphics::getInstance();
	g->clearWindow();
	g->gamepad_data.UpdateGamepads();
	if (isStudio && !getInstanceById(currentCamera)) {
		for (auto pair : Instance::world) {
			Instance* child = pair.second;
			if (!child) continue;
			if (child->type == InstanceType_Camera && child->omitFromHierarchyRender()) {
				currentCamera = child->getId();
				break;
			}
		}	
	}
	Camera* c = (Camera*)getInstanceById(currentCamera);
	if (!c) {
		debug_log("Scene::PreRender: No valid camera found for scene '%s' (ID %d). Rendering skipped.\n", name.c_str(), getId());
		return;
	}
	c->updateMatrices();
	c->aspect_ratio = Graphics::getInstance()->getAspectRatio();
	handleDestroyQueue();
}
void Scene::RenderScene() {
	Mesh::renderAllMeshes((Camera*)getInstanceById(currentCamera));
	Collision3D::RenderAllCollisionBoxesDebug();
	for(auto pair : Instance::world) {
		Instance* child = pair.second;
		if (!child) continue;
		if (child->type == InstanceType_Camera && isStudio && !child->omitFromHierarchyRender()) {
			//printf("Rendering debug for camera '%s' (ID %d)\n", child->name.c_str(), child->getId());
			((Camera*)child)->debugRender();
		}
	}
}

void Scene::setCurrentScene(int scene) {
	if (getInstanceById(scene) == nullptr) {
		printf("Scene::setCurrentScene: Scene with ID %d does not exist.\n", scene);
		return;
	}

	if (getInstanceById(scene)->type != InstanceType_Scene) {
		printf("Scene::setCurrentScene: Instance with ID %d is not a Scene.\n", scene);
		return;
	}

	if (!isIdValid(scene)) {
		printf("Scene::setCurrentScene: Scene with ID %d is not valid.\n", scene);
		return;
	}
	currentScene = scene;
}

Scene* Scene::getCurrentScene() {
	return (Scene*)getInstanceById(currentScene);
}

Camera* Scene::getCamera() {
	if (!isIdValid(currentCamera)) {
		currentCamera = getChildIdFromType(InstanceType_Camera);
	}
	return (Camera*)getInstanceById(currentCamera);
}

Camera* Scene::getCurrentCamera() {
	Scene* s = getCurrentScene();
	if (!s) {
		printf("Scene::getCurrentCamera: No current scene set.\n");
		return nullptr;
	}
	return s->getCamera();
}

int Scene::saveInstanceToFile(FILE* file) {
	int offset = 0;
	offset += Instance::saveInstanceToFile(file);
	offset += fwrite(&currentCamera, sizeof(int), 1, file);
	offset += fwrite(&hitboxShader, sizeof(int), 1, file);
	offset += fwrite(&startCamera, sizeof(int), 1, file);
	return offset;
}

int Scene::loadInstanceFromFile(void* data, int off) {
	int o = off;
	o += Instance::loadInstanceFromFile(data, o);
	memcpy(&currentCamera, offset(data, o), sizeof(int));
	o += sizeof(int);
	memcpy(&hitboxShader, offset(data, o), sizeof(int));
	o += sizeof(int);
	memcpy(&startCamera, offset(data, o), sizeof(int));
	o += sizeof(int);
	return o - off;
}

void Scene::WriteGameFileHeader(FILE* file) {
	// Magic
	fwrite("GE3X", sizeof(char), 4, file);

	// Version string (write terminating NUL so loader can read safely)
	char versionString[128];
	sprintf_s(versionString, sizeof(versionString), "%d.%d.%d", GAME_VERSION_MAJOR, GAME_VERSION_MINOR, GAME_VERSION_PATCH);
	fwrite(versionString, sizeof(char), strlen(versionString) + 1, file); // include '\0'

	// Number of instances (use int consistently)
	int numInstances = 0;
	for (auto pair : Instance::world) {
		if (!pair.second) continue;
		if (!pair.second->omitFromSave()) {
			numInstances++;
		}
	}
	fwrite(&numInstances, sizeof(int), 1, file);

	// Current scene id (guard against null)
	int currentSceneId = -1;
	Scene* cur = Scene::getCurrentScene();
	if (cur) {
		currentSceneId = cur->getId();
	}
	fwrite(&currentSceneId, sizeof(int), 1, file);

	// Total bytes of saved instances (include BOTH tag + numBytes for each saved instance)
	int total = 0;
	for (auto pair : Instance::world) {
		if (!pair.second->omitFromSave()) {
			// each saved instance on disk = tag(int) + numBytes(int) + payload (getNumSaveBytes)
			total += pair.second->getNumSaveBytes() + (int)(2 * sizeof(int));
		}
	}
	fwrite(&total, sizeof(int), 1, file);
};

int Scene::saveSceneToFile(const char* filename) {
	FILE* f_main;
	fopen_s(&f_main, filename, "wb");

	if (!f_main) {
		printf("Error saving to file: could not open\n");
		return 1;
	}

	WriteGameFileHeader(f_main);

	for (auto pair : Instance::world) {
		if (pair.second->saveToFile) {
			if (!pair.second->omitFromSave()) {
				printf("Saving instance ID %d, type %d (%s)\n", pair.second->getId(), pair.second->type, Instance::getInstanceNameFromType(pair.second->type).c_str());
				pair.second->saveInstanceToFile(f_main);
			}
		}
	}
	char endTag[4] = { 'E', 'N', 'D', '\0' };
	fwrite(&endTag, sizeof(char), 4, f_main);

	fclose(f_main);
}

int Scene::loadSceneFromFile(const char* filename) {


	inst_conv_map instanceIdConversionMap; // Map to store old ID to new ID mapping
	FILE* f_main;
	fopen_s(&f_main, Resource::resolveFilePath(filename).c_str(), "rb");

	if (!f_main) {
		printf("Error loading from file: could not open\n");
		return 1;
	}
	int numInstances = 0;
	if (!LoadGameFileHeader(f_main, &numInstances)) {
		printf("Error loading from file: invalid header\n");
		return 1;
	}

	int totalBytesToRead = 0;

	fread(&totalBytesToRead, sizeof(int), 1, f_main);

	char* buffer = new char[totalBytesToRead];

	fread(buffer, sizeof(char), totalBytesToRead, f_main);

	int o = 0;

	std::set<int> loadedInstanceIds; // To track loaded instance IDs
	for(int i = 0; i < numInstances; ++i) {
		int tag = *(int*)offset(buffer, o);
		o += sizeof(int);
		printf("Tag ID %d (hex: 0x%X), Instance #%d\n",tag, tag, i);
		if (tag == LOAD_INSTANCE_TAG) {
			int numBytes = *(int*)offset(buffer, o);
			o += sizeof(int);
			int type = *(int*)offset(buffer, o);
			o += sizeof(int);

			Instance* inst = nullptr;

			printf("Loading instance type %d, %s\n", type, Instance::getInstanceNameFromType(type).c_str());
			if (type == Instance::InstanceType_Scene) {
				inst = new Scene();
			} else
			if (type == Instance::InstanceType_Undefined) {
				inst = new Instance();
			} else
			if (type == Instance::InstanceType_Base) {
				inst = new Instance();
			}
			else if (type == Instance::InstanceType_Camera) {
				inst = new Camera();
			}
			else if (type == Instance::InstanceType_Mesh) {
				inst = new Mesh();
			}
			else if (type == Instance::InstanceType_Geometry) {
				inst = new Geometry();
			}
			else if (type == Instance::InstanceType_ProceduralGeometry) {
				inst = new ProceduralGeometry();
			}
			else if (type == Instance::InstanceType_Material) {
				inst = new Material();
			}
			else if (type == Instance::InstanceType_LuaResource) {
				inst = new LuaResource();
			}
			else if (type == Instance::InstanceType_ShaderProgram) {
				inst = new ShaderProgram();
			}
			else if (type == Instance::InstanceType_ShaderResource) {
				inst = new ShaderResource();
			}
			else if (type == Instance::InstanceType_ImageResource) {
				inst = new ImageResource();
			}
			else if (type == Instance::InstanceType_Collision2D) {
				inst = new Collision2D();
			}
			else if (type == Instance::InstanceType_Collision3D) {
				inst = new Collision3D();
			}
			else {
				printf("Error loading from file: unknown instance type %d\n", type);
				return 1;
			}

			loadedInstanceIds.insert(inst->getId()); // Track the loaded instance ID
			o += inst->loadInstanceFromFile(buffer, o);
			instanceIdConversionMap[inst->savedId] = inst->getId(); // Store the mapping of old ID to new ID
		}

	}

	for (int id : loadedInstanceIds) {
		getInstanceById(id)->finalizeLoad(&instanceIdConversionMap);
	}

	Scene::setCurrentScene(instanceIdConversionMap[currentScene]);
	Scene* cur = Scene::getCurrentScene();
	cur->startCamera = instanceIdConversionMap[cur->startCamera];
	cur->currentCamera = instanceIdConversionMap[cur->currentCamera];

	printf("Finished loading scene from file: %d instances, acurrent scene ID: %d\n", numInstances, Scene::getCurrentScene()->getId());
	return 0;
}

bool Scene::LoadGameFileHeader(FILE* f, int* numInstances) {
	int o = 0;
	char header[4];
	fread(header, sizeof(char), 4, f);
	if (strncmp(header, "GE3X", 4) != 0) {
		printf("Error loading from file: invalid header\n");
		return false;
	}
	o += 4;
	char versionString[128];
	for (int i = 0; i < 128; i++) {
		fread(&versionString[i], sizeof(char), 1, f);
		if (versionString[i] == '\0') {
			break;
		}
	}
	o += strlen(versionString) + 1;

	fread(numInstances, sizeof(int), 1, f);

	fread(&currentScene, sizeof(int), 1, f);

	printf("Loading scene from file: %d instances, current scene ID: %d\n", *numInstances, currentScene);
	return true;
}

int Scene::getNumSaveBytes() {
	return sizeof(int) + sizeof(int) + Instance::getNumSaveBytes();
}
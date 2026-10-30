#include "Scene.h"
#include "AABB.h"
#include "LuaResource.h"
#include "Shader.h"
#include "TextGeometry.h"
#include "SoundInstance.h"

#define LOAD_INSTANCE_TAG 10
#define LOAD_FONT_TAG 20
#define LOAD_SOUND_TAG 30
#define END_SEQUENCE_TAG 40

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
	startCamera = -1;
	uiRoot = -1;
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
	handleDestroyQueue();
}
void Scene::RenderScene() {
	UIRootNode* node = (UIRootNode*)getInstanceById(uiRoot);
	Camera* ui_cam = nullptr;
	if (node) {
		ui_cam = node->getCamera();
	}
	Mesh::renderAllMeshes((Camera*)getInstanceById(currentCamera), ui_cam);
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
		printf("Scene::getCamera: Current camera ID %d is not valid. Attempting to find a valid camera.\n", currentCamera);
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
	offset += fwrite(&uiRoot, sizeof(int), 1, file);
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
	memcpy(&uiRoot, offset(data, o), sizeof(int));
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
		if (!pair.second) continue;
		if (!pair.second->omitFromSave()) {
			// each saved instance on disk = tag(int) + numBytes(int) + payload (getNumSaveBytes)
			total += pair.second->getNumSaveBytes() + (int)(3 * sizeof(int));
		}
	}
	for (auto pair : TextGeometry::fontCharacterMaps) {
		if (pair.second.loaded) {
			total += pair.second.getNumSaveBytes() + (2 * sizeof(int));
		}
	}
	for (auto pair : Audio::cachedAudioTracks) {
		if (pair.second.loaded) {
			total += pair.second.getNumSaveBytes() + (2 * sizeof(int));
		}
	}
	total += 2 * sizeof(int); //End Sequence Tag
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
		if (!pair.second) continue;
			if (!pair.second->omitFromSave()) {
				writeTagAndSizeToFile(LOAD_INSTANCE_TAG, pair.second->getNumSaveBytes(), f_main);
				printf("Saving instance ID %d, type %d (%s)\n", pair.second->getId(), pair.second->type, Instance::getInstanceNameFromType(pair.second->type).c_str());
				pair.second->saveInstanceToFile(f_main);
			}
	}

	for (auto pair : TextGeometry::fontCharacterMaps) {
		if (!pair.second.loaded) continue;
		writeTagAndSizeToFile(LOAD_FONT_TAG, pair.second.getNumSaveBytes(), f_main);
		pair.second.serializeToFile(f_main);
	}

	for (auto pair : Audio::cachedAudioTracks) {
		if (!pair.second.loaded) continue;
		writeTagAndSizeToFile(LOAD_SOUND_TAG, pair.second.getNumSaveBytes(), f_main);
		pair.second.serializeToFile(f_main);
	}
	writeTagAndSizeToFile(END_SEQUENCE_TAG, 0, f_main);

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

	while(o < totalBytesToRead) {
		int tag = *(int*)offset(buffer, o);
		o += sizeof(int);
		int numBytes = *(int*)offset(buffer, o);
		o += sizeof(int);
		printf("Tag ID %d (hex: 0x%X), size = %d\n",tag, tag, numBytes);
		if (tag == LOAD_INSTANCE_TAG) {
			int type = *(int*)offset(buffer, o);
			o += sizeof(int);

			Instance* inst = newInstanceFromType(type);

			printf("Loading instance type %d, %s\n", type, Instance::getInstanceNameFromType(type).c_str());

			loadedInstanceIds.insert(inst->getId()); // Track the loaded instance ID
			printf("ID=%d, Said %d, Load %d\n", inst->getId(), numBytes, inst->loadInstanceFromFile(buffer, o));
			instanceIdConversionMap[inst->savedId] = inst->getId(); // Store the mapping of old ID to new ID
		}
		else if (tag == LOAD_FONT_TAG) {
			Font::deserializeFromFile(buffer, o);
		}
		else if (tag == LOAD_SOUND_TAG) {
			AudioTrack::deserializeFromFile(buffer, o);
		}
		else if (tag == END_SEQUENCE_TAG) {
			continue;
		}

		o += numBytes;
	}


	Scene::setCurrentScene(instanceIdConversionMap[currentScene]);
	Scene* cur = Scene::getCurrentScene();
	cur->hitboxShader = instanceIdConversionMap[cur->hitboxShader];
	cur->startCamera = instanceIdConversionMap[cur->startCamera];
	cur->uiRoot = instanceIdConversionMap[cur->uiRoot];
	if (!isStudio) {
		cur->currentCamera = instanceIdConversionMap[cur->currentCamera];
		printf("Current camera ID updated to %d\n", cur->currentCamera);
	}

	for (int id : loadedInstanceIds) {
		getInstanceById(id)->finalizeLoad(&instanceIdConversionMap);
	}
	fclose(f_main);
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
	return (4 * sizeof(int)) +Instance::getNumSaveBytes();
}

Instance* Scene::newInstanceFromType(int type) {
	Instance* inst = nullptr;
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
			else if (type == Instance::InstanceType_TextGeometry) {
				inst = new TextGeometry();
			}
			else if (type == Instance::InstanceType_UIRootNode) {
				inst = new UIRootNode();
			}
			else if (type == Instance::InstanceType_Audio) {
				inst = new Audio();
			}
			else {
				debug_log("Error creating new Instance: unknown instance type %d\n", type);
				return nullptr;
			}
			return inst;
}

UIRootNode::UIRootNode() {
	name = "UIRootNode";
	type = InstanceType_UIRootNode;

	camera = -1;
}

void UIRootNode::setCamera(int id) {
	if (!getInstanceById(id)) return;
	if (isInstanceOf(getInstanceById(id)->type, InstanceType_Camera)) {
		camera = id;
	}
}

Camera* UIRootNode::getCamera() {
	return (Camera*)getInstanceById(camera);
}

int UIRootNode::saveInstanceToFile(FILE* file) {
	int offset = Instance::saveInstanceToFile(file);
	offset += fwrite(&camera, sizeof(int), 1, file);
	return offset;
}

int UIRootNode::getNumSaveBytes() {
	return Instance::getNumSaveBytes() + sizeof(int);
}

int UIRootNode::loadInstanceFromFile(void* data, int off) {
	int o = off;
	o += Instance::loadInstanceFromFile(data, o);
	camera = *(int*)offset(data, o);
	o += sizeof(int);

	return o - off;
}

void UIRootNode::finalizeLoad(inst_conv_map* conversionMap) {
	this->camera = conversionMap->at(this->camera);
}

Instance* UIRootNode::clone_this() {
	UIRootNode* c = (UIRootNode*)registerInstance(new UIRootNode(*this), true);
	return c;
}

void Scene::writeTagAndSizeToFile(int tag, int size, FILE* file) {
	fwrite(&tag, sizeof(int), 1, file);
	fwrite(&size, sizeof(int), 1, file);
}
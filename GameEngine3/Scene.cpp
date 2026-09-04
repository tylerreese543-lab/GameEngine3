#include "Scene.h"
#include "AABB.h"
int Scene::currentScene = -1;
Scene::Scene() : Instance() {
	name = "Scene";
	type = InstanceType_Scene;
	camera = -1;

	if (currentScene == -1) {
		currentScene = getId();
	}
}

void Scene::PreRender() {
	Graphics* g = Graphics::getInstance();
	g->clearWindow();
	g->gamepad_data.UpdateGamepads();
	if (!isIdValid(camera)) {
		camera = getChildIdFromType(InstanceType_Camera);
	}
	Camera* c = (Camera*)getInstanceById(camera);
	if (!c) {
		return;
	}
	c->updateMatrices();
	c->aspect_ratio = Graphics::getInstance()->getAspectRatio();
	handleDestroyQueue();
}
void Scene::RenderScene() {
	Mesh::renderAllMeshes((Camera*)getInstanceById(camera));
	Collision3D::RenderAllCollisionBoxesDebug();
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
	if (!isIdValid(camera)) {
		camera = getChildIdFromType(InstanceType_Camera);
	}
	return (Camera*)getInstanceById(camera);
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
	offset += fwrite(&camera, sizeof(int), 1, file);
	return offset;
}

int Scene::loadInstanceFromFile(void* data, int off) {
	int o = off;
	o += Instance::loadInstanceFromFile(data, o);
	memcpy(&camera, offset(data, o), sizeof(int));
	o += sizeof(int);
	return o - off;
}

void Scene::WriteGameFileHeader(FILE* file) {
	fwrite("GE3X", sizeof(char), 4, file);
	char versionString[128];
	sprintf_s(versionString, "%d.%d.%d\n", GAME_VERSION_MAJOR, GAME_VERSION_MINOR, GAME_VERSION_PATCH);
	fwrite(versionString, sizeof(char), strlen(versionString), file);
	int numInstances = Instance::world.size();
	fwrite(&numInstances, sizeof(size_t), 1, file);
	int currentSceneId = Scene::getCurrentScene()->getId();
	fwrite(&currentSceneId, sizeof(int), 1, file);
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
		pair.second->saveInstanceToFile(f_main);
	}

}


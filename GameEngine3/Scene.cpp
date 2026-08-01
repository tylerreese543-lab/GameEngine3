#include "Scene.h"

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
}
void Scene::RenderScene() {
	Mesh::renderAllMeshes((Camera*)getInstanceById(camera));
	Graphics::getInstance()->windowUpdate();
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
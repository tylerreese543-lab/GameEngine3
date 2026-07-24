#include "Scene.h"

Scene::Scene() : Instance() {
	name = "Scene";
	type = InstanceType_Scene;
	camera = -1;
}

void Scene::PreRender() {
	Graphics* g = Graphics::getInstance();
	g->clearWindow();

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
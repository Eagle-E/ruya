#include "scene.h"

ruya::Scene::Scene()
{
}

ruya::Scene::~Scene()
{
	for (auto i = mObjects.begin(); i != mObjects.end(); i++)
	{
		delete *i;
	}

	for (auto i = mLightSources.begin(); i != mLightSources.end(); i++)
	{
		delete* i;
	}
}

/*
* Adds a new obj to the scene. Scene takes ownership and will delete given object on destruction.
* @pre: obj != nullptr
*/
void ruya::Scene::add_object(Object* obj)
{
	mObjects.push_back(obj);
}

void ruya::Scene::add_light(LightSource* light)
{
	mLightSources.push_back(light);
}


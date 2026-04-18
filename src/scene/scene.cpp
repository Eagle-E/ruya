#include "scene.h"

ruya::scene::Scene::Scene()
{
}

ruya::scene::Scene::~Scene()
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
void ruya::scene::Scene::add_object(Object* obj)
{
	mObjects.push_back(obj);
}

void ruya::scene::Scene::add_light(LightBasic* light)
{
	mLightSources.push_back(light);
}


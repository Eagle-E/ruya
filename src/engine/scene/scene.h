#ifndef SCENE_H
#define SCENE_H

#include <list>
#include "engine/scene/object.h"
#include "engine/scene/light_source.h"

using std::list;

namespace ruya
{
	/*
	* Represents the to-be-rendered scene.
	*/
	class Scene
	{
	public:
		Scene();
		~Scene();

		list<Object*>& get_scene_objects() { return mObjects; }
		list<LightSource*>& get_light_sources() { return mLightSources; }
		void add_object(Object* obj);
		void add_light(LightSource* light);
		//bool remove_object(Object* obj); // TODO: is this necessary?

	private:
		list<Object*> mObjects;
		list<LightSource*> mLightSources;
	};
}

#endif
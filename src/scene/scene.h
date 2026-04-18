#ifndef SCENE_H
#define SCENE_H

#include <glm/glm.hpp>
#include <list>

#include "scene/object.h"
#include "scene/light.hpp"

using std::list;
using glm::vec4;

namespace ruya::scene
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
		list<LightBasic*>& get_light_sources() { return mLightSources; }
		void add_object(Object* obj);
		void add_light(LightBasic* light);
		//bool remove_object(Object* obj); // TODO: is this necessary?

        // member variables
        glm::vec3 background_color {1.0f, 1.0f, 1.0f};

	private:
		list<Object*> mObjects;
		list<LightBasic*> mLightSources;
	};
}

#endif
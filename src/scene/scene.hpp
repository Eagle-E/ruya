#ifndef SCENE_H
#define SCENE_H

#include <list>
#include <glm/glm.hpp>
#include <entt/entt.hpp>

#include "scene/light.hpp"

using std::list;
using glm::vec4;

namespace ruya::scene
{
	/*
	* Represents the to-be-rendered scene.
	*/
	struct Scene
	{
        glm::vec3 background_color {1.0f, 1.0f, 1.0f};
		entt::registry registry;
	};
}

#endif
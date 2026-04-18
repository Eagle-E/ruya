#ifndef OBJECT_H
#define OBJECT_H

#include <glm/glm.hpp>
#include <list>
#include <memory>
#include <vector>
#include "scene/mesh.h"
#include "scene/material.h"
#include "core/uuid.h"


#include "render/texture.hpp"
#include "scene/vault.hpp"
#include "scene/components.hpp"

using ruya::scene::TextureID;

using glm::vec4;
using glm::vec3;
using glm::mat4;

using std::list;
using std::shared_ptr;
using ruya::scene::materials::Phong;
using ruya::scene::materials::PhongMaterials;
using ruya::scene::Model;

namespace ruya
{
	/*
		TODO: implement simple ecs, THEN remove this class by two "Storage<>" instances:
			- Storage<EntityID> E
			- Storage<Model>	M
			=> such that M[i] is the Model instance for entity E[i] 
	*/
	struct Object
	{
		// GETTERS & QUERIES
		Model model;
		UUID id = ruya::generate_uuid();
	};
}


#endif

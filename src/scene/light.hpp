#ifndef LIGHT_H
#define LIGHT_H

#include "scene/components.hpp"
#include "core/uuid.h"
#include <glm/glm.hpp>

using glm::vec3;
using ruya::scene::Model;

namespace ruya::scene
{
	
	struct LightBasic
	{
		UUID id = ruya::generate_uuid();
        Model model;
		vec3 position {0.0f};
		vec3 ambient {1.0f};
		vec3 diffuse {1.0f};
		vec3 specular {1.0f};
	};


}


#endif // LIGHT_H
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
		vec3 position {0.0f};
		vec3 ambient {1.0f};
		vec3 diffuse {1.0f};
		vec3 specular {1.0f};
	};

	struct DirectionalLight
	{
		vec3 direction {0.0f, -1.0f, 0.0f};
  
		vec3 ambient {1.0f};
		vec3 diffuse {1.0f};
		vec3 specular {1.0f};
	};

	struct PointLight {    
		vec3 position {0.0f};
		
		float constant = 1.0f;
		float linear = 0.09f;
		float quadratic = 0.032f;  

		vec3 ambient {1.0f};
		vec3 diffuse {1.0f};
		vec3 specular {1.0f};
	};  


}


#endif // LIGHT_H
#ifndef MATERIAL_H
#define MATERIAL_H

#include <glm/glm.hpp>
#include "scene/vault.hpp"

using glm::vec3;
using ruya::scene::ImageID;


namespace ruya::scene::materials
{
    struct Phong 
    {
        vec3 diffuse  {1.0f, 1.0f, 1.0f};    // base color
        vec3 specular {1.0f, 1.0f, 1.0f};   // reflection color
        float shininess = 0.5f; // reflection sharpness
        ImageID diffuse_map = 0;
        ImageID specular_map = 0;
    };

    struct PhongMaterials
    {
        static const Phong emerald;
        static const Phong jade;
        static const Phong obsidian;
        static const Phong pearl;
        static const Phong ruby;
        static const Phong turquoise;
        static const Phong brass;
        static const Phong bronze;
        static const Phong chrome;
        static const Phong copper;
        static const Phong gold;
        static const Phong silver;
        static const Phong black_plastic;
        static const Phong cyan_plastic;
        static const Phong green_plastic;
        static const Phong red_plastic;
        static const Phong white_plastic;
        static const Phong yellow_plastic;
        static const Phong black_rubber;
        static const Phong cyan_rubber;
        static const Phong green_rubber;
        static const Phong red_rubber;
        static const Phong white_rubber;
        static const Phong yellow_rubber;
    };
}

#endif //MATERIAL_H
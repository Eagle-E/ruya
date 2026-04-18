#ifndef COMPONENTS_H
#define COMPONENTS_H

#include <vector>
#include <glad/glad.h>
#include "scene/vault.hpp"
#include "scene/material.h"
#include "core/math/transform.h"

using ruya::scene::TextureID;
using ruya::scene::MeshID;
using ruya::scene::materials::Phong;
using ruya::math::Transform;

namespace ruya::scene
{
    
    
    /* An Element is a renderable unit. It has a single mesh and a texture. */
    struct Element
    {
        MeshID mesh;
        Phong material;
        Transform transform; // local transform of this element.
    };

    /* A Model is a collection of Elements. An entity that needs to be rendered gets a Model component. */
    struct Model
    {
        // TODO: add Transform component?
        std::vector<Element> elements;
    };
}


#endif // COMPONENTS_H
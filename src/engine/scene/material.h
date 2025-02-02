#ifndef MATERIAL_H
#define MATERIAL_H

#include <glm/glm.hpp>
using glm::vec3;

namespace ruya
{
    struct Material 
    {
        vec3 ambient;
        vec3 diffuse;
        vec3 specular;
        float shininess;

        Material(vec3 ambient, vec3 diffuse, vec3 specular, float shininess) 
            : ambient(ambient), diffuse(diffuse), specular(specular), shininess(shininess) {}
        Material(float ar, float ag, float ab, float dr, float dg, float db, float sr, float sg, float sb, float shininess)
            : ambient(ar, ag, ab), diffuse(dr, dg, db), specular(sr, sg, sb), shininess(shininess) {}
        Material() : ambient(0), diffuse(0), specular(0), shininess(0) {}
    };

    struct Materials
    {
        static const Material emerald;
        static const Material jade;
        static const Material obsidian;
        static const Material pearl;
        static const Material ruby;
        static const Material turquoise;
        static const Material brass;
        static const Material bronze;
        static const Material chrome;
        static const Material copper;
        static const Material gold;
        static const Material silver;
        static const Material black_plastic;
        static const Material cyan_plastic;
        static const Material green_plastic;
        static const Material red_plastic;
        static const Material white_plastic;
        static const Material yellow_plastic;
        static const Material black_rubber;
        static const Material cyan_rubber;
        static const Material green_rubber;
        static const Material red_rubber;
        static const Material white_rubber;
        static const Material yellow_rubber;
    };
    /*
    const Material Materials::emerald           (0.0215f,    0.1745f,    0.0215f,    0.07568f,   0.61424f,     0.07568f,     0.633f,       0.727811f,    0.633f,       0.6f);
    const Material Materials::jade              (0.135f,     0.2225f,    0.1575f,    0.54f,      0.89f,        0.63f,        0.316228f,    0.316228f,    0.316228f,    0.1f);
    const Material Materials::obsidian          (0.05375f,   0.05f,      0.06625f,   0.18275f,   0.17f,        0.22525f,     0.332741f,    0.328634f,    0.346435f,    0.3f);
    const Material Materials::pearl             (0.25f,      0.20725f,   0.20725f,   1.0f,       0.829f,       0.829f,       0.296648f,    0.296648f,    0.296648f,    0.088f);
    const Material Materials::ruby              (0.1745f,    0.01175f,   0.01175f,   0.61424f,   0.04136f,     0.04136f,     0.727811f,    0.626959f,    0.626959f,    0.6f);
    const Material Materials::turquoise         (0.1f,       0.18725f,   0.1745f,    0.396f,     0.74151f,     0.69102f,     0.297254f,    0.30829f,     0.306678f,    0.1f);
    const Material Materials::brass             (0.329412f,  0.223529f,  0.027451f,  0.780392f,  0.568627f,    0.113725f,    0.992157f,    0.941176f,    0.807843f,    0.21794872f);
    const Material Materials::bronze            (0.2125f,    0.1275f,    0.054f,     0.714f,     0.4284f,      0.18144f,     0.393548f,    0.271906f,    0.166721f,    0.2f);
    const Material Materials::chrome            (0.25f,      0.25f,      0.25f,      0.4f,       0.4f,         0.4f,         0.774597f,    0.774597f,    0.774597f,    0.6f);
    const Material Materials::copper            (0.19125f,   0.0735f,    0.0225f,    0.7038f,    0.27048f,     0.0828f,      0.256777f,    0.137622f,    0.086014f,    0.1f);
    const Material Materials::gold              (0.24725f,   0.1995f,    0.0745f,    0.75164f,   0.60648f,     0.22648f,     0.628281f,    0.555802f,    0.366065f,    0.4f);
    const Material Materials::silver            (0.19225f,   0.19225f,   0.19225f,   0.50754f,   0.50754f,     0.50754f,     0.508273f,    0.508273f,    0.508273f,    0.4f);
    const Material Materials::black_plastic     (0.0f,       0.0f,       0.0f,       0.01f,      0.01f,        0.01f,        0.50f,        0.50f,        0.50f,        0.25f);
    const Material Materials::cyan_plastic      (0.0f,       0.1f,       0.06f,      0.0f,       0.50980392f,  0.50980392f,  0.50196078f,  0.50196078f,  0.50196078f,  0.25f);
    const Material Materials::green_plastic     (0.0f,       0.0f,       0.0f,       0.1f,       0.35f,        0.1f,         0.45f,        0.55f,        0.45f,        0.25f);
    const Material Materials::red_plastic       (0.0f,       0.0f,       0.0f,       0.5f,       0.0f,         0.0f,         0.7f,         0.6f,         0.6f,         0.25f);
    const Material Materials::white_plastic     (0.0f,       0.0f,       0.0f,       0.55f,      0.55f,        0.55f,        0.70f,        0.70f,        0.70f,        0.25f);
    const Material Materials::yellow_plastic    (0.0f,       0.0f,       0.0f,       0.5f,       0.5f,         0.0f,         0.60f,        0.60f,        0.50f,        0.25f);
    const Material Materials::black_rubber      (0.02f,      0.02f,      0.02f,      0.01f,      0.01f,        0.01f,        0.4f,         0.4f,         0.4f,         0.078125f);
    const Material Materials::cyan_rubber       (0.0f,       0.05f,      0.05f,      0.4f,       0.5f,         0.5f,         0.04f,        0.7f,         0.7f,         0.078125f);
    const Material Materials::green_rubber      (0.0f,       0.05f,      0.0f,       0.4f,       0.5f,         0.4f,         0.04f,        0.7f,         0.04f,        0.078125f);
    const Material Materials::red_rubber        (0.05f,      0.0f,       0.0f,       0.5f,       0.4f,         0.4f,         0.7f,         0.04f,        0.04f,        0.078125f);
    const Material Materials::white_rubber      (0.05f,      0.05f,      0.05f,      0.5f,       0.5f,         0.5f,         0.7f,         0.7f,         0.7f,         0.078125f);
    const Material Materials::yellow_rubber     (0.05f,      0.05f,      0.0f,       0.5f,       0.5f,         0.4f,         0.7f,         0.7f,         0.04f,        0.078125f);                                       
    */
}

#endif //MATERIAL_H
#include "material.h"

using ruya::scene::materials::Phong;
using ruya::scene::materials::PhongMaterials;
#include <glm/glm.hpp>

using glm::vec3;

const Phong PhongMaterials::emerald           (vec3(0.076f, 0.614f, 0.076f), vec3(0.633f, 0.728f, 0.633f), 0.600f);
const Phong PhongMaterials::jade              (vec3(0.540f, 0.890f, 0.630f), vec3(0.316f, 0.316f, 0.316f), 0.100f);
const Phong PhongMaterials::obsidian          (vec3(0.182f, 0.170f, 0.225f), vec3(0.333f, 0.329f, 0.346f), 0.300f);
const Phong PhongMaterials::pearl             (vec3(1.000f, 0.829f, 0.829f), vec3(0.297f, 0.297f, 0.297f), 0.088f);
const Phong PhongMaterials::ruby              (vec3(0.614f, 0.041f, 0.041f), vec3(0.728f, 0.627f, 0.627f), 0.600f);
const Phong PhongMaterials::turquoise         (vec3(0.396f, 0.742f, 0.691f), vec3(0.297f, 0.308f, 0.307f), 0.100f);
const Phong PhongMaterials::brass             (vec3(0.780f, 0.569f, 0.114f), vec3(0.992f, 0.941f, 0.808f), 0.218f);
const Phong PhongMaterials::bronze            (vec3(0.714f, 0.428f, 0.181f), vec3(0.394f, 0.272f, 0.167f), 0.200f);
const Phong PhongMaterials::chrome            (vec3(0.400f, 0.400f, 0.400f), vec3(0.775f, 0.775f, 0.775f), 0.600f);
const Phong PhongMaterials::copper            (vec3(0.703f, 0.270f, 0.083f), vec3(0.257f, 0.138f, 0.086f), 0.100f);
const Phong PhongMaterials::gold              (vec3(0.751f, 0.606f, 0.226f), vec3(0.628f, 0.556f, 0.366f), 0.400f);
const Phong PhongMaterials::silver            (vec3(0.507f, 0.508f, 0.508f), vec3(0.508f, 0.508f, 0.508f), 0.400f);
const Phong PhongMaterials::black_plastic     (vec3(0.010f, 0.010f, 0.010f), vec3(0.500f, 0.500f, 0.500f), 0.250f);
const Phong PhongMaterials::cyan_plastic      (vec3(0.000f, 0.510f, 0.510f), vec3(0.502f, 0.502f, 0.502f), 0.250f);
const Phong PhongMaterials::green_plastic     (vec3(0.100f, 0.350f, 0.100f), vec3(0.450f, 0.550f, 0.450f), 0.250f);
const Phong PhongMaterials::red_plastic       (vec3(0.500f, 0.000f, 0.000f), vec3(0.700f, 0.600f, 0.600f), 0.250f);
const Phong PhongMaterials::white_plastic     (vec3(0.550f, 0.550f, 0.550f), vec3(0.700f, 0.700f, 0.700f), 0.250f);
const Phong PhongMaterials::yellow_plastic    (vec3(0.500f, 0.500f, 0.000f), vec3(0.600f, 0.600f, 0.500f), 0.250f);
const Phong PhongMaterials::black_rubber      (vec3(0.010f, 0.010f, 0.010f), vec3(0.400f, 0.400f, 0.400f), 0.078f);
const Phong PhongMaterials::cyan_rubber       (vec3(0.400f, 0.500f, 0.500f), vec3(0.040f, 0.700f, 0.700f), 0.078f);
const Phong PhongMaterials::green_rubber      (vec3(0.400f, 0.500f, 0.400f), vec3(0.040f, 0.700f, 0.040f), 0.078f);
const Phong PhongMaterials::red_rubber        (vec3(0.500f, 0.400f, 0.400f), vec3(0.700f, 0.040f, 0.040f), 0.078f);
const Phong PhongMaterials::white_rubber      (vec3(0.500f, 0.500f, 0.500f), vec3(0.700f, 0.700f, 0.700f), 0.078f);
const Phong PhongMaterials::yellow_rubber     (vec3(0.500f, 0.500f, 0.400f), vec3(0.700f, 0.700f, 0.040f), 0.078f);
#version 460 core

/*
Preconditions:
    - All inputs are in WORLD SPACE
*/

struct Material 
{
    vec3 diffuse;
    vec3 specular;
    float shininess;
    sampler2D diffuse_map;
    sampler2D specular_map;
}; 

struct BasicLight 
{
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    vec3 position;  
};

/*
`v_` prefix denotes a "varying" variable, meaning its value 
    is interpolated by previous shaders in the pipeline.
*/

#define MAX_BASIC_LIGHTS 128
uniform BasicLight simple_lights[MAX_BASIC_LIGHTS];
uniform uint num_simple_lights;
uniform Material material;
uniform vec3 camera_position;

in vec3 v_position; // fragment position in world space
in vec3 v_normal;
in vec2 v_uv;   // uv texture coordinate, varying: interpolated by vertex shader

out vec4 FragColor;

/* Compute fragment color imposed by given light.
Args:
    - light: the light object whose effect we want to calculate
    - diffuse_sample: color sampled from the diffuse map corresponding with the fragment
    - specular_sample: color sampled from the specular map corresponding with the fragment
    - position: frag pos in world space
    - normal: frag normal in world space
    - camera_position: cam pos in world space
*/
vec3 calc_simple_light(BasicLight light, vec3 diffuse_sample, vec3 specular_sample, vec3 position, vec3 normal, vec3 camera_position)
{
    // ambient color
    vec3 ambient = light.ambient * diffuse_sample.rgb;

    // diffuse color
    vec3 light_dir = normalize(position - light.position);
    float diffuse_factor = max(dot(-light_dir, normal), 0.0);
    vec3 diffuse = diffuse_factor * light.diffuse * material.diffuse * diffuse_sample;

    // specular component
    vec3 view_dir = normalize(position - camera_position);
    vec3 reflection_dir = reflect(light_dir, normal);
    float specular_effect = pow(max(dot(reflection_dir, -view_dir), 0.0), 32);
    vec3 specular = specular_effect * light.specular * material.specular * specular_sample; 

    // final color
    vec3 result = ambient + diffuse + specular;
    return result;
}

void main()
{
    vec3 normal = normalize(v_normal);
    vec3 diffuse_sample = texture(material.diffuse_map, v_uv).rgb;
    vec3 specular_sample = texture(material.specular_map, v_uv).rgb;

    vec3 result = vec3(0,0,0);
    for (int i = 0; i < num_simple_lights; i++)
    {
        result += calc_simple_light(simple_lights[i], diffuse_sample, specular_sample, v_position, normal, camera_position);
    }
    FragColor = vec4(result, 1.0);

    // Debug: render normal value as color
    // vec3 norm_colors = (v_normal + 1) / 2;
    // FragColor = vec4(norm_colors, 1.0);
} 


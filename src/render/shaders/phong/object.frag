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

struct Light 
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

uniform Light light; 
uniform Material material;
uniform vec3 camera_position;

in vec3 v_position; // fragment position in world space
in vec3 v_normal;
in vec2 v_uv;   // uv texture coordinate, varying: interpolated by vertex shader

out vec4 FragColor;

void main()
{
    vec3 albedo = texture(material.diffuse_map, v_uv).rgb;

    // ambient color
    vec3 ambient = light.ambient * albedo.rgb;

    // diffuse color
    vec3 norm = normalize(v_normal);
    vec3 light_dir = normalize(v_position - light.position);
    float diffuse_factor = max(dot(-light_dir, norm), 0.0);
    vec3 diffuse = light.diffuse * diffuse_factor * material.diffuse * albedo;

    // specular component
    vec3 view_dir = normalize(v_position - camera_position);
    vec3 reflection_dir = reflect(light_dir, norm);
    float specular_effect = pow(max(dot(reflection_dir, -view_dir), 0.0), 32);
    vec3 specular = light.specular * specular_effect * material.specular * texture(material.specular_map, v_uv).rgb; 

    // final color
    vec3 result = ambient + diffuse + specular;
    FragColor = vec4(result, 1.0);

    // Debug: render normal value as color
    // vec3 norm_colors = (v_normal + 1) / 2;
    // FragColor = vec4(norm_colors, 1.0);
} 


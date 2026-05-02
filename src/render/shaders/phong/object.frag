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

struct SimpleLight 
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

uniform SimpleLight simple_light; 
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
    vec3 ambient = simple_light.ambient * albedo.rgb;

    // diffuse color
    vec3 normal = normalize(v_normal);
    vec3 light_dir = normalize(v_position - simple_light.position);
    float diffuse_factor = max(dot(-light_dir, normal), 0.0);
    vec3 diffuse = simple_light.diffuse * diffuse_factor * material.diffuse * albedo;

    // specular component
    vec3 view_dir = normalize(v_position - camera_position);
    vec3 reflection_dir = reflect(light_dir, normal);
    float specular_effect = pow(max(dot(reflection_dir, -view_dir), 0.0), 32);
    vec3 specular = simple_light.specular * specular_effect * material.specular * texture(material.specular_map, v_uv).rgb; 

    // final color
    vec3 result = ambient + diffuse + specular;
    FragColor = vec4(result, 1.0);

    // Debug: render normal value as color
    // vec3 norm_colors = (v_normal + 1) / 2;
    // FragColor = vec4(norm_colors, 1.0);
} 


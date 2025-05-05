#version 400
out vec4 FragColor;

in VERTEX_OUT
{
	vec3 frag_pos;
	vec3 normal;
	vec2 texture_coordinate;
	vec4 frag_pos_light_space;
} f_in;

uniform sampler2D texture_diffuse1;
uniform sampler2D shadow_map;

uniform vec3 light_pos;
uniform vec3 view_pos;


// Function Pre-Definitions.
float CalculateShadowStrength(vec4 frag_pos_light_space);

void main()
{
	vec3 base_color = texture(texture_diffuse1, f_in.texture_coordinate).rgb;
    vec3 normal = normalize(f_in.normal);
    vec3 light_color = vec3(1.0);
    
    // Calculate Ambient Color
    vec3 ambient = 0.15f * light_color;
    
    // Calculate Diffuse Color
    vec3 light_dir = normalize(light_pos - f_in.frag_pos);
    float diffuse_strength = max(dot(light_dir, normal), 0.0);
    vec3 diffuse = diffuse_strength * light_color;
    
    // Calculate Specular Color
    vec3 view_dir = normalize(view_pos - f_in.frag_pos);
    vec3 halfway_dir = normalize(light_dir + view_dir);
    float specular_strength = pow(max(dot(normal, halfway_dir), 0.0), 64.0);
    vec3 specular = specular_strength * halfway_dir;

    // Calculate Shadow Strength
    float shadow_strength = CalculateShadowStrength(f_in.frag_pos_light_space);

    // Calculate and output our lighting.
    vec3 lighting = (ambient + (1.0 - shadow_strength) * (diffuse + specular)) * base_color;
    FragColor = vec4(lighting, 1.0);
}


float CalculateShadowStrength (vec4 frag_pos_light_space)
{
    // Perform Perspective Divide.
    vec3 projected_coordinates = frag_pos_light_space.xyz / frag_pos_light_space.w;
    projected_coordinates = (projected_coordinates * 0.5f) + 0.5f; // Convert to the range [0, 1].

    if (projected_coordinates.z > 1.0f)
    {
        return 0.0f;
    }

    float closest_depth = texture(shadow_map, projected_coordinates.xy).r;
    float current_depth = projected_coordinates.z;

    // Calculate and return our shadow strength.
    float shadow_strength = current_depth > closest_depth ? 1.0f : 0.0f;
    return shadow_strength;
}
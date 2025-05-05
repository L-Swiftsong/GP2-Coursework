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
float CalculateShadowStrength(vec4 frag_pos_light_space, vec3 normal, vec3 light_dir);

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
    float shadow_strength = CalculateShadowStrength(f_in.frag_pos_light_space, normal, light_dir);

    // Calculate and output our lighting.
    vec3 lighting = (ambient + (1.0 - shadow_strength) * (diffuse + specular)) * base_color;
    FragColor = vec4(lighting, 1.0);
}


float CalculateShadowStrength (vec4 frag_pos_light_space, vec3 normal, vec3 light_dir)
{
    // Perform Perspective Divide.
    vec3 projected_coordinates = frag_pos_light_space.xyz / frag_pos_light_space.w;
    projected_coordinates = (projected_coordinates * 0.5f) + 0.5f; // Convert to the range [0, 1].

    // Prevent shadows appearing if we are outside the shadow's far view plane.
    if (projected_coordinates.z > 1.0f)
    {
        return 0.0f;
    }


    float closest_depth = texture(shadow_map, projected_coordinates.xy).r;
    float current_depth = projected_coordinates.z;

    // Calculate our bias to remove shadowing artifacts (Shadow Acne).
    float min_bias = 0.00001f;

    // Calculate our shadow strength, using 'Percentage-Closer Filtering' to make our shadows less jagged.
    float shadow_strength = 0.0f;
    vec2 texel_size = 1.0f / textureSize(shadow_map, 0);
    for(int x = -1; x <= 1; ++x)
    {
        for (int y = -1; y <= 1; ++y)
        {
            float pfc_depth = texture(shadow_map, projected_coordinates.xy + vec2(x, y) * texel_size).r;
            shadow_strength += (current_depth - min_bias) > pfc_depth ? 1.0f : 0.0f;
        }
    }
    shadow_strength /= 9.0f;

    // Return our calculated shadow strength.
    return shadow_strength;
}
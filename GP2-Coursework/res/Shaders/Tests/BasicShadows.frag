#version 400
out vec4 FragColor;

// Light Structs.
struct DirectionalLight
{
	vec3 Direction;

	vec3 Diffuse;
};
struct PointLight
{
	vec3 Position;

	vec3 Diffuse;

    float Radius;
    float MaxIntensity;
    float Falloff;
};


// Input.
in VERTEX_OUT
{
	vec3 frag_pos;
	vec3 normal;
	vec2 texture_coordinate;
	vec4 frag_pos_light_space;
} f_in;


uniform sampler2D texture_diffuse1;
uniform sampler2D directional_shadow_map;
uniform samplerCube point_shadow_map;

uniform DirectionalLight directional_lights;
uniform PointLight point_lights;

uniform vec3 view_pos;
uniform float far_plane;


// Function Pre-Definitions.
float CalculateShadowStrength_PointLight(PointLight light, vec3 frag_pos);
float CalculateShadowStrength_DirectionalLight(DirectionalLight light, vec4 frag_pos_light_space);

void main()
{
	vec3 base_color = texture(texture_diffuse1, f_in.texture_coordinate).rgb;
    vec3 normal = normalize(f_in.normal);
    vec3 light_color = vec3(1.0);
    
    // Calculate Ambient Color
    vec3 ambient = 0.15f * light_color;
    
    // Calculate Diffuse Color
    vec3 light_dir = normalize(point_lights.Position - f_in.frag_pos);
    float diffuse_strength = max(dot(light_dir, normal), 0.0);
    vec3 diffuse = diffuse_strength * light_color;
    
    // Calculate Specular Color
    vec3 view_dir = normalize(view_pos - f_in.frag_pos);
    vec3 halfway_dir = normalize(light_dir + view_dir);
    float specular_strength = pow(max(dot(normal, halfway_dir), 0.0), 64.0);
    vec3 specular = specular_strength * halfway_dir;

    // Calculate Shadow Strength.
    // Note: This will incorrectly calculate the shadow strength where the two light's shadows don't overlap (Lights won't illuminate other lights shadows).
    float shadow_strength = 0.0f;
    //for(int i = 0; i < __; ++i)
        shadow_strength += (CalculateShadowStrength_DirectionalLight(directional_lights, f_in.frag_pos_light_space));
    //for(int i = 0; i < __; ++i)
        shadow_strength += (CalculateShadowStrength_PointLight(point_lights, f_in.frag_pos));


    // Calculate and output our lighting.
    vec3 lighting = (ambient + (1.0f - shadow_strength) * (diffuse + specular)) * base_color;
    FragColor = vec4(lighting, 1.0f);
}


// array of offset direction for sampling
vec3 grid_sampling_disk[20] = vec3[]
(
   vec3(1, 1,  1), vec3( 1, -1,  1), vec3(-1, -1,  1), vec3(-1, 1,  1), 
   vec3(1, 1, -1), vec3( 1, -1, -1), vec3(-1, -1, -1), vec3(-1, 1, -1),
   vec3(1, 1,  0), vec3( 1, -1,  0), vec3(-1, -1,  0), vec3(-1, 1,  0),
   vec3(1, 0,  1), vec3(-1,  0,  1), vec3( 1,  0, -1), vec3(-1, 0, -1),
   vec3(0, 1,  1), vec3( 0, -1,  1), vec3( 0, -1, -1), vec3( 0, 1, -1)
);
float CalculateShadowStrength_PointLight(PointLight light, vec3 frag_pos)
{
    vec3 frag_to_light = frag_pos - light.Position;

    float current_depth = length(frag_to_light);

    float shadow_strength = 0.0f;
    float bias = 0.0001f;
    int samples = 20;
    float view_distance = length(view_pos - frag_pos);
    float disk_radius = (1.0 + (view_distance / far_plane)) / 50.0f;
    for(int i = 0; i < samples; ++i)
    {
        float closest_depth = texture(point_shadow_map, frag_to_light + grid_sampling_disk[i] * disk_radius).r;
        closest_depth *= far_plane;   // Undo our 0 to 1 range mapping.
        if(current_depth - bias > closest_depth)
            shadow_strength += 1.0f;
    }
    shadow_strength /= float(samples);
        
    // display closestDepth as debug (to visualize depth cubemap)
    //FragColor = vec4(vec3(1.0f - shadow_strength), 1.0f);

    return shadow_strength;
}
float CalculateShadowStrength_DirectionalLight(DirectionalLight light, vec4 frag_pos_light_space)
{
    // Perform Perspective Divide.
    vec3 projected_coordinates = frag_pos_light_space.xyz / frag_pos_light_space.w;
    projected_coordinates = (projected_coordinates * 0.5f) + 0.5f; // Convert to the range [0, 1].

    // Prevent shadows appearing if we are outside the shadow's far view plane.
    if (projected_coordinates.z > 1.0f)
    {
        return 0.0f;
    }


    float closest_depth = texture(directional_shadow_map, projected_coordinates.xy).r;
    float current_depth = projected_coordinates.z;

    // Calculate our bias to remove shadowing artifacts (Shadow Acne).
    float min_bias = 0.00001f;

    // Calculate our shadow strength, using 'Percentage-Closer Filtering' to make our shadows less jagged.
    float shadow_strength = 0.0f;
    vec2 texel_size = 1.0f / textureSize(directional_shadow_map, 0);
    for(int x = -1; x <= 1; ++x)
    {
        for (int y = -1; y <= 1; ++y)
        {
            float pfc_depth = texture(directional_shadow_map, projected_coordinates.xy + vec2(x, y) * texel_size).r;
            shadow_strength += (current_depth - min_bias) > pfc_depth ? 1.0f : 0.0f;
        }
    }
    shadow_strength /= 9.0f;

    // Return our calculated shadow strength.
    return shadow_strength;
}
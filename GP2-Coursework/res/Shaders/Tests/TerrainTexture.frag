#version 400
out vec4 FragColor;

// Lighting Structs.
struct DirectionalLight
{
	vec3 Direction;

	vec3 Diffuse;
    
	mat4 light_space_matrix;
    sampler2D shadow_map;
};
struct PointLight
{
	vec3 Position;

	vec3 Diffuse;

    float Radius;
    float MaxIntensity;
    float Falloff;

    samplerCube shadow_map;
};


// Input.
in VERTEX_OUT
{
	vec3 frag_pos;
	vec2 texture_coordinates;
    
	mat3 TBN;
	vec3 tangent_view_pos;
	vec3 tangent_frag_pos;
} f_in;


// Grass.
uniform sampler2D texture_diffuse1;
uniform sampler2D texture_normal1;
uniform float grass_texture_scale = 200.0f;

// Dirt.
uniform sampler2D texture_diffuse2;
uniform sampler2D texture_normal2;
uniform float dirt_texture_scale = 200.0f;

// Rock.
uniform sampler2D texture_diffuse3;
uniform sampler2D texture_normal3;
uniform float rock_texture_scale = 100.0f;

// Blend.
uniform sampler2D texture_diffuse4;


// Lighting.
uniform DirectionalLight[1] directional_lights;
uniform PointLight[2] point_lights;

uniform vec3 view_pos;
uniform float far_plane;
uniform vec3 ambient_color = vec3(0.35f, 0.35f, 0.87f);


vec3 base_color;
vec3 normal;


// Function Pre-definitions.
vec3 CalculateAverageColor(vec3 material_strengths);
vec3 CalculateAverageNormal(vec3 material_strengths);
vec3 CalculateMaterialStrengths();


float CalculateShadowStrength_PointLight(PointLight light, vec3 frag_pos);
float CalculateShadowStrength_DirectionalLight(DirectionalLight light);

vec3 CalculateDirectionalLighting(DirectionalLight light);
vec3 CalculatePointLighting(PointLight light);


void main() 
{
	// Get the strength of each of our materials.
	vec3 material_strengths = CalculateMaterialStrengths();

	// Calculate our Base Colour & Normal using our material strengths values.
	base_color = CalculateAverageColor(material_strengths);
	normal = CalculateAverageNormal(material_strengths);


    // Calculate Ambient Color here so that it isn't applied once for each light.
    vec3 ambient = 0.15f * ambient_color;

    // Calculate and output our lighting.
    vec3 lighting = ambient * base_color;
    for(int i = 0; i < directional_lights.length(); ++i)
        lighting += CalculateDirectionalLighting(directional_lights[i]);
    for(int i = 0; i < point_lights.length(); ++i)
        lighting += CalculatePointLighting(point_lights[i]);

	FragColor = vec4(lighting, 1.0f);
}


vec3 CalculateAverageColor(vec3 material_strengths)
{
	// Calculate the diffuse colour for each material.
	vec3 grass_color = texture(texture_diffuse1, f_in.texture_coordinates * grass_texture_scale).rgb;
	vec3 dirt_color = texture(texture_diffuse2, f_in.texture_coordinates * dirt_texture_scale).rgb;
	vec3 rock_color = texture(texture_diffuse2, f_in.texture_coordinates * rock_texture_scale).rgb;

	// Determine the colour strengths based on our material_strengths value.
	return mix(mix(grass_color, dirt_color, material_strengths.g), rock_color, material_strengths.b);
}
vec3 CalculateAverageNormal(vec3 material_strengths)
{
	// Calculate our normals for each material.
	vec3 grass_normal = texture(texture_normal1, f_in.texture_coordinates * grass_texture_scale).rgb;
	grass_normal = normalize((grass_normal * 2.0f) - 1.0f);
	vec3 dirt_normal = texture(texture_normal2, f_in.texture_coordinates * dirt_texture_scale).rgb;
	dirt_normal = normalize((dirt_normal * 2.0f) - 1.0f);
	vec3 rock_normal = texture(texture_normal3, f_in.texture_coordinates * rock_texture_scale).rgb;
	rock_normal = normalize((rock_normal * 2.0f) - 1.0f);

	// Maybe?
	return mix(mix(grass_normal, dirt_normal, material_strengths.g), rock_normal, material_strengths.b);
}

vec3 CalculateMaterialStrengths()
{
	return texture(texture_diffuse4, f_in.texture_coordinates).rgb;
}


// ----- Shadows -----
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
        float closest_depth = texture(light.shadow_map, frag_to_light + grid_sampling_disk[i] * disk_radius).r;
        closest_depth *= far_plane;   // Undo our 0 to 1 range mapping.
        if(current_depth - bias > closest_depth)
            shadow_strength += 1.0f;
    }
    shadow_strength /= float(samples);
        
    // display closestDepth as debug (to visualize depth cubemap)
    //FragColor = vec4(vec3(1.0f - shadow_strength), 1.0f);

    return shadow_strength;
}
float CalculateShadowStrength_DirectionalLight(DirectionalLight light)
{
    // Calculate our fragment's position in the light's space.
    vec4 frag_pos_light_space = light.light_space_matrix * vec4(f_in.frag_pos, 1.0f);

    // Perform Perspective Divide.
    vec3 projected_coordinates = frag_pos_light_space.xyz / frag_pos_light_space.w;
    projected_coordinates = (projected_coordinates * 0.5f) + 0.5f; // Convert to the range [0, 1].

    // Prevent shadows appearing if we are outside the shadow's far view plane.
    if (projected_coordinates.z > 1.0f)
    {
        return 0.0f;
    }


    float closest_depth = texture(light.shadow_map, projected_coordinates.xy).r;
    float current_depth = projected_coordinates.z;

    // Calculate our bias to remove shadowing artifacts (Shadow Acne).
    float min_bias = 0.00001f;

    // Calculate our shadow strength, using 'Percentage-Closer Filtering' to make our shadows less jagged.
    float shadow_strength = 0.0f;
    vec2 texel_size = 1.0f / textureSize(light.shadow_map, 0);
    for(int x = -1; x <= 1; ++x)
    {
        for (int y = -1; y <= 1; ++y)
        {
            float pfc_depth = texture(light.shadow_map, projected_coordinates.xy + vec2(x, y) * texel_size).r;
            shadow_strength += (current_depth - min_bias) > pfc_depth ? 1.0f : 0.0f;
        }
    }
    shadow_strength /= 9.0f;

    // Return our calculated shadow strength.
    return shadow_strength;
}


// ----- Lighting -----.
vec3 CalculateDirectionalLighting(DirectionalLight light)
{
    // Calculate Diffuse Color.
    vec3 light_dir = normalize(f_in.TBN * -light.Direction);
    float diffuse_strength = max(dot(light_dir, normal), 0.0);
    vec3 diffuse = diffuse_strength * light.Diffuse;
    
    // Calculate Specular Color.
    vec3 view_dir = normalize(f_in.tangent_view_pos - f_in.tangent_frag_pos);
    vec3 halfway_dir = normalize(light_dir + view_dir);
    float specular_strength = pow(max(dot(normal, halfway_dir), 0.0), 64.0);
    vec3 specular = specular_strength * light.Diffuse;

    // Calculate our Shadow Strength for this light.
    float shadow_strength = CalculateShadowStrength_DirectionalLight(light);

    // Calculate and output this light's output.
    vec3 lighting_output = ((1.0f - shadow_strength) * (diffuse + specular)) * base_color;
    return lighting_output;
}
vec3 CalculatePointLighting(PointLight light)
{
    // Calculate Diffuse Color.
    vec3 light_dir = normalize((f_in.TBN * light.Position) - f_in.tangent_frag_pos);
    float diffuse_strength = max(dot(light_dir, normal), 0.0);
    vec3 diffuse = diffuse_strength * light.Diffuse;
    
    // Calculate Specular Color.
    vec3 view_dir = normalize(f_in.tangent_view_pos - f_in.tangent_frag_pos);
    vec3 halfway_dir = normalize(light_dir + view_dir);
    float specular_strength = pow(max(dot(normal, halfway_dir), 0.0), 64.0);
    vec3 specular = specular_strength * light.Diffuse;

    // Calculate our Shadow Strength for this light.
    float shadow_strength = CalculateShadowStrength_PointLight(light, f_in.frag_pos);

    // Calculate and output this light's output.
    vec3 lighting_output = ((1.0f - shadow_strength) * (diffuse + specular)) * base_color;
    return lighting_output;
}
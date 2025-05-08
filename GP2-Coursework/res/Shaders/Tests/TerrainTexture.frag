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
uniform PointLight[3] point_lights;

uniform vec3 view_pos;
uniform float far_plane;
uniform vec3 ambient_color = vec3(0.35f, 0.35f, 0.87f);


vec3 albedo;
float metallic;
float roughness;
vec3 zero_angle_surface_reflection;

vec3 normal;
vec3 view_dir;


const float PI = 3.14159265359;


// Function Pre-definitions.
//--- Averaging Materials ---
vec3 CalculateAverageColor(vec3 material_strengths);
vec3 CalculateAverageNormal(vec3 material_strengths);
vec3 CalculateMaterialStrengths();


//--- Lighting ---
vec3 CalculateDirectionalLighting(DirectionalLight light);

float CalcPointAttenuation(PointLight light, vec3 light_position);
vec3 CalculatePointLighting(PointLight light);


vec3 CalculatePBRLighting(vec3 light_dir, vec3 H, vec3 radiance, float shadow_strength);

float DistributionGGX(vec3 normal, vec3 H, float roughness);
float GeometrySchlickGGX(float normalDotView, float roughness);
float GeometrySmith(vec3 L, float roughness);
vec3 FresnelSchlick(float cos_theta, vec3 zero_angle_surface_reflection);

float clamp01(float value) { return clamp(value, 0.0, 1.0); }
float sqr(float x) { return x * x; }


//--- Shadows ---
float CalculateShadowStrength_PointLight(PointLight light);
float CalculateShadowStrength_DirectionalLight(DirectionalLight light);



void main()
{
	// Get the strength of each of our materials.
	vec3 material_strengths = CalculateMaterialStrengths();

	// Calculate our Albedo using our material strengths values.
	albedo = CalculateAverageColor(material_strengths);
    metallic = 0.0f;
    roughness = 0.85f;
    zero_angle_surface_reflection = mix(vec3(0.04), albedo, metallic);

    // Calculate our average surface normal.
	normal = CalculateAverageNormal(material_strengths);

    // Calculate our view direction.
    view_dir = normalize(f_in.tangent_view_pos - f_in.tangent_frag_pos);


    // Calculate Ambient Color here so that it isn't applied once for each light.
    vec3 ambient = 0.15f * ambient_color;

    // Calculate and output our lighting.
    vec3 lighting = ambient * albedo;
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


// ----- PBR Lighting -----
vec3 CalculateDirectionalLighting(DirectionalLight light)
{
    // Calculate per-light radiance.
    vec3 light_dir = normalize(f_in.TBN * -light.Direction);
	vec3 H = normalize(view_dir + light_dir);
    vec3 radiance = light.Diffuse;

    // Calculate our shadow strength.
    float shadow_strength = CalculateShadowStrength_DirectionalLight(light);


    // Calculate and return our PBR Lighting Value.
    return CalculatePBRLighting(light_dir, H, radiance, shadow_strength);
}
vec3 CalculatePointLighting(PointLight light)
{
    vec3 light_pos = f_in.TBN * light.Position;

    // Calculate per-light radiance.
	vec3 light_dir = normalize(light_pos - f_in.tangent_frag_pos);
	vec3 H = normalize(view_dir + light_dir);

    // Determine our radiance.
    float attenuation = CalcPointAttenuation(light, light_pos);
    vec3 radiance = light.Diffuse * attenuation;


    // Calculate our shadow strength.
    float shadow_strength = CalculateShadowStrength_PointLight(light);


    // Calculate and return our PBR Lighting Value.
    return CalculatePBRLighting(light_dir, H, radiance, shadow_strength);
}


// Calculate Distance Attenuation: 'https://lisyarus.github.io/blog/posts/point-light-attenuation.html'.
float CalcPointAttenuation(PointLight light, vec3 light_position)
{
    float distance = length(light_position - f_in.tangent_frag_pos);
    float normalised_distance = distance / light.Radius;

    if (normalised_distance >= 1.0)
    {
        // Outwith max range.
        return 0.0;
    }

    float sqr_distance = sqr(normalised_distance);
    return light.MaxIntensity * sqr(1.0 - sqr_distance) / (1.0 + light.Falloff * normalised_distance);
}


// ----------------------------------------------------------------------------
vec3 CalculatePBRLighting(vec3 light_dir, vec3 H, vec3 radiance, float shadow_strength)
{
    // Cook-torrance brdf.
    float normal_distribution = DistributionGGX(normal, H, roughness);
    float geometry = GeometrySmith(light_dir, roughness);
    vec3 fresnel = FresnelSchlick(max(dot(H, view_dir), 0.0), zero_angle_surface_reflection);

    vec3 numerator = normal_distribution * geometry * fresnel;
    float denominator = 4.0 * max(dot(normal, view_dir), 0.0) * max(dot(normal, light_dir), 0.0) + 0.0001; // + 0.0001 to prevent divide by zero error.
    vec3 specular = numerator/denominator;


    // Specular contribution is equal to the Fresnel.
    vec3 specular_contribution = fresnel;
    // For energy conservation, the diffuse and specular light can't
    //  be above 1.0 (unless the surface emits light); to preserve this
    //  relationship the diffuse component (kD) should equal 1.0 - specular_contribution.
    vec3 diffuse_contribution = vec3(1.0) - specular_contribution;
    // Multiply kD by the inverse metalness such that only non-metals 
    //  have diffuse lighting, or a linear blend if partly metal (pure metals
    //  have no diffuse light).
    diffuse_contribution *= 1.0 - metallic;

    // Scale light by the dot between the Normal and Light Direction.
    float normal_dot_light_dir = max(dot(normal, light_dir), 0.0);

    // Calculate our non-shadow output colour.
    vec3 output_color = (diffuse_contribution * albedo / PI + specular) * radiance * normal_dot_light_dir;

    // Apply shadows.
    output_color *= (1.0f - shadow_strength);

    // Return our output colour.
    return output_color;
}


// ----------------------------------------------------------------------------
float DistributionGGX(vec3 normal, vec3 H, float roughness)
{
    float a = roughness * roughness;
    float a2 = a * a;
    float normal_dot_H = max(dot(normal, H), 0.0);
    float normal_dot_H_sqr = normal_dot_H * normal_dot_H;

    float nom   = a2;
    float denom = (normal_dot_H_sqr * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return nom / denom;
}
// ----------------------------------------------------------------------------
float GeometrySchlickGGX(float normal_dot_view, float roughness)
{
    float r = (roughness + 1.0f);
    float k = (r * r) / 8.0f;

    float nom   = normal_dot_view;
    float denom = normal_dot_view * (1.0f - k) + k;

    return nom / denom;
}
// ----------------------------------------------------------------------------
float GeometrySmith(vec3 light_dir, float roughness)
{
    float normal_dot_light_dir = max(dot(normal, light_dir), 0.0f);
    float ggx1 = GeometrySchlickGGX(normal_dot_light_dir, roughness);

    float normal_dot_view = max(dot(normal, view_dir), 0.0f);
    float ggx2 = GeometrySchlickGGX(normal_dot_view, roughness);

    return ggx1 * ggx2;
}
// ----------------------------------------------------------------------------
vec3 FresnelSchlick(float cos_theta, vec3 zero_angle_surface_reflection)
{
    return zero_angle_surface_reflection + (1.0 - zero_angle_surface_reflection) * pow(clamp(1.0 - cos_theta, 0.0, 1.0), 5.0);
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
float CalculateShadowStrength_PointLight(PointLight light)
{
    vec3 frag_to_light = f_in.frag_pos - light.Position;

    float current_depth = length(frag_to_light);

    float shadow_strength = 0.0f;
    float bias = 0.0001f;
    int samples = 20;
    float view_distance = length(view_pos - f_in.frag_pos);
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
    const int kShadowBlurStrength = 3;
    float shadow_strength = 0.0f;
    vec2 texel_size = 1.0f / textureSize(light.shadow_map, 0);
    for(int x = -kShadowBlurStrength; x <= kShadowBlurStrength; ++x)
    {
        for (int y = -kShadowBlurStrength; y <= kShadowBlurStrength; ++y)
        {
            float pfc_depth = texture(light.shadow_map, projected_coordinates.xy + vec2(x, y) * texel_size).r;
            shadow_strength += (current_depth - min_bias) > pfc_depth ? 1.0f : 0.0f;
        }
    }
    shadow_strength /= ((kShadowBlurStrength * 2 + 1) * (kShadowBlurStrength * 2 + 1));

    // Return our calculated shadow strength.
    return shadow_strength;
}
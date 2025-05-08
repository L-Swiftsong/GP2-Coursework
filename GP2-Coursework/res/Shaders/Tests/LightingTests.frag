#version 400
out vec4 FragColor;

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

in VERTEX_OUT
{
    vec3 frag_pos;
    vec2 texture_coordinate;
	vec3 vertex_normal;

	mat3 TBN;
} f_in;

uniform DirectionalLight[1] directional_lights;
uniform PointLight[3] point_lights;

uniform vec3 camera_pos;
uniform float far_plane;
uniform vec3 ambient_color = vec3(0.35f, 0.35f, 0.87f);


// PBR Textures.
uniform sampler2D texture_diffuse1;

uniform sampler2D texture_metallic1;
uniform bool has_metallic;

uniform sampler2D texture_roughness1;
uniform bool has_roughness;

uniform sampler2D texture_normal1;
uniform bool has_normal;


vec3 albedo;
float metallic;
float roughness;
vec3 zero_angle_surface_reflection;

vec3 calculated_view_pos;
vec3 calculated_frag_pos;
vec3 calculated_normal;


const float PI = 3.14159265359;


// Function Declarations.
vec3 CalculateDirectionalLighting(DirectionalLight light, vec3 normal, vec3 viewDir);

float CalcPointAttenuation(PointLight light, vec3 light_position);
vec3 CalculatePointLighting(PointLight light, vec3 normal, vec3 viewDir);


vec3 CalculatePBRLighting(vec3 light_dir, vec3 H, vec3 radiance, vec3 normal, vec3 view_dir, float shadow_strength);

float DistributionGGX(vec3 normal, vec3 H, float roughness);
float GeometrySchlickGGX(float normalDotView, float roughness);
float GeometrySmith(vec3 normal, vec3 view, vec3 L, float roughness);
vec3 FresnelSchlick(float cos_theta, vec3 zero_angle_surface_reflection);

float clamp01(float value) { return clamp(value, 0.0, 1.0); }
float sqr(float x) { return x * x; }


float CalculateShadowStrength_PointLight(PointLight light);
float CalculateShadowStrength_DirectionalLight(DirectionalLight light);



void main() 
{
    // Determine unlit colour/metallic/roughness values.
    albedo = pow(texture(texture_diffuse1, f_in.texture_coordinate).rgb, vec3(2.2));
    metallic = has_metallic ? texture(texture_metallic1, f_in.texture_coordinate).r : 0.0f;
    roughness = has_roughness ? texture(texture_roughness1, f_in.texture_coordinate).r : 1.0f;

    zero_angle_surface_reflection = mix(vec3(0.04), albedo, metallic);


    // Determine normal using normal map.
    if (!has_normal)
    {
        // We don't have an assigned normal map.
        calculated_normal = f_in.vertex_normal;
    }
    else
    {
        // We have an assigned normal map.
	    calculated_normal = texture(texture_normal1, f_in.texture_coordinate).rgb;
    	calculated_normal = normalize((calculated_normal * 2.0f) - 1.0f);
    }


    // Calculate our view direction.
    calculated_view_pos = has_normal ? (f_in.TBN * camera_pos) : camera_pos;
    calculated_frag_pos = has_normal ? (f_in.TBN * f_in.frag_pos) : f_in.frag_pos;
    vec3 calculated_view_dir = normalize(calculated_view_pos - calculated_frag_pos);


    // Calculate Ambient Lighting (Temp).
    vec3 ambient = vec3(0.0f);


	vec3 lighting = ambient * albedo;
    for(int i = 0; i < directional_lights.length(); ++i)
    {
        lighting += CalculateDirectionalLighting(directional_lights[i], calculated_normal, calculated_view_dir);
    }
    for(int i = 0; i < point_lights.length(); ++i)
    {
        lighting += CalculatePointLighting(point_lights[i], calculated_normal, calculated_view_dir);
    }

    vec3 color = lighting;
    color = color / (color + vec3(1.0));    // HDR Tonemapping.
    color = pow(color, vec3(1.0/2.2));      // Gamma Correction.

    FragColor = vec4(color, 1.0f);
}


// Function Definitions.
vec3 CalculateDirectionalLighting(DirectionalLight light, vec3 normal, vec3 view_dir)
{
    // Calculate per-light radiance.
    vec3 light_dir = normalize(has_normal ? (f_in.TBN * -light.Direction) : -light.Direction);
	vec3 H = normalize(view_dir + light_dir);
    vec3 radiance = light.Diffuse;

    // Calculate our shadow strength.
    float shadow_strength = CalculateShadowStrength_DirectionalLight(light);


    // Calculate and return our PBR Lighting Value.
    return CalculatePBRLighting(light_dir, H, radiance, normal, view_dir, shadow_strength);
}
vec3 CalculatePointLighting(PointLight light, vec3 normal, vec3 view_dir)
{
    vec3 light_pos = has_normal ? (f_in.TBN * light.Position) : light.Position;

    // Calculate per-light radiance.
	vec3 light_dir = normalize(light_pos - calculated_frag_pos);
	vec3 H = normalize(view_dir + light_dir);

    // Determine our radiance.
    float attenuation = CalcPointAttenuation(light, light_pos);
    vec3 radiance = light.Diffuse * attenuation;


    // Calculate our shadow strength.
    float shadow_strength = CalculateShadowStrength_PointLight(light);


    // Calculate and return our PBR Lighting Value.
    return CalculatePBRLighting(light_dir, H, radiance, normal, view_dir, shadow_strength);
}


// Calculate Distance Attenuation: 'https://lisyarus.github.io/blog/posts/point-light-attenuation.html'.
float CalcPointAttenuation(PointLight light, vec3 light_position)
{
    float distance = length(light_position - calculated_frag_pos);
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
vec3 CalculatePBRLighting(vec3 light_dir, vec3 H, vec3 radiance, vec3 normal, vec3 view_dir, float shadow_strength)
{
    // Cook-torrance brdf.
    float normal_distribution = DistributionGGX(normal, H, roughness);
    float geometry = GeometrySmith(normal, view_dir, light_dir, roughness);
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
float GeometrySmith(vec3 normal, vec3 view_dir, vec3 light_dir, float roughness)
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
// Array of offset directions for sampling.
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
    float view_distance = length(camera_pos - f_in.frag_pos);
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
#version 400
out vec4 FragColor;

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

in VERTEX_OUT
{
    vec3 frag_pos;
    vec2 texture_coordinate;
	vec3 vertex_normal;

	mat3 TBN;
} f_in;

uniform vec3 cameraPos;
uniform DirectionalLight directionalLight;
uniform PointLight pointLight;

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_specular1;
uniform sampler2D texture_metallic1;
uniform sampler2D texture_roughness1;
uniform sampler2D texture_normal1;
uniform sampler2D texture_displacement1;

uniform bool has_normal;


vec3 albedo;
float metallic;
float roughness;
vec3 zero_angle_surface_reflection;

vec3 view_pos;
vec3 frag_pos;


const float PI = 3.14159265359;


// Function Declarations.
vec3 CalcDirectionalLight(DirectionalLight light, vec3 normal, vec3 viewDir);

float CalcPointAttenuation(PointLight light, vec3 light_position);
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 viewDir);


vec3 CalculatePBRLighting(vec3 light_dir, vec3 H, vec3 radiance, vec3 normal, vec3 view_dir);

float DistributionGGX(vec3 normal, vec3 H, float roughness);
float GeometrySchlickGGX(float normalDotView, float roughness);
float GeometrySmith(vec3 normal, vec3 view, vec3 L, float roughness);
vec3 FresnelSchlick(float cos_theta, vec3 zero_angle_surface_reflection);

float clamp01(float value) { return clamp(value, 0.0, 1.0); }
float sqr(float x) { return x * x; }


void main() 
{
    // Determine unlit colour/metallic/roughness values.
    albedo = pow(texture(texture_diffuse1, f_in.texture_coordinate).rgb, vec3(2.2));
    metallic = texture(texture_metallic1, f_in.texture_coordinate).r;
    roughness = texture(texture_roughness1, f_in.texture_coordinate).r;

    zero_angle_surface_reflection = mix(vec3(0.04), albedo, metallic);
    

    // Determine normal using normal map.
    vec3 normal;
    if (!has_normal)
    {
        // We don't have an assigned normal map.
        normal = f_in.vertex_normal;
        //FragColor = vec4(normal, 1.0f);
        //return;
    }
    else
    {
        // We have an assigned normal map.
	    normal = texture(texture_normal1, f_in.texture_coordinate).rgb;
    	normal = normalize((normal * 2.0f) - 1.0f);
    }


    // Calculate our view direction.
    view_pos = has_normal ? f_in.TBN * cameraPos : cameraPos;
    frag_pos = has_normal ? f_in.TBN * f_in.frag_pos : f_in.frag_pos;
    vec3 view_dir = normalize(view_pos - frag_pos);


	vec3 lighting_output = CalcDirectionalLight(directionalLight, normal, view_dir);
    lighting_output += CalcPointLight(pointLight, normal, view_dir);

    // Calculate Ambient Lighting (Temp).
    vec3 ambient = vec3(0.03) * albedo;

    vec3 color = lighting_output;
    color = color / (color + vec3(1.0));    // HDR Tonemapping.
    color = pow(color, vec3(1.0/2.2));      // Gamma Correction.

    FragColor = vec4(color, 1.0f);
}


// Function Definitions.
vec3 CalcDirectionalLight(DirectionalLight light, vec3 normal, vec3 view_dir)
{
	// Calculate per-light radiance.
    vec3 light_dir = normalize(has_normal ? (f_in.TBN * -light.Direction) : -light.Direction);
	vec3 H = normalize(view_dir + light_dir);
    vec3 radiance = light.Diffuse;

    // Calculate and return our PBR Lighting Value.
    return CalculatePBRLighting(light_dir, H, radiance, normal, view_dir);
}
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 view_dir)
{
    vec3 light_pos = has_normal ? (f_in.TBN * light.Position) : light.Position;

    // Calculate per-light radiance.
	vec3 light_dir = normalize(light_pos - frag_pos);
	vec3 H = normalize(view_dir + light_dir);

    // Determine our radiance.
    float attenuation = CalcPointAttenuation(light, light_pos);
    vec3 radiance = light.Diffuse * attenuation;

    // Calculate and return our PBR Lighting Value.
    return CalculatePBRLighting(light_dir, H, radiance, normal, view_dir);
}


// Calculate Distance Attenuation: 'https://lisyarus.github.io/blog/posts/point-light-attenuation.html'.
float CalcPointAttenuation(PointLight light, vec3 light_position)
{
    float distance = length(light_position - 
    frag_pos);
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
vec3 CalculatePBRLighting(vec3 light_dir, vec3 H, vec3 radiance, vec3 normal, vec3 view_dir)
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

    // Add to ourgoing radiance LO.
    return (diffuse_contribution * albedo / PI + specular) * radiance * normal_dot_light_dir;
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
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;

    float nom   = normal_dot_view;
    float denom = normal_dot_view * (1.0 - k) + k;

    return nom / denom;
}
// ----------------------------------------------------------------------------
float GeometrySmith(vec3 normal, vec3 view_dir, vec3 light_dir, float roughness)
{
    float normal_dot_light_dir = max(dot(normal, light_dir), 0.0);
    float ggx1 = GeometrySchlickGGX(normal_dot_light_dir, roughness);

    float normal_dot_view = max(dot(normal, view_dir), 0.0);
    float ggx2 = GeometrySchlickGGX(normal_dot_view, roughness);

    return ggx1 * ggx2;
}
// ----------------------------------------------------------------------------
vec3 FresnelSchlick(float cos_theta, vec3 zero_angle_surface_reflection)
{
    return zero_angle_surface_reflection + (1.0 - zero_angle_surface_reflection) * pow(clamp(1.0 - cos_theta, 0.0, 1.0), 5.0);
}
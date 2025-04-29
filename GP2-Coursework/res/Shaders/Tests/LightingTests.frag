#version 400

out vec4 FragColor;

struct DirectionalLight
{
	vec3 Direction;

	vec3 Ambient;
	vec3 Diffuse;
	vec3 Specular;
};
struct PointLight
{
	vec3 Position;

	vec3 Ambient;
	vec3 Diffuse;
	vec3 Specular;

    float Radius;
    float MaxIntensity;
    float Falloff;
};

in vec2 textureCoordinate;
in vec3 v_normal;
in vec3 v_pos;

uniform vec3 cameraPos;
uniform DirectionalLight directionalLight;
uniform PointLight pointLight;

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_specular1;
uniform sampler2D texture_metallic1;
uniform sampler2D texture_roughness1;
uniform sampler2D texture_normal1;
uniform sampler2D texture_displacement1;


vec3 albedo;
float metallic;
float roughness;
vec3 F0;


const float PI = 3.14159265359;


// Function Declarations.
vec3 CalcDirectionalLight(DirectionalLight light, vec3 normal, vec3 viewDir);

float CalcPointAttenuation(PointLight light);
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 viewDir);

float DistributionGGX(vec3 normal, vec3 height, float roughness);
float GeometrySchlickGGX(float normalDotView, float roughness);
float GeometrySmith(vec3 normal, vec3 view, vec3 L, float roughness);
vec3 FresnelSchlick(float cosTheta, vec3 F0);

float clamp01(float value) { return clamp(value, 0.0, 1.0); }
float sqr(float x) { return x * x; }


void main() 
{
    albedo = pow(texture(texture_diffuse1, textureCoordinate).rgb, vec3(2.2));
    metallic = texture(texture_metallic1, textureCoordinate).r;
    roughness = texture(texture_roughness1, textureCoordinate).r;

    F0 = mix(vec3(0.04), albedo, metallic);
    

    vec3 viewDir = normalize(cameraPos - v_pos.xyz);
	vec3 lightingResult = CalcDirectionalLight(directionalLight, v_normal, viewDir);
    lightingResult += CalcPointLight(pointLight, v_normal, viewDir);

    // Calculate Ambient Lighting (Temp).
    vec3 ambient = vec3(0.03) * albedo;

    vec3 color = ambient + lightingResult;
    color = color / (color + vec3(1.0));    // HDR Tonemapping.
    color = pow(color, vec3(1.0/2.2));      // Gamma Correction.

    FragColor = vec4(color, 1.0f);
}


// Function Definitions.
vec3 CalcDirectionalLight(DirectionalLight light, vec3 normal, vec3 viewDir)
{
	vec3 lightDir = normalize(-light.Direction);
    
    // Diffuse multiplier.
    float diffuseMult = max(dot(normal, lightDir), 0.0f);

    // Specular multiplier.
    vec3 reflectDir = reflect(-lightDir, normal);
    float specularMult = pow(max(dot(viewDir, reflectDir), 0.0f), 1.0f); //material.Shininess);

    // Calculate results.
    vec3 ambient  = light.Ambient * albedo;
    vec3 diffuse  = light.Diffuse * diffuseMult * albedo;
    vec3 specular = light.Specular * specularMult * metallic;

    // Combine results.
    return (ambient + diffuse + specular);
}
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 viewDir)
{
    // Calculate per-light radiance.
	vec3 lightDir = normalize(light.Position - v_pos);
	vec3 height = normalize(viewDir + lightDir);

    float attenuation = CalcPointAttenuation(light);
    vec3 radiance = light.Diffuse * attenuation;


    // Cook-torrance brdf.
    float NDF = DistributionGGX(normal, height, roughness);
    float G = GeometrySmith(normal, viewDir, lightDir, roughness);
    vec3 F = FresnelSchlick(max(dot(height, viewDir), 0.0), F0);

    vec3 numerator = NDF * G * F;
    float denominator = 4.0 * max(dot(normal, viewDir), 0.0) * max(dot(normal, lightDir), 0.0) + 0.0001; // + 0.0001 to prevent divide by zero error.
    vec3 specular = numerator/denominator;


    // kS is equal to Fresnel.
    vec3 kS = F;
    // For energy conservation, the diffuse and specular light can't
    //  be above 1.0 (unless the surface emits light); to preserve this
    //  relationship the diffuse component (kD) should equal 1.0 - kS.
    vec3 kD = vec3(1.0) - kS;
    // Multiply kD by the inverse metalness such that only non-metals 
    //  have diffuse lighting, or a linear blend if partly metal (pure metals
    //  have no diffuse light).
    kD *= 1.0 - metallic;

    // Scale light by the dot between the Normal and Light Direction.
    float NdotL = max(dot(normal, lightDir), 0.0);

    // Add to ourgoing radiance LO.
    return (kD * albedo / PI + specular) * radiance * NdotL;
}


// Calculate Distance Attenuation: 1.0 / (c + kd + sd^2).
/*float CalcPointAttenuation(PointLight light)
{
    float distance = length(light.Position - v_pos.xyz);
    return clamp01(1.0 / (light.ConstantAttenuation + light.LinearAttenuation * distance + light.QuadraticAttenuation * (distance * distance)));
}*/
// Calculate Distance Attenuation: 'https://lisyarus.github.io/blog/posts/point-light-attenuation.html'.
float CalcPointAttenuation(PointLight light)
{
    float distance = length(light.Position - v_pos);
    float normalisedDistance = distance / light.Radius;

    if (normalisedDistance >= 1.0)
    {
        // Outwith max range.
        return 0.0;
    }

    float sqrDistance = sqr(normalisedDistance);
    return light.MaxIntensity * sqr(1.0 - sqrDistance) / (1.0 + light.Falloff * normalisedDistance);
}


// ----------------------------------------------------------------------------
float DistributionGGX(vec3 normal, vec3 height, float roughness)
{
    float a = roughness * roughness;
    float a2 = a * a;
    float normalDotHeight = max(dot(normal, height), 0.0);
    float normalDotHeightSqr = normalDotHeight * normalDotHeight;

    float nom   = a2;
    float denom = (normalDotHeightSqr * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return nom / denom;
}
// ----------------------------------------------------------------------------
float GeometrySchlickGGX(float normalDotView, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;

    float nom   = normalDotView;
    float denom = normalDotView * (1.0 - k) + k;

    return nom / denom;
}
// ----------------------------------------------------------------------------
float GeometrySmith(vec3 normal, vec3 viewDir, vec3 L, float roughness)
{
    float normalDotView = max(dot(normal, viewDir), 0.0);
    float normalDotL = max(dot(normal, L), 0.0);
    float ggx2 = GeometrySchlickGGX(normalDotView, roughness);
    float ggx1 = GeometrySchlickGGX(normalDotL, roughness);

    return ggx1 * ggx2;
}
// ----------------------------------------------------------------------------
vec3 FresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}
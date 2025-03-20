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
struct Material {
    sampler2D Diffuse;
    sampler2D Specular;
    float Shininess;
};


in vec2 textureCoordinate;
in vec3 v_normal;
in vec4 v_pos;

uniform vec3 cameraPos;
uniform DirectionalLight directionalLight;
uniform PointLight pointLight;
uniform Material material;


// Function Declarations.
vec3 CalcDirectionalLight(DirectionalLight light, vec3 normal, vec3 viewDir);

float CalcPointAttenuation(PointLight light);
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 viewDir);

float clamp01(float value) { return clamp(value, 0.0, 1.0); }
float sqr(float x) { return x * x; }


void main() 
{
    vec3 viewDir = normalize(cameraPos - v_pos.xyz);
	vec3 lightingResult = CalcDirectionalLight(directionalLight, v_normal, viewDir);
    lightingResult += CalcPointLight(pointLight, v_normal, viewDir);

    FragColor = vec4(lightingResult, 1.0f);
}


// Function Definitions.
vec3 CalcDirectionalLight(DirectionalLight light, vec3 normal, vec3 viewDir)
{
	vec3 lightDir = normalize(-light.Direction);
    
    // Diffuse multiplier.
    float diffuseMult = max(dot(normal, lightDir), 0.0f);

    // Specular multiplier.
    vec3 reflectDir = reflect(-lightDir, normal);
    float specularMult = pow(max(dot(viewDir, reflectDir), 0.0f), material.Shininess);

    // Calculate results.
    vec3 ambient  = light.Ambient * vec3(texture(material.Diffuse, textureCoordinate));
    vec3 diffuse  = light.Diffuse * diffuseMult * vec3(texture(material.Diffuse, textureCoordinate));
    vec3 specular = light.Specular * specularMult * vec3(texture(material.Specular, textureCoordinate));

    // Combine results.
    return (ambient + diffuse + specular);
}
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 viewDir)
{
	vec3 lightDir = normalize(light.Position - v_pos.xyz);

    // Diffuse multiplier.
    float diffuseMult = max(dot(normal, lightDir), 0.0f);

    // Specular multiplier.
    vec3 reflectDir = reflect(-lightDir, normal);
    float specularMult = pow(max(dot(viewDir, reflectDir), 0.0f), material.Shininess);

    // Calculate Distance Attenuation.
    float attenuation = CalcPointAttenuation(light);
    
    // Calculate results.
    vec3 ambient  = light.Ambient * vec3(texture(material.Diffuse, textureCoordinate));
    vec3 diffuse  = light.Diffuse * diffuseMult * vec3(texture(material.Diffuse, textureCoordinate));
    vec3 specular = light.Specular * specularMult * vec3(texture(material.Specular, textureCoordinate));

    // Apply Attenuation.
    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;

    // Combine results.
    return (ambient + diffuse + specular);
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
    float distance = length(light.Position - v_pos.xyz);
    float normalisedDistance = distance / light.Radius;

    if (normalisedDistance >= 1.0)
    {
        // Outwith max range.
        return 0.0;
    }

    float sqrDistance = sqr(normalisedDistance);
    return light.MaxIntensity * sqr(1.0 - sqrDistance) / (1.0 + light.Falloff * normalisedDistance);
}
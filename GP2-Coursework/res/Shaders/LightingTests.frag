#version 400

out vec4 FragColor;

struct DirectionalLight
{
	vec3 Direction;

	vec3 Ambient;
	vec3 Diffuse;
	vec3 Specular;
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
uniform Material material;


// Function Declarations.
vec3 CalcDirectionalLight(DirectionalLight light, vec3 normal, vec3 viewDir);


void main() 
{
    vec3 viewDir = normalize(cameraPos - v_pos.xyz);
	vec3 lightingResult = CalcDirectionalLight(directionalLight, v_normal, viewDir);

    FragColor = vec4(lightingResult, 1.0f);
}


// Function Definitions.
vec3 CalcDirectionalLight(DirectionalLight light, vec3 normal, vec3 viewDir)
{
	vec3 lightDir = normalize(-light.Direction);
    
    // Diffuse multiplier.
    float diffuseMult = max(dot(normal, lightDir), 0.0f);

    // Specular shading.
    vec3 reflectDir = reflect(-lightDir, normal);
    float specularMult = pow(max(dot(viewDir, reflectDir), 0.0f), material.Shininess);

    // Combine results.
    vec3 ambient  = light.Ambient  * vec3(texture(material.Diffuse, textureCoordinate));
    vec3 diffuse  = light.Diffuse  * diffuseMult * vec3(texture(material.Diffuse, textureCoordinate));
    vec3 specular = light.Specular * specularMult * vec3(texture(material.Specular, textureCoordinate));
    return (ambient + diffuse + specular);
}
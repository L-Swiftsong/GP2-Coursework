#version 400
out vec4 FragColor;

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_normal1;

uniform vec3 lightPos;
uniform vec3 viewPos;

in VERTEX_OUT
{
	vec3 FragPos;
	vec2 TexCoords;
	vec3 TangentLightPos;
	vec3 TangentViewPos;
	vec3 TangentFragPos;
} f_in;


void main() 
{
	// Calculate Normal.
	vec3 normal = texture(texture_normal1, f_in.TexCoords).rgb;
	normal = normalize((normal * 2.0f) - 1.0f);

	// Get the diffuse colour.
	vec3 color = texture(texture_diffuse1, f_in.TexCoords).rgb;
	// Ambient.
	vec3 ambient = 0.1f * color;
	// Diffuse.
	vec3 lightDir = normalize(f_in.TangentLightPos - f_in.TangentFragPos);
	float diffuseStrength = max(dot(lightDir, normal), 0.0f);
	vec3 diffuse = diffuseStrength * color;

	// Specular.
	vec3 viewDir = normalize(f_in.TangentViewPos - f_in.TangentFragPos);
	vec3 reflectDir = reflect(-lightDir, normal);
	vec3 halfwayDir = normalize(lightDir + viewDir);
	float specularStrength = pow(max(dot(normal, halfwayDir), 0.0f), 32.0f);
	vec3 specular = vec3(0.2) * specularStrength;

	// Apply Lighting.
	FragColor = vec4(ambient + diffuse + specular, 1.0);
}

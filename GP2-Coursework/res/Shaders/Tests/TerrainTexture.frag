#version 400
out vec4 FragColor;


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

uniform sampler2D texture_diffuse4; // Blend.


in VERTEX_OUT
{
	vec3 FragPos;
	vec2 TexCoords;

	vec3 TangentLightDir;
	vec3 TangentViewPos;
	vec3 TangentFragPos;
} f_in;


// Function Pre-definitions.
vec3 CalculateColor(sampler2D diffuse_texture, sampler2D normal_texture, float texture_scale);

vec3 CalculateMaterialStrengths();


void main() 
{
	// Calculate Individual Material Colours.
	vec3 grass_color = CalculateColor(texture_diffuse1, texture_normal1, grass_texture_scale);
	vec3 dirt_color = CalculateColor(texture_diffuse2, texture_normal2, dirt_texture_scale);
	vec3 rock_color = CalculateColor(texture_diffuse3, texture_normal3, rock_texture_scale);

	vec3 material_strengths = CalculateMaterialStrengths();

	// Apply Lighting.
	//FragColor = vec4(grass_color, material_strengths.r) + vec4(dirt_color, material_strengths.g) + vec4(rock_color, material_strengths.b);
	//vec3 output_color = mix(mix(grass_color, dirt_color, material_strengths.g), rock_color, material_strengths.b);
	vec3 output_color = mix(mix(grass_color, dirt_color, material_strengths.g), rock_color, material_strengths.b);
	FragColor = vec4(output_color, 1.0f);
}


vec3 CalculateColor(sampler2D diffuse_texture, sampler2D normal_texture, float texture_scale)
{
	// Calculate Normal.
	vec3 normal = texture(normal_texture, f_in.TexCoords * texture_scale).rgb;
	normal = normalize((normal * 2.0f) - 1.0f);

	// Get the diffuse colour.
	vec3 color = texture(diffuse_texture, f_in.TexCoords * texture_scale).rgb;

	// Ambient.
	vec3 ambient = 0.1f * color;
	// Diffuse.
	vec3 lightDir = f_in.TangentLightDir;
	float diffuseStrength = max(dot(lightDir, normal), 0.0f);
	vec3 diffuse = diffuseStrength * color;

	// Specular.
	vec3 viewDir = normalize(f_in.TangentViewPos - f_in.TangentFragPos);
	vec3 reflectDir = reflect(-lightDir, normal);
	vec3 halfwayDir = normalize(lightDir + viewDir);
	float specularStrength = pow(max(dot(normal, halfwayDir), 0.0f), 32.0f);
	vec3 specular = vec3(0.2) * specularStrength;


	// Apply Lighting.
	return ambient + diffuse + specular;
}

vec3 CalculateMaterialStrengths()
{
	return texture(texture_diffuse4, f_in.TexCoords).rgb;
}
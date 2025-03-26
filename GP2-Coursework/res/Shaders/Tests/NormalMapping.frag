#version 400
out vec4 FragColor;

uniform sampler2D diffuse;
uniform sampler2D normalMap;

in mat3 TBN;
in vec4 v_pos;
in vec2 v_textureCoordinates;


void main() 
{
	// Calculate Normal.
	vec3 normal = texture(normalMap, v_textureCoordinates).rgb;
	normal = TBN * normal;
	normal = normalize((normal * 2.0f) - 1.0f);


	// Apply Lighting.
	vec3 lightDir = vec3(1.0f, 0.0f, 0.0f);
	float scaledDot = (dot(normal, lightDir) + 1.0f) / 2.0f;
	FragColor = vec4(scaledDot, scaledDot, scaledDot, 1.0);
}

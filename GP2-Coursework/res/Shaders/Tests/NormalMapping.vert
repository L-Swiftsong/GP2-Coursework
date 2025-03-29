#version 400

layout (location = 0) in vec3 VertexPosition;
layout (location = 1) in vec3 VertexNormal;
layout (location = 2) in vec2 TextureCoordinates;
layout (location = 3) in vec3 Tangent;
layout (location = 4) in vec2 Bitangent;

out VERTEX_OUT
{
	vec3 FragPos;
	vec2 TexCoords;
	vec3 TangentLightPos;
	vec3 TangentViewPos;
	vec3 TangentFragPos;
} v_out;

uniform mat4 modelMatrix;
uniform mat4 transform;

uniform vec3 lightPos;
uniform vec3 viewPos;


void main()
{
	// Fragment Position.
	v_out.FragPos = vec3(modelMatrix * vec4(VertexPosition, 1.0f));
	v_out.TexCoords = TextureCoordinates;

	// Calculate the TBN Matrix.
	mat3 normalMatrix = mat3(transpose(inverse(modelMatrix)));
	vec3 normal = normalize(normalMatrix * VertexNormal);
	vec3 tangent = normalize(normalMatrix * Tangent);
	tangent = normalize(tangent - dot(tangent, normal) * normal);
	vec3 biTangent = cross(normal, tangent);

	mat3 TBN = mat3(tangent, biTangent, normal);
	v_out.TangentLightPos = TBN * lightPos;
	v_out.TangentViewPos = TBN * viewPos;
	v_out.TangentFragPos = TBN * v_out.FragPos;


	// Output Position.
	gl_Position = transform * vec4(VertexPosition, 1.0);
}
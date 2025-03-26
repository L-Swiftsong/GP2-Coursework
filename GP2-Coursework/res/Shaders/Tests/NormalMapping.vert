#version 400

layout (location = 0) in vec3 VertexPosition;
layout (location = 1) in vec2 TextureCoordinate;
layout (location = 2) in vec3 VertexNormal;

uniform mat4 modelMatrix;
uniform mat4 transform;

out mat3 TBN;
out vec4 v_pos; 
out vec2 v_textureCoordinates; 

void main()
{
	// Fragment Position.
	v_pos = modelMatrix * vec4(VertexPosition, 1.0f);

	v_textureCoordinates = TextureCoordinate;

	// Calculate TBN Matrix.
	mat3 normalMatrix = mat3(transpose(inverse(modelMatrix)));
	vec3 normal = normalize(normalMatrix * VertexNormal);
	vec3 tangent = normalize(normalMatrix * vec3(TextureCoordinate, 0.0));
	vec3 biTangent = cross(normal, tangent);
	mat3 TBN = mat3(tangent, biTangent, normal);


	// Output Position.
	gl_Position = transform * vec4(VertexPosition, 1.0);
}
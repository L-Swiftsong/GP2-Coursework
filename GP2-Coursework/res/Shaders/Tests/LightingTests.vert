#version 400

layout (location = 0) in vec3 VertexPosition;
layout (location = 1) in vec3 VertexNormal;
layout (location = 2) in vec2 TextureCoordinate;

uniform mat4 modelMatrix;
uniform mat4 transform;

out vec2 textureCoordinate;
out vec3 v_normal;
out vec4 v_pos; 

void main()
{
	// Fragment Position.
	v_pos = modelMatrix * vec4(VertexPosition, 1.0f);

	// Normal Matrix.
	mat3 normalMatrix = mat3(transpose(inverse(modelMatrix)));
	v_normal = normalize(normalMatrix * VertexNormal);

	// Texture Coordinate
	textureCoordinate = TextureCoordinate;


	// Output Position.
	gl_Position = transform * vec4(VertexPosition, 1.0);
}
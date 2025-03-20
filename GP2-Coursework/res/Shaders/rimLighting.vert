#version 400

layout (location = 0) in vec3 VertexPosition;
layout (location = 1) in vec2 TextureCoordinate;
layout (location = 2) in vec3 VertexNormal;

uniform mat4 transform;
uniform vec3 cameraPos;

out vec3 v_normal;
out vec4 v_pos;
out vec2 textureCoordinate;
out vec3 inverseViewDirection;

void main()
{
	// Calculate correct rotation normal.
	v_normal = normalize(VertexNormal * inverse(transpose(mat3(transform))));

	textureCoordinate = TextureCoordinate;

	v_pos = transform * vec4(VertexPosition, 1.0);
	inverseViewDirection = normalize(-vec3(v_pos.xyz - cameraPos));

	gl_Position = transform * vec4(VertexPosition, 1.0);
}
#version 400

layout (location = 0) in vec3 VertexPosition;
layout (location = 2) in vec2 TextureCoordinate;

uniform mat4 transform;

out vec2 texture_coordinate;

void main()
{
	texture_coordinate = TextureCoordinate;

	// Output Position.
	gl_Position = transform * vec4(VertexPosition, 1.0);
}
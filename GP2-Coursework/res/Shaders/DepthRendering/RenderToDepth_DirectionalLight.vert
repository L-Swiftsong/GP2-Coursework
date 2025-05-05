#version 400

layout (location = 0) in vec3 VertexPosition;

uniform mat4 light_space_matrix;
uniform mat4 modelMatrix;


void main()
{
	gl_Position = light_space_matrix * modelMatrix * vec4(VertexPosition, 1.0f);
}
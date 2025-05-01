#version 400

layout (location = 0) in vec3 VertexPosition;

uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

out vec3 texture_direction;

void main()
{
    vec4 pos = projectionMatrix * viewMatrix * vec4(VertexPosition, 1.0);
	gl_Position = pos.xyww;

	texture_direction = VertexPosition;//vec3(VertexPosition.x, VertexPosition.y, -VertexPosition.z);
}
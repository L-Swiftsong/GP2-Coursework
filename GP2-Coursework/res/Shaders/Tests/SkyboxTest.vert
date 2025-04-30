#version 400

layout (location = 0) in vec3 VertexPosition;

uniform mat4 view;
uniform mat4 projection;

out vec3 texture_direction;

void main()
{
    vec4 pos = projection * view * vec4(VertexPosition, 1.0);
	gl_Position = pos.xyww;

	texture_direction = vec3(VertexPosition.x, VertexPosition.y, -VertexPosition.z);
}
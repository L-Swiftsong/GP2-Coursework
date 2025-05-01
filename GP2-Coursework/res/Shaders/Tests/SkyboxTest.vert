#version 400

layout (location = 0) in vec3 VertexPosition;

uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

uniform float skybox_1_rotation;
uniform float skybox_2_rotation;

out vec3 texture_direction;


// Function Pre-declarations.
vec3 RotateAroundYInDegrees(vec3 vertex, float degrees);


void main()
{
	/*
	vec3 rotatedPos = RotateAroundYInDegrees(VertexPosition, skybox_1_rotation);
    vec4 pos = projectionMatrix * viewMatrix * vec4(rotatedPos, 1.0);

	texture_direction = rotatedPos;



	gl_Position = pos.xyww;
	*/


	// Old Code.
    vec4 pos = projectionMatrix * viewMatrix * vec4(VertexPosition, 1.0);
	gl_Position = pos.xyww;
	texture_direction = VertexPosition;
}
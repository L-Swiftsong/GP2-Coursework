#version 400
out vec4 FragColor;

uniform samplerCube skybox;

in vec3 texture_direction;


void main() 
{
	FragColor = texture(skybox, texture_direction);
}

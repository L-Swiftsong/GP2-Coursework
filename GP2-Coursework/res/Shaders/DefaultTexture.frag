#version 400
out vec4 FragColor;

uniform sampler2D texture_diffuse1;

in vec2 texture_coordinate;


void main() 
{
	FragColor = texture(texture_diffuse1, texture_coordinate);
}

#version 400
out vec4 FragColor;

uniform sampler2D texture_diffuse1;

in vec3 v_normal;
in vec4 v_pos;
in vec2 texture_coordinates;


void main() 
{
	FragColor = texture(texture_diffuse1, texture_coordinates);
}

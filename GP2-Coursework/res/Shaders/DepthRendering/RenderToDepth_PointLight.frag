#version 400
in vec4 FragPos;

uniform vec3 light_pos;
uniform float far_plane;

void main()
{
	// Get the distance between this fragment and our light source.
	float light_distance = length(FragPos.xyz - light_pos);

	// Map to between 0 & 1 by dividing by our far clipping plane.
	light_distance /= far_plane;

	// Write our distance as the modified depth.
	gl_FragDepth = light_distance;
}
#version 400
out vec4 FragColor;

uniform samplerCube skybox1;
uniform samplerCube skybox2;
uniform samplerCube sun_and_moon_map;
uniform float skybox_blend_value;

in vec3 texture_direction;

uniform mat3 sun_rotation_matrix;


void main() 
{
	vec4 skybox1Color = texture(skybox1, texture_direction);
	vec4 skybox2Color = texture(skybox2, texture_direction);

	// Triangle Wave.
	float transformed_blend_value = 2.0f * abs(2.0f * (skybox_blend_value - floor(0.5f + skybox_blend_value))) - 1.0f;

	float skybox_1_strength = 1.0f - skybox_blend_value; //clamp(transformed_blend_value, 0.0f, 1.0f);
	float skybox_2_strength = skybox_blend_value; //clamp(-transformed_blend_value, 0.0f, 1.0f);

	// Calculate our sun/moon colour.
	vec3 rotated_texture_direction = sun_rotation_matrix * texture_direction;
	vec4 sun_color = texture(sun_and_moon_map, rotated_texture_direction);

	// Determine our final colour.
	FragColor = vec4(mix(skybox1Color, skybox2Color, skybox_blend_value).rgb + sun_color.rgb, 1.0f);
}

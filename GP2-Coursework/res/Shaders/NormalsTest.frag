#version 400
out vec4 FragColor;

in vec3 v_normal;
in vec4 v_pos;


void main() 
{
	vec3 lightDir = vec3(1.0f, 0.0f, 0.0f);
	float dotProduct = dot(v_normal, lightDir);
	vec3 color;
	if (dotProduct >= 0.0f)
	{
		color = vec3(1.0f, 1.0f, 1.0f);
	}
	else
	{
		color = vec3(0.0f, 0.0f, 0.0f);
	}
	
	float scaledDot = (dotProduct + 1.0f) / 2.0f;
	FragColor = vec4(scaledDot, scaledDot, scaledDot, 1.0);
}

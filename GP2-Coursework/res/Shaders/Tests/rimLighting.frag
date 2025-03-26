#version 400

uniform vec4 rimColour;
uniform vec4 lightColour;
uniform vec3 lightDir;
uniform sampler2D diffuse;

in vec3 v_normal;
in vec4 v_pos;
in vec2 textureCoordinate;
in vec3 inverseViewDirection;

int modeIndex = 0;
bool applyIntensityToObject = false;

out vec4 FragColor;

void main()
{
	// To-do: Take 'lightDirection' into consideration.
	// To-do: Prevent rim lighting appearing on occluded objects.


	float dotProduct = dot(v_normal, inverseViewDirection);
	float rimFactor = 1.0 - max(0.0, dotProduct);

	float intensity = 0;
	if (modeIndex == 0)
	{
		// Stepped Transition.
		if (rimFactor > 0.9)
		{
			intensity = 1;
		}
		else if (rimFactor > 0.5)
		{
			intensity = 0.5;
		}
	}
	else if (modeIndex == 1)
	{
		// Pow Function.
		intensity = pow(rimFactor, 4.0f);
	}
	else if (modeIndex == 2)
	{
		// Smoothstep Function.
		const float LOWER_BOUNDS = 0.2;
		const float UPPER_BOUNDS = 0.8;
		intensity = smoothstep(LOWER_BOUNDS, UPPER_BOUNDS, rimFactor);
	}
	

	if (applyIntensityToObject)
	{
		FragColor = (texture2D(diffuse, textureCoordinate) * lightColour) * (rimColour * intensity);
	}
	else
	{
		float lightIntensity = pow(dot(v_normal, lightDir), 2);
		FragColor = (texture2D(diffuse, textureCoordinate) * lightColour * lightIntensity) + (rimColour * intensity);
	}
}
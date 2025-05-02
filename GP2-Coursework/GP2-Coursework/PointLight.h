#pragma once

#include "Light.h"

struct PointLight : Light
{
public:
	PointLight() :
		position_(glm::vec3(0.0f)),
		diffuse_(glm::vec3(0.8f)),
		radius_ (3.0f), max_intensity_(1.0f), falloff_(0.5f)
	{}
	PointLight(glm::vec3 position, glm::vec3 diffuse, float radius, float max_intensity, float falloff) :
		position_(position),
		diffuse_(diffuse),
		radius_(radius), max_intensity_(max_intensity), falloff_(falloff)
	{}


	const virtual void UpdateShader(const Shader& shader) override
	{
		shader.set_vec_3("pointLight.Position", position_);

		shader.set_vec_3("pointLight.Diffuse", diffuse_);

		shader.set_float("pointLight.Radius", radius_);
		shader.set_float("pointLight.MaxIntensity", max_intensity_);
		shader.set_float("pointLight.Falloff", falloff_);
	}

private:
	glm::vec3 position_;

	glm::vec3 diffuse_;

	float radius_;
	float max_intensity_;
	float falloff_;
};
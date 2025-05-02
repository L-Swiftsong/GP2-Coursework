#pragma once

#include "Light.h"

struct DirectionalLight : Light
{
public:
	DirectionalLight() :
		direction_(glm::vec3(0.0f, -1.0f, 0.0f)),
		diffuse_(glm::vec3(1.0f)),
		intensity_(0.8f)
	{}
	DirectionalLight(glm::vec3 direction, glm::vec3 diffuse, float intensity) :
		direction_(direction),
		diffuse_(diffuse),
		intensity_(intensity)
	{}


	const virtual void UpdateShader(const Shader& shader) override
	{
		shader.set_vec_3("directionalLight.Direction", direction_);
		shader.set_vec_3("directionalLight.Diffuse", diffuse_ * intensity_);
	}


	// Getters & Setters.
	inline void set_direction(glm::vec3 new_direction) { direction_ = new_direction; }
	inline glm::vec3 get_direction() { return direction_; }

	inline void set_diffuse(glm::vec3 new_diffuse) { diffuse_ = new_diffuse; }
	inline glm::vec3 get_diffuse() { return diffuse_; }

	inline void set_intensity(float new_intensity) { intensity_ = new_intensity; }
	inline float get_intensity() { return intensity_; }


private:
	glm::vec3 direction_;

	glm::vec3 diffuse_;
	float intensity_;
};
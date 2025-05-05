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


	// Getters & Setters.
	inline void set_position(glm::vec3 new_position) { position_ = new_position; }
	inline glm::vec3 get_position() { return position_; }

	inline void set_diffuse(glm::vec3 new_diffuse) { diffuse_ = new_diffuse; }
	inline glm::vec3 get_diffuse() { return diffuse_; }

	inline void set_radius(float new_radius) { radius_ = new_radius; }
	inline float get_radius() { return radius_; }
	inline void set_max_intensity(float new_max_intensity) { max_intensity_ = new_max_intensity; }
	inline float get_max_intensity() { return max_intensity_; }
	inline void set_falloff(float new_falloff) { falloff_ = new_falloff; }
	inline float get_falloff() { return falloff_; }

private:
	glm::vec3 position_;

	glm::vec3 diffuse_;

	float radius_;
	float max_intensity_;
	float falloff_;
};
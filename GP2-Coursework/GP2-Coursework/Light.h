#pragma once

#include "Shader.h"

struct Light
{
public:
	Light() : shadow_map_fbo(-1), shadow_map(-1),
		transform_(std::make_unique<Transform>())
	{}
	Light(glm::vec3 position) : shadow_map_fbo(-1), shadow_map(-1),
		transform_(std::make_unique<Transform>(position, glm::vec3(0.0f), glm::vec3(1.0f)))
	{}

	const virtual void UpdateShader(const Shader* shader, const int& light_index) = 0;

	unsigned int shadow_map_fbo, shadow_map;


	// Getters and Setters.
	inline Transform* get_transform() const { return transform_.get(); }

protected:
	std::unique_ptr<Transform> transform_;
};
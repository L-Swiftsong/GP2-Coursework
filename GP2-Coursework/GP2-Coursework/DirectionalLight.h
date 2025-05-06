#pragma once

#include "Light.h"

struct DirectionalLight : Light
{
public:
	DirectionalLight() :
		direction_(glm::vec3(0.0f, -1.0f, 0.0f)),
		diffuse_(glm::vec3(1.0f)),
		intensity_(0.8f)
	{
		InitialiseShadowMap();
	}
	DirectionalLight(glm::vec3 direction, glm::vec3 diffuse, float intensity) :
		direction_(direction),
		diffuse_(diffuse),
		intensity_(intensity)
	{
		InitialiseShadowMap();
	}


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


	void InitialiseShadowMap()
	{
		glGenFramebuffers(1, &shadow_map_fbo);

		// Generate the Depth Buffer Texture.
		glGenTextures(1, &shadow_map);
		glBindTexture(GL_TEXTURE_2D, shadow_map);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, Light::kShadowTextureWidth, Light::kShadowTextureHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
		float border_color[] = { 1.0f, 1.0f, 1.0f, 1.0f };
		glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, border_color);

		// Set the Framebuffer's Depth Buffer as our generated texture.
		glBindFramebuffer(GL_FRAMEBUFFER, shadow_map_fbo);
		glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, shadow_map, 0);
		glDrawBuffer(GL_NONE);
		glReadBuffer(GL_NONE);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
};
#pragma once

#include "Light.h"

struct PointLight : Light
{
public:
	PointLight() :
		position_(glm::vec3(0.0f)),
		diffuse_(glm::vec3(0.8f)),
		radius_ (3.0f), max_intensity_(1.0f), falloff_(0.5f)
	{
		InitialiseShadowMap();
	}
	PointLight(glm::vec3 position, glm::vec3 diffuse, float radius, float max_intensity, float falloff) :
		position_(position),
		diffuse_(diffuse),
		radius_(radius), max_intensity_(max_intensity), falloff_(falloff)
	{
		InitialiseShadowMap();
	}

	const virtual void UpdateShader(const Shader& shader, const int& light_index) override
	{
		shader.set_vec_3("point_lights[" + std::to_string(light_index) + "].Position", position_, true);
		shader.set_vec_3("point_lights[" + std::to_string(light_index) + "].Diffuse", diffuse_, true);

		shader.set_float("point_lights[" + std::to_string(light_index) + "].Radius", radius_, true);
		shader.set_float("point_lights[" + std::to_string(light_index) + "].MaxIntensity", max_intensity_, true);
		shader.set_float("point_lights[" + std::to_string(light_index) + "].Falloff", falloff_, true);
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



	void InitialiseShadowMap()
	{
		// Generate our Framebuffer.
		glGenFramebuffers(1, &shadow_map_fbo);

		// Generate the Depth Buffer Texture.
		glGenTextures(1, &shadow_map);
		glBindTexture(GL_TEXTURE_CUBE_MAP, shadow_map);
		for (unsigned int i = 0; i < 6; ++i)
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT, Light::kShadowTextureWidth, Light::kShadowTextureHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

		// Set the Framebuffer's Depth Buffer as our generated texture.
		glBindFramebuffer(GL_FRAMEBUFFER, shadow_map_fbo);
		glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, shadow_map, 0);
		glDrawBuffer(GL_NONE);
		glReadBuffer(GL_NONE);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
};
#pragma once

#include "Shader.h"

struct Light
{
public:
	Light() : shadow_map_fbo(-1), shadow_map(-1) {}

	const virtual void UpdateShader(const Shader& shader) = 0;

	static const unsigned int kShadowTextureWidth = 2048, kShadowTextureHeight = 2048;
	unsigned int shadow_map_fbo, shadow_map;
};
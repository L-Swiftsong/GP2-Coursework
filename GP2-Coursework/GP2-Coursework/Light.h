#pragma once

#include "Shader.h"

struct Light
{
public:
	const virtual void UpdateShader(const Shader& shader) = 0;
};
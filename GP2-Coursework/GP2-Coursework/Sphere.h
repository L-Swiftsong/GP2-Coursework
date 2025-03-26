#pragma once

#include <glm\glm.hpp>
#include <GL\glew.h>

struct Sphere
{
public:

	Sphere() : pos_(glm::vec3(0.0f, 0.0f, 0.0f)), radius_(0.0f) {}

	Sphere(glm::vec3& pos, float radius)
	{
		this->pos_ = pos;
		this->radius_ = radius;
	}

	glm::vec3 get_pos() const { return pos_; }
	float get_radius() const { return radius_; }

	void set_pos(glm::vec3 pos)
	{
		this->pos_ = pos;
	}

	void set_radius(float radius)
	{
		this->radius_ = radius;
	}

private:
	glm::vec3 pos_;
	float radius_;
};
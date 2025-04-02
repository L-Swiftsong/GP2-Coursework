#pragma once

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include "transform.h"

struct Camera
{
public:
	Camera(const glm::vec3& pos_, float fov, float aspect, float near_clip, float far_clip)	:
		transform_(new Transform(pos_, glm::vec3(0.0f), glm::vec3(1.0f))),
		projection_(glm::perspective(fov, aspect, near_clip, far_clip))
	{}
	~Camera()
	{
		delete this->transform_;
	}

	Transform* get_transform() const { return transform_; }


	inline glm::mat4 get_view_projection() const
	{
		return projection_ * glm::lookAt(transform_->get_pos(), transform_->get_pos() + transform_->get_forward(), transform_->get_up());
	}
	inline glm::mat4 get_projection() const
	{
		return projection_;
	}
	inline glm::mat4 get_view() const
	{
		return glm::lookAt(transform_->get_pos(), transform_->get_pos() + transform_->get_forward(), transform_->get_up());
	}

protected:
private:
	Camera() : transform_(new Transform()), projection_(glm::perspective(70.0f, 1.778f, 0.01f, 1000.0f))
	{}

	glm::mat4 projection_;
	Transform* transform_;
};



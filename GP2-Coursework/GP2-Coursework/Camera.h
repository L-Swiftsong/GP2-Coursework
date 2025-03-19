#pragma once

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>

struct Camera
{
public:
	Camera(const glm::vec3& pos_, float fov, float aspect, float near_clip, float far_clip)
	{
		this->pos_ = pos_;
		this->forward_ = glm::vec3(0.0f, 0.0f, 1.0f);
		this->up_ = glm::vec3(0.0f, 1.0f, 0.0f);
		this->projection_ = glm::perspective(fov, aspect, near_clip, far_clip);
	}

	glm::vec3 get_pos()
	{
		return this->pos_;
	}
	glm::vec3 get_forward()
	{
		return this->forward_;
	}

	inline glm::mat4 get_view_projection() const
	{
		return projection_ * glm::lookAt(pos_, pos_ + forward_, up_);
	}

	inline glm::mat4 get_projection() const
	{
		return projection_;
	}

	inline glm::mat4 get_view() const
	{
		return glm::lookAt(pos_, pos_ + forward_, up_);
	}

	//void MoveForward(float amt)
	//{
	//	pos_ += forward_ * amt;
	//}

	//void MoveRight(float amt)
	//{
	//	pos_ += glm::cross(up_, forward) * amt;
	//}

	//void Pitch(float angle)
	//{
	//	glm::vec3 right = glm::normalize(glm::cross(up_, forward_));

	//	forward_ = glm::vec3(glm::normalize(glm::rotate(angle, right) * glm::vec4(forward_, 0.0)));
	//	up_ = glm::normalize(glm::cross(forward_, right));
	//}

	void RotateY(float angle)
	{
		static const glm::vec3 kUp(0.0f, 1.0f, 0.0f);

		glm::mat4 rotation = glm::rotate(angle, kUp);

		forward_ = glm::vec3(glm::normalize(rotation * glm::vec4(forward_, 0.0)));
		up_ = glm::vec3(glm::normalize(rotation * glm::vec4(up_, 0.0)));
	}

protected:
private:
	Camera() : pos_(glm::vec3(0, 0, 0)), forward_(glm::vec3(0.0f, 0.0f, 1.0f)), up_(glm::vec3(0.0f, 1.0f, 0.0f))
	{
		this->projection_ = glm::perspective(70.0f, 1.778f, 0.01f, 1000.0f);
	}

	glm::mat4 projection_;
	glm::vec3 pos_;
	glm::vec3 forward_;
	glm::vec3 up_;
};



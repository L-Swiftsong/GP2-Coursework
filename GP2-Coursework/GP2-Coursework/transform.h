#pragma once

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include "camera.h"

struct Transform
{
public:
	Transform(const glm::vec3& pos = glm::vec3(), const glm::vec3& rot = glm::vec3(), const glm::vec3& scale = glm::vec3(1.0f, 1.0f, 1.0f))
	{
		this->pos_ = pos;
		this->rot_ = rot;
		this->scale_ = scale;
	}

	inline glm::mat4 get_model() const
	{
		glm::mat4 posMat = glm::translate(pos_);
		glm::mat4 scaleMat = glm::scale(scale_);
		glm::mat4 rotX = glm::rotate(rot_.x, glm::vec3(1.0, 0.0, 0.0));
		glm::mat4 rotY = glm::rotate(rot_.y, glm::vec3(0.0, 1.0, 0.0));
		glm::mat4 rotZ = glm::rotate(rot_.z, glm::vec3(0.0, 0.0, 1.0));
		glm::mat4 rotMat = rotX * rotY * rotZ;

		return posMat * rotMat * scaleMat;
	}

	/*inline glm::mat4 GetMVP(const Camera& camera) const
	{
		glm::mat4 VP = camera.GetViewProjection();
		glm::mat4 M = GetModel();

		return VP * M;//camera.GetViewProjection() * GetModel();
	}*/

	inline glm::vec3* get_pos() { return &pos_; } //getters
	inline glm::vec3* get_rot() { return &rot_; }
	inline glm::vec3* get_scale() { return &scale_; }

	inline void set_pos(glm::vec3& pos) { this->pos_ = pos; } // setters
	inline void set_rot(glm::vec3& rot) { this->rot_ = rot; }
	inline void set_scale(glm::vec3& scale) { this->scale_ = scale; }
protected:
private:
	glm::vec3 pos_;
	glm::vec3 rot_;
	glm::vec3 scale_;
};



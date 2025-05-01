#pragma once

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <iostream>

struct Transform
{
public:
	Transform(const glm::vec3& pos = glm::vec3(), const glm::vec3& rot = glm::vec3(), const glm::vec3& scale = glm::vec3(1.0f, 1.0f, 1.0f))
	{
		this->pos_ = pos;
		this->rot_ = glm::quat(rot);
		this->scale_ = scale;
	}

	inline glm::mat4 get_model() const
	{
		glm::mat4 posMat = glm::translate(pos_);
		glm::mat4 scaleMat = glm::scale(scale_);
		glm::mat4 rotMat = glm::toMat4(rot_);

		return posMat * rotMat * scaleMat;
	}

	/*inline glm::mat4 GetMVP(const Camera& camera) const
	{
		glm::mat4 VP = camera.GetViewProjection();
		glm::mat4 M = GetModel();

		return VP * M;//camera.GetViewProjection() * GetModel();
	}*/


	// Position.
	inline glm::vec3 get_pos() const { return pos_; }
	inline void set_pos(glm::vec3& pos) { this->pos_ = pos; }

	// Rotation.
	inline glm::quat get_rot() const { return rot_; }
	inline glm::vec3 get_euler_angles() const { return glm::eulerAngles(rot_); }
	inline void set_rot(glm::quat& rot) { this->rot_ = rot; }
	inline void set_euler_angles(glm::vec3& rot) { this->rot_ = glm::quat(rot); }

	// Scale.
	inline glm::vec3 get_scale() const { return scale_; }
	inline void set_scale(glm::vec3& scale) { this->scale_ = scale; }

	// Orientation.
	inline glm::vec3 get_forward() const { return kWorldForward * this->rot_; }
	inline glm::vec3 get_up() const { return kWorldUp * this->rot_; }
	inline glm::vec3 get_right() const { return kWorldRight * this->rot_; }


	enum RotationSpace
	{
		kLocalSpace,
		kWorldSpace
	};

	void Rotate(glm::vec3 axis, float angle, RotationSpace rotationSpace = RotationSpace::kLocalSpace)
	{
		if (rotationSpace == RotationSpace::kLocalSpace)
		{
			// Local Space Rotation.
			// Rotate the desired axis of rotation based on our current rotation.
			axis = glm::normalize(rot_ * axis);
		}

		// Calculate and set our new rotation (Second Rot * First Rot).
		glm::quat orientation_quaternion = glm::angleAxis(angle, axis);
		rot_ = (orientation_quaternion) * rot_;
	}
	void RotateAroundPoint(glm::vec3 point, glm::vec3 rotation_axis, float angle)
	{
		// Rotate our position around the specified point.
		glm::quat position_orientation_quaternion = glm::angleAxis(angle, rotation_axis);
		glm::vec3 rotated_point = point + (position_orientation_quaternion * (this->get_pos() - point));
		this->set_pos(rotated_point);


		// Rotate the desired axis of rotation based on our current rotation.
		rotation_axis = glm::normalize(rot_ * rotation_axis);

		// Apply our rotation.
		rot_ = glm::angleAxis(-angle, rotation_axis) * rot_;
	}

protected:
private:
	const glm::vec3 kWorldForward = glm::vec3(0.0f, 0.0f, 1.0f);
	const glm::vec3 kWorldUp = glm::vec3(0.0f, 1.0f, 0.0f);
	const glm::vec3 kWorldRight = glm::vec3(1.0f, 0.0f, 1.0f);


	glm::vec3 pos_;
	glm::quat rot_;
	glm::vec3 scale_;
};



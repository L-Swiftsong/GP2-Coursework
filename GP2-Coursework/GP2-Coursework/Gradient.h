#pragma once

#include <SDL\SDL.h>
#include <glm/glm.hpp>
#include <vector>
#include <array>

class GradientValue
{
public:
	GradientValue(glm::vec3 value) : value_(glm::vec4(value.x, value.y, value.z, 1.0f)) {}
	GradientValue(glm::vec4 value) : value_(value) {}
	GradientValue(float r, float g, float b) : value_(glm::vec4(r, g, b, 1.0f)) {}
	GradientValue(float r, float g, float b, float a) : value_(glm::vec4(r, g, b, a)) {}


	const GradientValue& operator +=(const GradientValue& lhs)
	{
		value_.x += lhs.value_.x;
		value_.y += lhs.value_.y;
		value_.z += lhs.value_.z;
		value_.w += lhs.value_.w;
		return *this;
	}
	const GradientValue& operator -=(const GradientValue& lhs)
	{
		value_.x -= lhs.value_.x;
		value_.y -= lhs.value_.y;
		value_.z -= lhs.value_.z;
		value_.w -= lhs.value_.w;
		return *this;
	}
	const GradientValue& operator *=(const float& lhs)
	{
		value_.x *= lhs;
		value_.y *= lhs;
		value_.z *= lhs;
		value_.w *= lhs;
		return *this;
	}

	const GradientValue operator+(const GradientValue& lhs) const
	{
		GradientValue res = *this;
		res += lhs;
		return res;
	}
	const GradientValue operator-(const GradientValue& lhs) const
	{
		GradientValue res = *this;
		res -= lhs;
		return res;
	}
	const GradientValue operator*(const float& lhs) const
	{
		GradientValue res = *this;
		res *= lhs;
		return res;
	}

	glm::vec4 get_value() const { return value_; }


private:
	glm::vec4 value_;
};
struct GradientStop
{
public:
	GradientStop(float time, glm::vec3 value) : t_(time), value_(GradientValue(value)) {}
	GradientStop(float time, glm::vec4 value) : t_(time), value_(GradientValue(value)) {}

	float get_time() const { return t_; }
	GradientValue get_value() const { return value_; }

private:
	float t_;
	GradientValue value_;
};

struct Gradient
{
public:
	Gradient(bool forwardBackConstrain) : forward_back_constrain_(forwardBackConstrain) {}
	Gradient(bool forwardBackConstrain, std::vector<std::tuple<float, glm::vec3>> values) : forward_back_constrain_(forwardBackConstrain)
	{
		for (int i = 0; i < values.size(); ++i)
		{
			AddStop(std::get<0>(values[i]), std::get<1>(values[i]));
		}
	}
	~Gradient()
	{
		for (typename std::vector<GradientStop*>::iterator it = stops_.begin(); it != stops_.end(); it++)
		{
			delete* it;
		}
	}

	void AddStop(float time, glm::vec3 value)
	{
		std::vector<GradientStop*>::iterator iterator;
		for (iterator = stops_.begin(); iterator != stops_.end(); iterator++) {
			if ((*iterator)->get_time() > time)
			{
				break;
			}
		}

		stops_.insert(iterator, new GradientStop(time, value));
	}

	glm::vec3 get_value(float time)
	{
		// Ensure that we have at least one value.
		if (stops_.empty())
		{
			return glm::vec3(0.0f);
		}

		// Constrain our time to between 0.0f & 1.0f.
		time = ConstrainFloat(time);

		if (stops_.front()->get_time() >= time)
		{
			// Our time is before our first step.
			return stops_.front()->get_value().get_value();
		}
		else if (stops_.back()->get_time() <= time)
		{
			// Our time is after our last step.
			return stops_.back()->get_value().get_value();
		}

		// Find the two GradientStops that our time lies between.
		std::vector<GradientStop*>::iterator iterator;
		GradientStop *start = NULL, *stop = NULL;
		for (iterator = stops_.begin(); iterator != stops_.end(); ++iterator)
		{
			stop = *iterator;
			if (stop->get_time() > time)
			{
				start = *(--iterator);
				float percent = (time - start->get_time()) / (stop->get_time() - start->get_time());
				return Lerp(start->get_value(), stop->get_value(), percent);
			}
		}

		// Something went wrong when finding our time.
		throw ("Error when trying to retreive Gradient Value for time " + std::to_string(time));
	}

private:
	std::vector<GradientStop*> stops_;
	bool forward_back_constrain_;


	inline float ConstrainFloat(const float value) const
	{
		// Ensure the value is between 0 & 1, using an up-down zig-zag motion of values to allow smooth back-transitioning.
		if (value > 1.0f)
		{
			if (forward_back_constrain_)
			{
				int floorVal = std::floor(value);
				if (floorVal % 2 == 0)
				{
					// Even.
					return value - floorVal;
				}
				else
				{
					// Odd.
					return (floorVal + 1.0f) - value;
				}
			}
			else
			{
				return value - std::floor(value);
			}
		}

		return value;
	}
	inline glm::vec4 Lerp(const GradientValue& a, const GradientValue& b, float t) const
	{
		float ft = t * M_PI;
		float f = (1.0 - cos(ft)) / 2.0;
		return (a * (1.0 - f) + b * f).get_value();
	}
};

/*struct Gradient
{
public:
	Gradient(const int length, const glm::vec4 colours[], const float times[])
	{
		this->size_ = length;
		this->colour_array_ = new glm::vec4[length];
		this->time_array_ = new float[length];

		for (int i = 0; i < length; ++i)
		{
			this->colour_array_[i] = colours[i];
			this->time_array_[i] = times[i];
		}
	};
	Gradient(const int length, const glm::vec3 colours[], const float times[])
	{
		this->size_ = length;
		this->colour_array_ = new glm::vec4[length];
		this->time_array_ = new float[length];

		for (int i = 0; i < length; ++i)
		{
			this->colour_array_[i] = glm::vec4(colours[i].x, colours[i].y, colours[i].z, 1.0f);
			this->time_array_[i] = times[i];
		}
	};
	~Gradient()
	{
		delete[] colour_array_;
		delete[] time_array_;
	};

	glm::vec4 get_colour(float value) const
	{
		value = ConstrainFloat(value);


		if (time_array_[0] > value)
		{
			// The value was below our first index.
			return colour_array_[0];
		}

		for (int i = 0; i < size_ - 1; ++i)
		{
			if (time_array_[i + 1] > value)
			{
				// Linearly interpolate between the two colours.
				float percentBetween = value / (time_array_[i] + time_array_[i + 1]);
				return InterpolateBetween(colour_array_[i], colour_array_[i + 1], percentBetween);
			}
		}

		// The value was above our last index.
		return colour_array_[size_ - 1];
	}


private:
	int size_;
	glm::vec4 *colour_array_;
	float *time_array_;

	inline float ConstrainFloat(const float value) const
	{
		// Ensure the value is between 0 & 1, using an up-down zig-zag motion of values to allow smooth back-transitioning.
		if (value > 1.0f)
		{
			int floorVal = std::floor(value);
			if (floorVal % 2 == 0)
			{
				// Even.
				return value - floorVal;
			}
			else
			{
				// Odd.
				return (floorVal + 1.0f) - value;
			}
		}

		return value;
	}

	inline glm::vec4 InterpolateBetween(glm::vec4 val1, glm::vec4 val2, float t) const
	{
		return glm::vec4(
			lerp(val1.x, val2.x, t),
			lerp(val1.y, val2.y, t),
			lerp(val1.z, val2.z, t),
			lerp(val1.w, val2.w, t)
		);
	}
	float lerp(const float a, const float b, const float f) const
	{
		return a * (1.0f - f) + (b * f);
	}
};*/
#pragma once

#include <SDL\SDL.h>
#include <glm/gtx/transform.hpp>
#include "MainGame.h"
#include "Display.h"

// For Debugging.
#include <string>
#include <iostream>

class InputManager
{
public:
	InputManager() : receiving_mouse_input(true) {}

	void ProcessInput(Display* display);


	const glm::vec3 get_camera_movement_input_normalized();
	const glm::vec2 get_camera_look_input();
	const bool get_sprint_held();

	const bool get_speedup_time_held();

private:
	void PrintKeyInfo(SDL_KeyboardEvent* key);

	const glm::vec3 SafeNormalizeVec3(const glm::vec3& vector);
	const glm::vec2 SafeNormalizeVec2(const glm::vec2& vector);

	glm::vec3 camera_movement_input_;
	glm::vec2 camera_look_input_;
	bool sprint_held_;


	bool receiving_mouse_input;
	bool speedup_time_held_;


	const glm::vec2 kVector2None = glm::vec2(0.0f);
	const glm::vec3 kVector3None = glm::vec3(0.0f);
};


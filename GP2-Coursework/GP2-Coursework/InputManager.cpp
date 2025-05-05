#include "InputManager.h"


void InputManager::ProcessInput()
{
	SDL_Event event;

	while (SDL_PollEvent(&event)) //get and process events
	{
		switch (event.type)
		{
		case SDL_QUIT:
			//game_state_ = GameState::kExit;
			break;
		case SDL_KEYDOWN:
			switch (event.key.keysym.sym)
			{
				// Camera Movement.
				case SDLK_w: camera_movement_input_.z = 1.0f; break;
				case SDLK_s: camera_movement_input_.z = -1.0f; break;
				case SDLK_a: camera_movement_input_.x = 1.0f; break;
				case SDLK_d: camera_movement_input_.x = -1.0f; break;
				case SDLK_SPACE: camera_movement_input_.y = 1.0f; break;
				case SDLK_LCTRL: camera_movement_input_.y = -1.0f; break;

				case SDLK_LSHIFT: sprint_held_ = true; break;
			}
			break;
		// Mouse Movement.
		case SDL_MOUSEMOTION:
			//camera_look_input_.x = event.motion.xrel;
			//camera_look_input_.y = event.motion.yrel;
			break;
		case SDL_KEYUP:
			switch (event.key.keysym.sym)
			{
				// Camera Movement.
				case SDLK_w: if (camera_movement_input_.z > 0.0f) { camera_movement_input_.z = 0.0f; } break;
				case SDLK_s: if (camera_movement_input_.z < 0.0f) { camera_movement_input_.z = 0.0f; } break;
				case SDLK_a: if (camera_movement_input_.x > 0.0f) { camera_movement_input_.x = 0.0f; } break;
				case SDLK_d: if (camera_movement_input_.x < 0.0f) { camera_movement_input_.x = 0.0f; } break;
				case SDLK_SPACE: if (camera_movement_input_.y > 0.0f) { camera_movement_input_.y = 0.0f; } break;
				case SDLK_LCTRL: if (camera_movement_input_.y < 0.0f) { camera_movement_input_.y = 0.0f; } break;

				case SDLK_LSHIFT: sprint_held_ = false; break;
			}
			break;
		}
	}

	int x, y;
	SDL_GetRelativeMouseState(&x, &y);
	camera_look_input_ = glm::vec2(x, y);
}

void InputManager::PrintKeyInfo(SDL_KeyboardEvent* key)
{
	std::string output = "";

	// Press or a Release?
	if (key->type == SDL_KEYDOWN)
		output += "Press: '";
	else
		output += "Release: '";

	// Print the name of the key
	output += SDL_GetKeyName(key->keysym.sym);

	// Append key modifiers.
	Uint16 key_modifier = key->keysym.mod;
	if (key_modifier != KMOD_NONE)
	{
		output += "' | Modifier: ";

		if (key_modifier & KMOD_NUM) output += "NUMLOCK;";
		if (key_modifier & KMOD_CAPS) output += "CAPSLOCK;";
		if (key_modifier & KMOD_LCTRL) output += "LCTRL;";
		if (key_modifier & KMOD_RCTRL) output += "RCTRL;";
		if (key_modifier & KMOD_RSHIFT) output += "RSHIFT;";
		if (key_modifier & KMOD_LSHIFT) output += "LSHIFT;";
		if (key_modifier & KMOD_RALT) output += "RALT;";
		if (key_modifier & KMOD_LALT) output += "LALT;";
		if (key_modifier & KMOD_CTRL) output += "CTRL;";
		if (key_modifier & KMOD_SHIFT) output += "SHIFT;";
		if (key_modifier & KMOD_ALT) output += "ALT;";
	}
	else
	{
		output += "'";
	}

	// Output our debug text.
	std::cout << output << std::endl;
}



const glm::vec3 InputManager::get_camera_movement_input_normalized() { return SafeNormalizeVec3(camera_movement_input_); }
const glm::vec2 InputManager::get_camera_look_input() { return camera_look_input_; }
const bool InputManager::get_sprint_held() { return sprint_held_; }


const glm::vec3 InputManager::SafeNormalizeVec3(const glm::vec3& vector)
{
	if (vector == InputManager::kVector3None)
		return InputManager::kVector3None;

	return glm::normalize(vector);
}
const glm::vec2 InputManager::SafeNormalizeVec2(const glm::vec2& vector)
{
	if (vector == InputManager::kVector2None)
		return InputManager::kVector2None;

	return glm::normalize(vector);
}
#pragma once

#include <SDL\SDL.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "Display.h"
#include "InputManager.h"
#include "Shader.h"
#include "GameObject.h"
#include "Mesh.h"
#include "Texture.h"
#include "transform.h"
#include "Gradient.h"
#include "Camera.h"

#include "Skybox.h"

#include "PointLight.h"
#include "DirectionalLight.h"

#include "Cubemap.h"
#include <iostream>
#include <string>
#include <algorithm>

// File Reading.
#include <iostream>
#include <fstream>
#include <sstream>

enum class GameState {kPlay, kExit};

// Forward declarations to prevent cyclical dependency errors.
class InputManager;


// Lighting Definitions.
#define MORNING_DIRECTIONAL_LIGHT_AMBIENT glm::vec3(1.0f, 0.537f, 0.055f)
#define MIDDAY_DIRECTIONAL_LIGHT_AMBIENT glm::vec3(1.0f, 0.931f, 0.872f)
#define EVENING_DIRECTIONAL_LIGHT_AMBIENT glm::vec3(1.0f, 0.537f, 0.055f)
#define NIGHTTIME_DIRECTIONAL_LIGHT_AMBIENT glm::vec3(0.791, 0.855f, 1.0f)

#define LANTERN_LIGHT_LOCAL_POSITION glm::vec3(0.0f, 0.15f, 0.0f)


class MainGame
{
public:
	MainGame();
	~MainGame();

	void Run();

	static GameState game_state;

private:
	const glm::vec3 kMiddayLightDirection = glm::vec3(0.0f, -1.0f, 0.0f);
	const float kDayLength = 240.0f; // The length of a full day+night cycle (In seconds).
	const glm::vec3 kSunRotationAxis = glm::normalize(glm::vec3(0.0f, 0.0f, 1.0f));


	void GameLoop();
	void CalculateDeltaTime();

	void HandleCameraMovement();
	void HandleCameraLook();
	
	void RenderDepthMap_PointLights(const int& point_light_index);
	void RenderDepthMap_DirectionalLights(const int& directional_light_index);
	void DrawGame();
	void ConfigureShaders();
	void RenderScene();

	void CreateTrees();


	void CalculateLightingValues();

	inline void LogVec3(const glm::vec3& vector);


	Display game_display_;
	std::unique_ptr<InputManager> input_manager_;
	
	GameObject* ground_terrain_;
	std::vector<GameObject*> fir_trees_;

	GameObject* plane_;
	GameObject* wooden_bench_;
	GameObject* hanging_lantern_;
	GameObject* bench_lantern_;
	GameObject* floor_lantern_;

	GameObject* dir_light_object_reference_;
	std::vector<GameObject*> point_light_object_references_;
	GameObject* point_light_object_reference_0_;
	GameObject* point_light_object_reference_1_;
	GameObject* three_axies_;


	Camera* main_camera_;
	Gradient* test_gradient_;


	std::unique_ptr<Skybox> skybox_;


	// Camera Movement & Rotation.
	const float kCameraMoveSpeed = 5.0f;
	const float kCameraFastMoveSpeed = 15.0f;
	const float kCameraLookSensitivity = 50.0f;
	float x_rotation;
	float y_rotation;


	// Shaders.
	std::weak_ptr<Shader> active_shader_;

	std::shared_ptr<Shader> lighting_test_shader_;
	std::shared_ptr<Shader> terrain_shader_;
	std::shared_ptr<Shader> default_shader_;

	std::shared_ptr<Shader> depth_buffer_directional_light_shader_;
	std::shared_ptr<Shader> depth_buffer_point_light_shader_;
	std::shared_ptr<Shader> basic_shadows_;


	// Lights.
	std::array<DirectionalLight, 2> directional_lights_;
	std::array<PointLight, 3> point_lights_;

	// Day/Night Cycle.
	float day_percentage_time, day_lerp_time;
	glm::quat sun_light_dir_;
	glm::vec3 sun_diffuse_;


	const float kTimeSpeedupValue = 10.0f;
	float counter_ = 0.0f;
	float previous_time_since_start_ = 0.0f, current_time_since_start_ = 0.0f, delta_time_ = 0.0f;
};


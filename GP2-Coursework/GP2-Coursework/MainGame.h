#pragma once
#include <SDL\SDL.h>
#include <GL/glew.h>
#include "Display.h" 
#include "Shader.h"
#include "GameObject.h"
#include "Mesh.h"
#include "Texture.h"
#include "transform.h"
#include "Gradient.h"

enum class GameState {kPlay, kExit};

// Lighting Definitions.
#define MORNING_DIRECTIONAL_LIGHT_AMBIENT glm::vec3(1.0f, 0.537f, 0.055f)
#define MIDDAY_DIRECTIONAL_LIGHT_AMBIENT glm::vec3(1.0f, 0.931f, 0.872f)
#define EVENING_DIRECTIONAL_LIGHT_AMBIENT glm::vec3(1.0f, 0.537f, 0.055f)
#define NIGHTTIME_DIRECTIONAL_LIGHT_AMBIENT glm::vec3(0.791, 0.855f, 1.0f)


#define SUSANNE_1_INITIAL_POSITION glm::vec3(0.0f, 1.0f, 0.0f)


class MainGame
{
public:
	MainGame();
	~MainGame();

	void Run();

private:
	void ProcessInput();
	void GameLoop();
	void DrawGame();

	void LinkFogShader();
	void LinkRimShader();
	void LinkLightingTestsShader();

	bool Collision(glm::vec3 m1Pos, float m1Rad, glm::vec3 m2Pos, float m2Rad);
	//void playAudio(unsigned int Source, glm::vec3 pos);

	Display game_display_;
	GameState game_state_;
	GameObject* backpack_;
	//GameObject* suzanne_;
	//GameObject* suzanne_2_;
	Camera* main_camera_;
	Gradient* test_gradient_;


	Shader* active_shader_;
	Shader fog_shader_;
	Shader rim_lighting_shader_;
	Shader lighting_test_shader_;
	//Audio audioDevice;

	float counter_;
};


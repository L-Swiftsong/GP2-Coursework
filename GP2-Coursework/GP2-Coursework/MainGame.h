#pragma once
#include <SDL\SDL.h>
#include <GL/glew.h>
#include "Display.h" 
#include "Shader.h"
#include "GameObject.h"
#include "Mesh.h"
#include "Texture.h"
#include "transform.h"
#include "Audio.h"

enum class GameState {kPlay, kExit};


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

	bool Collision(glm::vec3 m1Pos, float m1Rad, glm::vec3 m2Pos, float m2Rad);
	//void playAudio(unsigned int Source, glm::vec3 pos);

	Display game_display_;
	GameState game_state_;
	GameObject suzanne_;
	GameObject suzanne_2_;
	Camera* main_camera_;


	Shader* active_shader_;
	Shader fog_shader_;
	Shader rim_lighting_shader_;
	//Audio audioDevice;

	float counter_;
};


#include "MainGame.h"
#include "Camera.h"
#include <iostream>
#include <string>


Transform transform;

MainGame::MainGame() : game_state_(GameState::kPlay),
	game_display_(Display()),
	main_camera_(new Camera(glm::vec3(0, 0, -5), 70.0f, (float)game_display_.get_screen_width() / game_display_.get_screen_height(), 0.01f, 1000.0f)),
	fog_shader_(Shader("..\\res\\Shaders\\fogShader.vert", "..\\res\\Shaders\\fogShader.frag")),
	rim_lighting_shader_(Shader("..\\res\\Shaders\\rimLighting.vert", "..\\res\\Shaders\\rimLighting.frag")),
	lighting_test_shader_(Shader("..\\res\\Shaders\\LightingTests.vert", "..\\res\\Shaders\\LightingTests.frag")),
	suzanne_(GameObject("..\\res\\monkey3.obj", "..\\res\\water.jpg", SUSANNE_1_INITIAL_POSITION, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f))),
	suzanne_2_(GameObject("..\\res\\monkey3.obj", "..\\res\\bricks.jpg", glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f))),
	counter_(1.0f)
{
	active_shader_ = &lighting_test_shader_;
}

MainGame::~MainGame()
{
	delete main_camera_;
}

void MainGame::Run()
{
	GameLoop();
}

void MainGame::GameLoop()
{
	while (game_state_ != GameState::kExit)
	{
		ProcessInput();
		DrawGame();
		Collision(suzanne_.get_mesh()->get_sphere_pos(), suzanne_.get_mesh()->get_sphere_radius(), suzanne_2_.get_mesh()->get_sphere_pos(), suzanne_2_.get_mesh()->get_sphere_radius());
		//playAudio(backGroundMusic, glm::vec3(0.0f,0.0f,0.0f));
	}
}

void MainGame::ProcessInput()
{
	SDL_Event evnt;

	while(SDL_PollEvent(&evnt)) //get and process events
	{
		switch (evnt.type)
		{
			case SDL_QUIT:
				game_state_ = GameState::kExit;
				break;
		}
	}
	
}


bool MainGame::Collision(glm::vec3 m1Pos, float m1Rad, glm::vec3 m2Pos, float m2Rad)
{
	float distance = glm::sqrt((m2Pos.x - m1Pos.x)*(m2Pos.x - m1Pos.x) + (m2Pos.y - m1Pos.y)*(m2Pos.y - m1Pos.y) + (m2Pos.z - m1Pos.z)*(m2Pos.z - m1Pos.z));

	if (distance < (m1Rad + m2Rad))
	{
		//audioDevice.setlistener(myCamera.getPos(), m1Pos); //add bool to mesh
		//playAudio(whistle, m1Pos);
		return true;
	}
	else
	{
		return false;
	}
}

//void MainGame::playAudio(unsigned int Source, glm::vec3 pos)
//{
//	
//	ALint state; 
//	alGetSourcei(Source, AL_SOURCE_STATE, &state);
//	/*
//	Possible values of state
//	AL_INITIAL
//	AL_STOPPED
//	AL_PLAYING
//	AL_PAUSED
//	*/
//	if (AL_PLAYING != state)
//	{
//		audioDevice.playSound(Source, pos);
//	}
//}

void MainGame::LinkFogShader()
{
	//fogShader.setMat4("u_pm", myCamera.getProjection());
	//fogShader.setMat4("u_vm", myCamera.getProjection());
	fog_shader_.set_float("maxDist", 20.0f);
	fog_shader_.set_float("minDist", 0.0f);
	fog_shader_.set_vec_3("fogColor", glm::vec3(0.0f, 0.0f, 0.0f));
}
void MainGame::LinkRimShader()
{
	rim_lighting_shader_.set_vec_3("cameraPos", main_camera_->get_pos());
	rim_lighting_shader_.set_vec_3("lightDir", glm::vec3(1.0, 0.0, 1.0));
	rim_lighting_shader_.set_vec_4("lightColour", glm::vec4(1.0, 1.0, 1.0, 1.0));
	rim_lighting_shader_.set_vec_4("rimColour", glm::vec4(1.0, 1.0, 1.0, 1.0));
	//rimLightingShader.setParam<glm::vec3>("lightDirection", glm::vec3(0.0f, 0.5f, 0.5f));
	//rimLightingShader.setParam("lightColour", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
}
void MainGame::LinkLightingTestsShader()
{
	lighting_test_shader_.set_vec_3("cameraPos", main_camera_->get_pos());

	// Setup the Material.
	lighting_test_shader_.set_int("material.Diffuse", 0);
	lighting_test_shader_.set_int("material.Specular", 1);
	lighting_test_shader_.set_float("material.Shininess", 32.0f);


	// Setup the Directional Lights.
	lighting_test_shader_.set_vec_3("directionalLight.Direction", 0.2f, -1.0f, -0.3f);
	lighting_test_shader_.set_vec_3("directionalLight.Ambient", 0.05f, 0.0f, 0.05f);
	lighting_test_shader_.set_vec_3("directionalLight.Diffuse", 0.4f, 0.4f, 0.4f);
	lighting_test_shader_.set_vec_3("directionalLight.Specular", 0.5f, 0.5f, 0.5f);

	// Setup the Point Lights.
	lighting_test_shader_.set_vec_3("pointLight.Position", 3.0f, 0.0f, 0.0f);
	lighting_test_shader_.set_vec_3("pointLight.Ambient", 0.5f, 1.0f, 0.5f);
	lighting_test_shader_.set_vec_3("pointLight.Diffuse", 0.75f, 0.75f, 0.75f);
	lighting_test_shader_.set_vec_3("pointLight.Specular", 1.0f, 1.0f, 1.0f);

	lighting_test_shader_.set_float("pointLight.Radius", 7.5f);
	lighting_test_shader_.set_float("pointLight.MaxIntensity", 1.0f);
	lighting_test_shader_.set_float("pointLight.Falloff", 0.5);
}

void MainGame::DrawGame()
{
	//_gameDisplay.clearDisplay(0.8f, 0.8f, 0.8f, 1.0f); //sets our background colour
	game_display_.ClearDisplay(0.0f, 0.0f, 0.0f, 1.0f); //sets our background colour

	//linkFogShader();
	//LinkRimShader();
	LinkLightingTestsShader();

	
	// Update Transform.
	suzanne_.get_transform()->set_rot(glm::vec3(0.0, counter_ * 5, 0.0));
	suzanne_.get_transform()->set_scale(glm::vec3(0.6, 0.6, 0.6));

	// Draw the GameObject.
	suzanne_.Draw(*main_camera_, active_shader_);
	

	// Update Transform.
	suzanne_2_.get_transform()->set_pos(glm::vec3(0.0, 0.0, 0.0));
	suzanne_2_.get_transform()->set_rot(glm::vec3(0.0, 0.0, counter_ * 5));
	suzanne_2_.get_transform()->set_scale(glm::vec3(0.6, 0.6, 0.6));

	// Draw the GameObject.
	suzanne_2_.Draw(*main_camera_, active_shader_);


	// Increment the Counter.
	counter_ = counter_ + 0.02f;

				
	glEnableClientState(GL_COLOR_ARRAY); 
	glEnd();

	game_display_.SwapBuffer();
} 
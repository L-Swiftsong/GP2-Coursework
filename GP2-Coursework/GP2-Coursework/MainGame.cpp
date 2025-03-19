#include "MainGame.h"
#include "Camera.h"
#include <iostream>
#include <string>


Transform transform;

MainGame::MainGame() : game_state_(GameState::kPlay),
	game_display_(Display()),
	main_camera_(new Camera(glm::vec3(0, 0, -5), 70.0f, (float)game_display_.get_screen_width() / game_display_.get_screen_height(), 0.01f, 1000.0f)),
	fog_shader_(Shader("..\\res\\fogShader.vert", "..\\res\\fogShader.frag")),
	rim_lighting_shader_(Shader("..\\res\\rimLighting.vert", "..\\res\\rimLighting.frag")),
	mesh_1_(Mesh("..\\res\\monkey3.obj")),
	mesh_2_(Mesh("..\\res\\monkey3.obj")),
	counter_(1.0f)
{
	active_shader_ = &rim_lighting_shader_;
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
		Collision(mesh_1_.get_sphere_pos(), mesh_1_.get_sphere_radius(), mesh_2_.get_sphere_pos(), mesh_2_.get_sphere_radius());
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

void MainGame::DrawGame()
{
	//_gameDisplay.clearDisplay(0.8f, 0.8f, 0.8f, 1.0f); //sets our background colour
	game_display_.ClearDisplay(0.0f, 0.0f, 0.0f, 1.0f); //sets our background colour

	//linkFogShader();
	LinkRimShader();

	Texture texture("..\\res\\bricks.jpg"); //load texture
	Texture texture1("..\\res\\water.jpg"); //load texture
	
	transform.set_pos(glm::vec3(0.0, 2.0, 0.0));
	transform.SetRot(glm::vec3(0.0, counter_ * 5, 0.0));
	transform.SetScale(glm::vec3(0.6, 0.6, 0.6));

	active_shader_->Bind();
	texture.Bind(0);
	active_shader_->Update(transform, *main_camera_);
	mesh_1_.Draw();
	mesh_1_.UpdateSphereData(*transform.get_pos(), 0.62f);
	

	//transform.SetPos(glm::vec3(-sinf(counter), -0.5, 10.0 +(-sinf(counter)*8)));
	transform.set_pos(glm::vec3(0.0, 0.0, 0.0));
	transform.SetRot(glm::vec3(0.0, 0.0, counter_ * 5));
	transform.SetScale(glm::vec3(0.6, 0.6, 0.6));

	active_shader_->Update(transform, *main_camera_);
	mesh_2_.Draw();
	mesh_2_.UpdateSphereData(*transform.get_pos(), 0.62f);
	counter_ = counter_ + 0.02f;

				
	glEnableClientState(GL_COLOR_ARRAY); 
	glEnd();

	game_display_.SwapBuffer();
} 
#include "MainGame.h"
#include "Camera.h"
#include <iostream>
#include <string>



MainGame::MainGame() : game_state_(GameState::kPlay),
	game_display_(Display()),
	main_camera_(new Camera(glm::vec3(0, 0, -10.0), 70.0f, (float)game_display_.get_screen_width() / game_display_.get_screen_height(), 0.01f, 1000.0f)),
	fog_shader_(			Shader("..\\res\\Shaders\\Tests\\fogShader.vert",		"..\\res\\Shaders\\Tests\\fogShader.frag")),
	rim_lighting_shader_(	Shader("..\\res\\Shaders\\Tests\\rimLighting.vert",		"..\\res\\Shaders\\Tests\\rimLighting.frag")),
	lighting_test_shader_(	Shader("..\\res\\Shaders\\Tests\\LightingTests.vert",	"..\\res\\Shaders\\Tests\\LightingTests.frag")),
	//suzanne_2_(GameObject("..\\res\\TestModel\\backpack.obj", glm::vec3(50.0, 0.0, 0.0), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f))),
	//suzanne_(GameObject("..\\res\\IcoSphere.obj", SUSANNE_1_INITIAL_POSITION, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f))),
	//suzanne_2_(GameObject("..\\res\\IcoSphere.obj", glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f))),
	counter_(1.0f)
{
	stbi_set_flip_vertically_on_load(true);

	active_shader_ = new Shader("..\\res\\Shaders\\Tests\\NormalMapping.vert", "..\\res\\Shaders\\Tests\\NormalMapping.frag");
	//active_shader_ = new Shader("..\\res\\Shaders\\Tests\\NormalsTest.vert", "..\\res\\Shaders\\Tests\\NormalsTest.frag");
	//backpack_ = new GameObject("..\\res\\TestModel\\backpack.obj", SUSANNE_1_INITIAL_POSITION, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f));
	backpack_ = new GameObject("..\\res\\Plane.obj", SUSANNE_1_INITIAL_POSITION, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), "brickwall.jpg", "", "brickwall_normal.jpg");


	glm::vec3 testCols[6]
	{
		MORNING_DIRECTIONAL_LIGHT_AMBIENT,
		MIDDAY_DIRECTIONAL_LIGHT_AMBIENT,
		EVENING_DIRECTIONAL_LIGHT_AMBIENT,
		NIGHTTIME_DIRECTIONAL_LIGHT_AMBIENT,
		NIGHTTIME_DIRECTIONAL_LIGHT_AMBIENT,
		MORNING_DIRECTIONAL_LIGHT_AMBIENT
	};
	float testVals[6]
	{
		0.0f, // Sunrise.
		0.2f, // Midday.
		0.4f, // Sunset.
		0.5f, // Nighttime.
		0.9f, // Nighttime.
		1.0f // Sunrise.
	};
	test_gradient_ = new Gradient(false);
	for (int i = 0; i < 6; ++i)
	{
		test_gradient_->AddStop(testVals[i], testCols[i]);
	}
}

MainGame::~MainGame()
{
	delete main_camera_;
	delete test_gradient_;
	delete active_shader_;
	delete backpack_;
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
		//Collision(suzanne_.get_mesh()->get_sphere_pos(), suzanne_.get_mesh()->get_sphere_radius(), suzanne_2_.get_mesh()->get_sphere_pos(), suzanne_2_.get_mesh()->get_sphere_radius());
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

	//glm::vec3 ambientVal = test_gradient_->GetValue(counter_ / 20.0f);
	//lighting_test_shader_.set_vec_3("directionalLight.Ambient", ambientVal.x, ambientVal.y, ambientVal.z);

	//lighting_test_shader_.set_vec_3("directionalLight.Ambient", MORNING_DIRECTIONAL_LIGHT_AMBIENT);
	lighting_test_shader_.set_vec_3("directionalLight.Ambient", MIDDAY_DIRECTIONAL_LIGHT_AMBIENT);
	//lighting_test_shader_.set_vec_3("directionalLight.Ambient", EVENING_DIRECTIONAL_LIGHT_AMBIENT);
	//lighting_test_shader_.set_vec_3("directionalLight.Ambient", NIGHTTIME_DIRECTIONAL_LIGHT_AMBIENT);

	lighting_test_shader_.set_vec_3("directionalLight.Diffuse", 0.4f, 0.4f, 0.4f);
	lighting_test_shader_.set_vec_3("directionalLight.Specular", 0.5f, 0.5f, 0.5f);

	// Setup the Point Lights.
	lighting_test_shader_.set_vec_3("pointLight.Position", 3.0f, 0.0f, 0.0f);
	lighting_test_shader_.set_vec_3("pointLight.Ambient", 0.5f, 1.0f, 0.5f);
	lighting_test_shader_.set_vec_3("pointLight.Diffuse", 0.75f, 0.75f, 0.75f);
	lighting_test_shader_.set_vec_3("pointLight.Specular", 1.0f, 1.0f, 1.0f);

	lighting_test_shader_.set_float("pointLight.Radius", 0.1f);
	lighting_test_shader_.set_float("pointLight.MaxIntensity", 1.0f);
	lighting_test_shader_.set_float("pointLight.Falloff", 0.5);
}

void MainGame::DrawGame()
{
	//_gameDisplay.clearDisplay(0.8f, 0.8f, 0.8f, 1.0f); //sets our background colour
	game_display_.ClearDisplay(0.0f, 0.0f, 0.0f, 1.0f); //sets our background colour

	//linkFogShader();
	//LinkRimShader();
	//LinkLightingTestsShader();

	
	glm::vec3 currentCameraPos = main_camera_->get_pos();
	//main_camera_->set_pos(glm::vec3(glm::sin(counter_), currentCameraPos.y, currentCameraPos.z));
	//main_camera_->RotateY(glm::radians(glm::sin(counter_)) * 0.25f);
	active_shader_->set_vec_3("viewPos", currentCameraPos);
	active_shader_->set_vec_3("lightPos", glm::vec3(5.0f, 1.0f, -5.0f));

	// Update Transform.
	backpack_->get_transform()->set_rot(glm::vec3(-90.0f, 0.0f, 0.0f));

	// Draw the GameObject.
	active_shader_->Bind();
	backpack_->Draw(*main_camera_, active_shader_);
	

	// Update Transform.
	//suzanne_2_.get_transform()->set_rot(glm::vec3(0.0, counter_, counter_));
	//suzanne_2_.get_transform()->set_scale(glm::vec3(0.6, 0.6, 0.6));

	// Draw the GameObject.
	//suzanne_2_.Draw(*main_camera_, active_shader_);


	// Increment the Counter.
	counter_ = counter_ + 0.02f;

				
	glEnableClientState(GL_COLOR_ARRAY); 
	glEnd();

	game_display_.SwapBuffer();
} 
#include "MainGame.h"


MainGame::MainGame() : game_state_(GameState::kPlay),
	game_display_(Display()),
	main_camera_(new Camera(glm::vec3(0.0f, 3.5f, 5.0f), 70.0f, (float)game_display_.get_screen_width() / game_display_.get_screen_height(), 0.01f, 1000.0f)),
	fog_shader_(			Shader("..\\res\\Shaders\\Tests\\fogShader.vert",		"..\\res\\Shaders\\Tests\\fogShader.frag")),
	rim_lighting_shader_(	Shader("..\\res\\Shaders\\Tests\\rimLighting.vert",		"..\\res\\Shaders\\Tests\\rimLighting.frag")),
	lighting_test_shader_(	Shader("..\\res\\Shaders\\Tests\\LightingTests.vert",	"..\\res\\Shaders\\Tests\\LightingTests.frag")),

	//skybox_(std::make_unique<Skybox>(Skybox("..\\res\\Skyboxes\\TestSky", ".jpg"))),
	skybox_shader_(Shader("..\\res\\Shaders\\Tests\\SkyboxTest.vert", "..\\res\\Shaders\\Tests\\SkyboxTest.frag")),
	skybox_texture_(Cubemap::CreateCubemapTexture("..\\res\\Skyboxes\\TestSky", ".jpg")),

	sun_light_dir_(glm::quat(glm::vec3(0.0f, 1.0f, 0.0f))),
	sun_diffuse_(MIDDAY_DIRECTIONAL_LIGHT_AMBIENT),
	counter_(0.0f)
{
	//stbi_set_flip_vertically_on_load(true);

	active_shader_ = std::make_unique<Shader>(lighting_test_shader_);//new Shader("..\\res\\Shaders\\Tests\\NormalMapping.vert", "..\\res\\Shaders\\Tests\\NormalMapping.frag");
	//active_shader_ = new Shader("..\\res\\Shaders\\Tests\\NormalsTest.vert", "..\\res\\Shaders\\Tests\\NormalsTest.frag");
	//backpack_ = new GameObject("..\\res\\TestModel\\backpack.obj", SUSANNE_1_INITIAL_POSITION, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f));
	wooden_bench_ = new GameObject("..\\res\\Models\\Bench\\WoodenBench.obj", glm::vec3(0.0f), glm::vec3(0.0f), glm::vec3(2.0f));
	main_camera_->get_transform()->set_euler_angles(ToRadians(glm::vec3(10.0f, 180.0f, 0.0f)));

	dir_light_object_reference_ = new GameObject("..\\res\\IcoSphere.obj", glm::vec3(0.0f, -0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), "brickwall.jpg", "", "brickwall_normal.jpg");


	std::vector<std::tuple<float, glm::vec3>> directional_light_values
	{
		std::make_tuple(0.25f, MIDDAY_DIRECTIONAL_LIGHT_AMBIENT),		// Midday.
		std::make_tuple(0.45f, EVENING_DIRECTIONAL_LIGHT_AMBIENT),		// Sunrise/Sunset.
		std::make_tuple(0.55f, NIGHTTIME_DIRECTIONAL_LIGHT_AMBIENT),	// Nighttime.
	};
	test_gradient_ = new Gradient(true, directional_light_values);



	unsigned int skybox_ebo;
	glGenVertexArrays(1, &skybox_vao);
	glGenBuffers(1, &skybox_vbo);
	glGenBuffers(1, &skybox_ebo);
	glBindVertexArray(skybox_vao);
	glBindBuffer(GL_ARRAY_BUFFER, skybox_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, skybox_ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(skyboxIndices), &skyboxIndices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

MainGame::~MainGame()
{
	delete main_camera_;
	delete test_gradient_;
	delete backpack_;
	delete wooden_bench_;
	delete dir_light_object_reference_;
}

void MainGame::Run()
{
	glfwInit();
	GameLoop();
}

void MainGame::GameLoop()
{
	previous_time_since_start_ = glfwGetTime();
	while (game_state_ != GameState::kExit)
	{
		CalculateDeltaTime();
		ProcessInput();
		DrawGame();
		//Collision(suzanne_.get_mesh()->get_sphere_pos(), suzanne_.get_mesh()->get_sphere_radius(), suzanne_2_.get_mesh()->get_sphere_pos(), suzanne_2_.get_mesh()->get_sphere_radius());
		//playAudio(backGroundMusic, glm::vec3(0.0f,0.0f,0.0f));
	}

	glfwTerminate();
}

void MainGame::CalculateDeltaTime()
{
	current_time_since_start_ = glfwGetTime();
	delta_time_ = current_time_since_start_ - previous_time_since_start_;
	previous_time_since_start_ = current_time_since_start_;
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
	rim_lighting_shader_.set_vec_3("cameraPos", main_camera_->get_transform()->get_pos());
	rim_lighting_shader_.set_vec_3("lightDir", glm::vec3(1.0, 0.0, 1.0));
	rim_lighting_shader_.set_vec_4("lightColour", glm::vec4(1.0, 1.0, 1.0, 1.0));
	rim_lighting_shader_.set_vec_4("rimColour", glm::vec4(1.0, 1.0, 1.0, 1.0));
	//rimLightingShader.setParam<glm::vec3>("lightDirection", glm::vec3(0.0f, 0.5f, 0.5f));
	//rimLightingShader.setParam("lightColour", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
}
void MainGame::LinkLightingTestsShader()
{
	lighting_test_shader_.set_vec_3("cameraPos", main_camera_->get_transform()->get_pos());


	// Setup the Directional Lights.
	//lighting_test_shader_.set_vec_3("directionalLight.Direction", kMiddayLightDirection * sun_light_dir_);
	//lighting_test_shader_.set_vec_3("directionalLight.Ambient", glm::vec3(0.0f));
	//lighting_test_shader_.set_vec_3("directionalLight.Diffuse", sun_diffuse_ * 0.8f);
	//lighting_test_shader_.set_vec_3("directionalLight.Specular", sun_diffuse_);

	lighting_test_shader_.set_vec_3("directionalLight.Direction", glm::vec3(-1.0f, -1.0f, -1.0f));
	lighting_test_shader_.set_vec_3("directionalLight.Ambient", glm::vec3(0.0f));
	lighting_test_shader_.set_vec_3("directionalLight.Diffuse", glm::vec3(0.8f));
	lighting_test_shader_.set_vec_3("directionalLight.Specular", glm::vec3(1.0f));

	// Setup the Point Lights.
	lighting_test_shader_.set_vec_3("pointLight.Position", 0.0f, 1.0f, 0.0f);
	lighting_test_shader_.set_vec_3("pointLight.Diffuse", glm::vec3(1.0f));

	lighting_test_shader_.set_float("pointLight.Radius", 3.0f);
	lighting_test_shader_.set_float("pointLight.MaxIntensity", 2.0f);
	lighting_test_shader_.set_float("pointLight.Falloff", 0.5);
}

void MainGame::DrawGame()
{
	game_display_.ClearDisplay(0.0f, 0.0f, 0.0f, 1.0f); // Sets our background colour.

	CalculateLightingValues();

	//linkFogShader();
	//LinkRimShader();
	LinkLightingTestsShader();

	//glm::vec3 currentCameraPos = main_camera_->get_transform()->get_pos();
	//main_camera_->set_pos(glm::vec3(glm::sin(counter_), currentCameraPos.y, currentCameraPos.z));
	//main_camera_->RotateY(glm::radians(glm::sin(counter_)) * 0.25f);
	//active_shader_->set_vec_3("viewPos", currentCameraPos);
	//active_shader_->set_vec_3("lightPos", glm::vec3(5.0f, 1.0f, 5.0f));

	// Update Transform.
	//backpack_->get_transform()->set_rot(glm::vec3(-90.0f, 0.0f, 0.0f));

	// Draw the GameObject.
	active_shader_->Bind();
	wooden_bench_->Draw(*main_camera_, active_shader_.get());
	dir_light_object_reference_->Draw(*main_camera_, active_shader_.get());


	//skybox_->Draw(*main_camera_);
	DrawSkybox();


	// Increment the Counter.
	counter_ = counter_ + (1.0f * delta_time_);

	glEnableClientState(GL_COLOR_ARRAY); 
	glEnd();

	game_display_.SwapBuffer();
}


void MainGame::CalculateLightingValues()
{
	float scaled_counter = counter_ / kDayLength;

	// Calculate our sunlight/skybox colour.
	float time = (-std::cos(glm::radians(scaled_counter * 360.0f)) + 1.0f) / 2.0f;
	sun_diffuse_ = test_gradient_->get_value(time);

	// Calculate our sunlight direction.
	float desired_angle = std::fmod((360.0f * scaled_counter), 360.0f);
	sun_light_dir_ = glm::quat(ToRadians(kSunRotationAxis * desired_angle));


	// (Debug) Display our sunlight direction.
	glm::vec3 desired_direction = -kMiddayLightDirection * sun_light_dir_;
	dir_light_object_reference_->get_transform()->set_pos(desired_direction * 5.0f); // Shows the direction the light is shining FROM.

	// (Debug) Set the background colour to match our sunlight colour (Will later replace when doing Skybox stuff).
	//game_display_.ClearDisplay(sun_diffuse_.x, sun_diffuse_.y, sun_diffuse_.z, 1.0f);
}

void MainGame::DrawSkybox()
{
	glDepthFunc(GL_LEQUAL);

	skybox_shader_.Bind();
	glm::mat4 view = glm::mat4(glm::mat3(glm::lookAt(main_camera_->get_transform()->get_pos(), main_camera_->get_transform()->get_pos() + main_camera_->get_transform()->get_forward(), main_camera_->get_transform()->get_up())));
	glm::mat4 projection = main_camera_->get_projection();

	glUniformMatrix4fv(glGetUniformLocation(skybox_shader_.get_shader_id(), "view"), 1, GL_FALSE, &view[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(skybox_shader_.get_shader_id(), "projection"), 1, GL_FALSE, &projection[0][0]);


	glBindVertexArray(skybox_vao);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, skybox_texture_.get_texture_id());
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);

	glDepthFunc(GL_LESS);
}



inline glm::vec3 MainGame::ToDegrees(const glm::vec3& vector)
{
	return glm::vec3(glm::degrees(vector.x), glm::degrees(vector.y), glm::degrees(vector.z));
}
inline glm::vec3 MainGame::ToRadians(const glm::vec3& vector)
{
	return glm::vec3(glm::radians(vector.x), glm::radians(vector.y), glm::radians(vector.z));
}
inline void MainGame::LogVec3(const glm::vec3& vector)
{
	std::cout << vector.x << "," << vector.y << "," << vector.z << std::endl;
}

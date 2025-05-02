#include "MainGame.h"


MainGame::MainGame() : game_state_(GameState::kPlay),
	game_display_(Display()),
	input_manager_(std::make_unique<InputManager>()),
	main_camera_(new Camera(glm::vec3(0.0f, 3.5f, 5.0f), 70.0f, (float)game_display_.get_screen_width() / game_display_.get_screen_height(), 0.01f, 1000.0f)),
	fog_shader_(			Shader("..\\res\\Shaders\\Tests\\fogShader.vert",		"..\\res\\Shaders\\Tests\\fogShader.frag")),
	rim_lighting_shader_(	Shader("..\\res\\Shaders\\Tests\\rimLighting.vert",		"..\\res\\Shaders\\Tests\\rimLighting.frag")),
	lighting_test_shader_(	Shader("..\\res\\Shaders\\Tests\\LightingTests.vert",	"..\\res\\Shaders\\Tests\\LightingTests.frag")),

	//skybox_(std::make_unique<Skybox>("..\\res\\Skyboxes\\TestSky", ".jpg")),
	skybox_(std::make_unique<Skybox>("..\\res\\Skyboxes\\PolyverseSkies-NightSky", ".jpg")),

	sun_light_dir_(glm::quat(glm::vec3(0.0f, 1.0f, 0.0f))),
	sun_diffuse_(MIDDAY_DIRECTIONAL_LIGHT_AMBIENT),
	counter_(0.0f)
{
	//stbi_set_flip_vertically_on_load(true);

	active_shader_ = std::make_unique<Shader>(lighting_test_shader_);

	// Setup Models.
	wooden_bench_ = new GameObject("..\\res\\Models\\Bench\\WoodenBench.obj", glm::vec3(0.0f), glm::vec3(0.0f), glm::vec3(2.0f));
	dir_light_object_reference_ = new GameObject("..\\res\\IcoSphere.obj", glm::vec3(0.0f, -0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), "brickwall.jpg", "", "brickwall_normal.jpg");

	main_camera_->get_transform()->set_euler_angles(ToRadians(glm::vec3(0.0f, 180.0f, 0.0f)));


	// Setup Lights.
	directional_lights_[0] = DirectionalLight(kMiddayLightDirection, MIDDAY_DIRECTIONAL_LIGHT_AMBIENT, 1.0f);
	point_lights_[0] = PointLight(glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(1.0f), 3.0f, 1.0f, 0.5f);


	std::vector<std::tuple<float, glm::vec3>> directional_light_values
	{
		std::make_tuple(0.25f, MIDDAY_DIRECTIONAL_LIGHT_AMBIENT),		// Midday.
		std::make_tuple(0.45f, EVENING_DIRECTIONAL_LIGHT_AMBIENT),		// Sunrise/Sunset.
		std::make_tuple(0.55f, NIGHTTIME_DIRECTIONAL_LIGHT_AMBIENT),	// Nighttime.
	};
	test_gradient_ = new Gradient(true, directional_light_values);
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
		input_manager_->ProcessInput();
		
		HandleCameraMovement();
		HandleCameraLook();

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


void MainGame::HandleCameraMovement()
{
	Transform* camera_transform = main_camera_->get_transform();

	// Calculate Current Frame's Movement.
	glm::vec3 camera_movement = input_manager_->get_camera_movement_input_normalized();
	camera_movement = camera_movement * main_camera_->get_transform()->get_rot();
	camera_movement *= (input_manager_->get_sprint_held() ? kCameraFastMoveSpeed : kCameraMoveSpeed) * delta_time_;

	// Apply Camera Movement.
	main_camera_->get_transform()->set_pos(main_camera_->get_transform()->get_pos() + camera_movement);
}
void MainGame::HandleCameraLook()
{
	Transform* camera_transform = main_camera_->get_transform();

	// Calculate Desired Rotation.
	glm::vec2 camera_look = input_manager_->get_camera_look_input();
	camera_look *= kCameraLookSensitivity * delta_time_;

	y_rotation += camera_look.x;

	x_rotation -= camera_look.y;
	if (x_rotation < -90.0f)
		x_rotation = -90.0f;
	else if (x_rotation > 90.0f)
		x_rotation = 90.0f;


	// Apply our rotation.
	// Note: For some reason we are having errors if we don't reset our rotation before applying our current.
	camera_transform->set_euler_angles(0.0f, 0.0f, 0.0f);
	camera_transform->Rotate(glm::vec3(1.0f, 0.0f, 0.0f), glm::radians(x_rotation), Transform::kLocalSpace);
	camera_transform->Rotate(glm::vec3(0.0f, 1.0f, 0.0f), glm::radians(y_rotation), Transform::kWorldSpace);
}


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
	for (int i = 0; i < directional_lights_.size(); ++i)
	{
		directional_lights_[i].UpdateShader(lighting_test_shader_);
	}

	// Setup the Point Lights.
	for (int i = 0; i < point_lights_.size(); ++i)
	{
		point_lights_[i].UpdateShader(lighting_test_shader_);
	}
}

void MainGame::DrawGame()
{
	game_display_.ClearDisplay(0.0f, 0.0f, 0.0f, 1.0f); // Sets our background colour.

	CalculateLightingValues();

	//linkFogShader();
	//LinkRimShader();
	LinkLightingTestsShader();


	// Draw the skybox (Done before other objects as for some reason doing it after causes other shaders to fail).
	skybox_->Draw(*main_camera_, day_lerp_time);


	// Draw the GameObject.
	active_shader_->Bind();
	wooden_bench_->Draw(*main_camera_, active_shader_.get());
	dir_light_object_reference_->Draw(*main_camera_, active_shader_.get());


	// Increment the Counter.
	counter_ = counter_ + (1.0f * delta_time_);

	glEnableClientState(GL_COLOR_ARRAY); 
	glEnd();

	game_display_.SwapBuffer();
}


void MainGame::CalculateLightingValues()
{
	float scaled_counter = counter_ / kDayLength;
	day_percentage_time = scaled_counter - std::floorf(scaled_counter);
	day_lerp_time = (-std::cos(glm::radians(day_percentage_time * 360.0f)) + 1.0f) / 2.0f;

	// Calculate our sunlight/skybox colour.
	sun_diffuse_ = test_gradient_->get_value(day_lerp_time);

	// Calculate our sunlight direction.
	float desired_angle = std::fmod((360.0f * day_percentage_time), 360.0f);
	sun_light_dir_ = glm::quat(ToRadians(kSunRotationAxis * desired_angle));

	// Update the sun directional light.
	directional_lights_[0].set_direction(kMiddayLightDirection * sun_light_dir_);
	directional_lights_[0].set_diffuse(sun_diffuse_);


	// (Debug) Display our sunlight direction.
	glm::vec3 desired_direction = -kMiddayLightDirection * sun_light_dir_;
	dir_light_object_reference_->get_transform()->set_pos(desired_direction * 5.0f); // Shows the direction the light is shining FROM.
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

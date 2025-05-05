#include "MainGame.h"


MainGame::MainGame() : game_state_(GameState::kPlay),
	game_display_(Display()),
	input_manager_(std::make_unique<InputManager>()),
	main_camera_(new Camera(glm::vec3(0.0f, 3.5f, 0.0f), 70.0f, (float)game_display_.get_screen_width() / game_display_.get_screen_height(), 0.01f, 1000.0f)),
	depth_buffer_shader_(	Shader("..\\res\\Shaders\\RenderToDepth.vert",	"..\\res\\Shaders\\RenderToDepth.frag")),
	lighting_test_shader_(	Shader("..\\res\\Shaders\\Tests\\LightingTests.vert",	"..\\res\\Shaders\\Tests\\LightingTests.frag")),
	terrain_shader_(		Shader("..\\res\\Shaders\\Tests\\TerrainTexture.vert",	"..\\res\\Shaders\\Tests\\TerrainTexture.frag")),
	default_shader_(		Shader("..\\res\\Shaders\\DefaultTexture.vert", "..\\res\\Shaders\\DefaultTexture.frag")),
	basic_shadows_(			Shader("..\\res\\Shaders\\Tests\\BasicShadows.vert", "..\\res\\Shaders\\Tests\\BasicShadows.frag")),

	//skybox_(std::make_unique<Skybox>("..\\res\\Skyboxes\\TestSky", ".jpg")),
	skybox_(std::make_unique<Skybox>("..\\res\\Skyboxes\\PolyverseSkies-NightSky", ".jpg")),

	sun_light_dir_(glm::quat(glm::vec3(0.0f, 1.0f, 0.0f))),
	sun_diffuse_(MIDDAY_DIRECTIONAL_LIGHT_AMBIENT),
	counter_(0.0f)
{
	//stbi_set_flip_vertically_on_load(true);

	active_shader_ = std::make_unique<Shader>(basic_shadows_);
	//active_shader_ = std::make_unique<Shader>("..\\res\\Shaders\\Tests\\NormalMapping.vert", "..\\res\\Shaders\\Tests\\NormalMapping.frag");

	// Setup Models.
	ground_terrain_ = new GameObject("..\\res\\Models\\Terrain\\NewMeshTerrain.obj", glm::vec3(500.0f, 0.0f, -500.0f), glm::radians(glm::vec3(0.0f)), glm::vec3(1.0f),
		std::vector<std::string>{"..\\res\\Models\\Terrain\\Texture_Grass_Diffuse.png", "..\\res\\Models\\Terrain\\Texture_Dirt_Diffuse.png", "..\\res\\Models\\Terrain\\Texture_Rock_Diffuse.png", "..\\res\\Models\\Terrain\\SplatAlpha 0.png"},
		std::vector<std::string>{},
		std::vector<std::string>{"..\\res\\Models\\Terrain\\Texture_Grass_Normal.png", "..\\res\\Models\\Terrain\\Texture_Dirt_Normal.png", "..\\res\\Models\\Terrain\\Texture_Rock_Normal.png"});
	fir_tree_ = new GameObject("..\\res\\Models\\Trees\\LowPolyFirTree.obj", glm::vec3(0.0f, 0.0f, -5.0f), glm::radians(glm::vec3(0.0f)), glm::vec3(1.0f));

	plane_ = new GameObject("..\\res\\Plane.obj", glm::vec3(0.0f), glm::radians(glm::vec3(0.0f, 0.0f, 0.0f)), glm::vec3(1.0f), "brickwall.jpg", "", "brickwall_normal.jpg");
	wooden_bench_ = new GameObject("..\\res\\Models\\Bench\\WoodenBench.obj", glm::vec3(0.0f), glm::vec3(0.0f), glm::vec3(1.0f));
	dir_light_object_reference_ = new GameObject("..\\res\\IcoSphere.obj", glm::vec3(0.0f, 0.0f, 2.0f), glm::vec3(0.0f), glm::vec3(0.25f), "brickwall.jpg", "", "brickwall_normal.jpg");
	three_axies_ = new GameObject("..\\res\\Models\\ThreeAxies.obj", glm::vec3(0.0f), glm::vec3(0.0f), glm::vec3(0.25f));

	main_camera_->get_transform()->set_euler_angles(ToRadians(glm::vec3(0.0f, 180.0f, 0.0f)));


	// Setup Lights.
	directional_lights_[0] = DirectionalLight(kMiddayLightDirection, MIDDAY_DIRECTIONAL_LIGHT_AMBIENT, 1.0f);
	point_lights_[0] = PointLight(glm::vec3(0.0f, 0.25f, 0.0f), glm::vec3(1.0f), 3.0f, 1.0f, 0.5f);


	std::vector<std::tuple<float, glm::vec3>> directional_light_values
	{
		std::make_tuple(0.25f, MIDDAY_DIRECTIONAL_LIGHT_AMBIENT),		// Midday.
		std::make_tuple(0.45f, EVENING_DIRECTIONAL_LIGHT_AMBIENT),		// Sunrise/Sunset.
		std::make_tuple(0.55f, NIGHTTIME_DIRECTIONAL_LIGHT_AMBIENT),	// Nighttime.
	};
	test_gradient_ = new Gradient(true, directional_light_values);

	InitialiseShadowMap();
}
MainGame::~MainGame()
{
	delete main_camera_;
	delete test_gradient_;
	delete ground_terrain_;
	delete fir_tree_;
	delete plane_;
	delete wooden_bench_;
	delete dir_light_object_reference_;
	delete three_axies_;
}

void MainGame::InitialiseShadowMap()
{
	// Generate our Framebuffer.
	glGenFramebuffers(1, &depth_map_fbo);

	// Generate the Depth Buffer Texture.
	depth_map;
	glGenTextures(1, &depth_map);
	glBindTexture(GL_TEXTURE_2D, depth_map);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, kShadowTextureWidth, kShadowTextureHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	float border_color[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, border_color);

	// Set the Framebuffer's Depth Buffer as our generated texture.
	glBindFramebuffer(GL_FRAMEBUFFER, depth_map_fbo);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depth_map, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
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

		// Process then Handle Input.
		input_manager_->ProcessInput();
		HandleCameraMovement();
		HandleCameraLook();

		CalculateLightingValues();

		RenderDepthMap();
		DrawGame();
		//Collision(suzanne_.get_mesh()->get_sphere_pos(), suzanne_.get_mesh()->get_sphere_radius(), suzanne_2_.get_mesh()->get_sphere_pos(), suzanne_2_.get_mesh()->get_sphere_radius());
		//playAudio(backGroundMusic, glm::vec3(0.0f,0.0f,0.0f));


		// Increment the Counter.
		counter_ = counter_ + (1.0f * delta_time_);
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
	camera_transform->set_pos(main_camera_->get_transform()->get_pos() + camera_movement);
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
		//point_lights_[i].UpdateShader(lighting_test_shader_);
	}
}


void MainGame::RenderDepthMap()
{
	glViewport(0, 0, kShadowTextureWidth, kShadowTextureHeight);
	glBindFramebuffer(GL_FRAMEBUFFER, depth_map_fbo);
	glClear(GL_DEPTH_BUFFER_BIT);

	// Determine matrices for rendering from the light's point of view.
	glm::mat4 light_projection, light_view, light_space_matrix;
	const float kNearPlane = 0.1f, kFarPlane = 1000.0f; // Change to our camera's near & far clipping planes?
	light_projection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, kNearPlane, kFarPlane);
	light_view = glm::lookAt(directional_lights_[0].get_direction() * -7.5f, glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	light_space_matrix = light_projection * light_view;

	depth_buffer_shader_.Bind();
	depth_buffer_shader_.set_mat_4("light_space_matrix", light_space_matrix);

	// Draw all our GameObjects.
	ground_terrain_->Draw(*main_camera_, &depth_buffer_shader_);

	fir_tree_->Draw(*main_camera_, &depth_buffer_shader_);
	plane_->Draw(*main_camera_, &depth_buffer_shader_);
	wooden_bench_->Draw(*main_camera_, &depth_buffer_shader_);
	dir_light_object_reference_->Draw(*main_camera_, &depth_buffer_shader_);
	three_axies_->Draw(*main_camera_, &depth_buffer_shader_);
	

	basic_shadows_.set_mat_4("light_space_matrix", light_space_matrix);
	Mesh::s_shadows_depth_map = depth_map;

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
void MainGame::DrawGame()
{
	game_display_.ClearDisplay(0.0f, 0.0f, 0.0f, 1.0f); // Sets our background colour.


	ConfigureShaders();
	RenderScene();

	
	glEnableClientState(GL_COLOR_ARRAY); 
	glEnd();

	game_display_.SwapBuffer();
}

void MainGame::ConfigureShaders()
{
	LinkLightingTestsShader();

	terrain_shader_.set_vec_3("light_dir", directional_lights_[0].get_direction());
	terrain_shader_.set_vec_3("view_pos", main_camera_->get_transform()->get_pos());

	basic_shadows_.set_vec_3("light_pos", directional_lights_[0].get_direction() * -7.5f);
	basic_shadows_.set_vec_3("view_pos", main_camera_->get_transform()->get_pos());
}
void MainGame::RenderScene()
{
	// Draw all our GameObjects.
	//ground_terrain_->Draw(*main_camera_, &terrain_shader_);
	ground_terrain_->Draw(*main_camera_, active_shader_.get());

	fir_tree_->Draw(*main_camera_, active_shader_.get());
	plane_->Draw(*main_camera_, active_shader_.get());
	wooden_bench_->Draw(*main_camera_, active_shader_.get());
	dir_light_object_reference_->Draw(*main_camera_, active_shader_.get());
	three_axies_->Draw(*main_camera_, &default_shader_);


	// Draw the skybox.
	skybox_->Draw(*main_camera_, day_lerp_time);
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
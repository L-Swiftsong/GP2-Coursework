#include "MainGame.h"


MainGame::MainGame() : game_state_(GameState::kPlay),
	game_display_(Display()),
	input_manager_(std::make_unique<InputManager>()),
	main_camera_(new Camera(glm::vec3(0.0f, 3.5f, 0.0f), 70.0f, (float)game_display_.get_screen_width() / game_display_.get_screen_height(), 0.01f, 1000.0f)),
	depth_buffer_directional_light_shader_(	Shader("..\\res\\Shaders\\DepthRendering\\RenderToDepth_DirectionalLight.vert",	"..\\res\\Shaders\\DepthRendering\\RenderToDepth_DirectionalLight.frag")),
	depth_buffer_point_light_shader_(	Shader("..\\res\\Shaders\\DepthRendering\\RenderToDepth_PointLight.vert",	"..\\res\\Shaders\\DepthRendering\\RenderToDepth_PointLight.frag", "..\\res\\Shaders\\DepthRendering\\RenderToDepth_PointLight.gs")),
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

	active_shader_ = std::make_unique<Shader>(lighting_test_shader_);
	//active_shader_ = std::make_unique<Shader>("..\\res\\Shaders\\Tests\\NormalMapping.vert", "..\\res\\Shaders\\Tests\\NormalMapping.frag");

	// Setup Models.
	ground_terrain_ = new GameObject("..\\res\\Models\\Terrain\\NewMeshTerrain.obj", glm::vec3(500.0f, 0.0f, -500.0f), glm::radians(glm::vec3(0.0f)), glm::vec3(1.0f),
		std::vector<std::string>{"..\\res\\Models\\Terrain\\Texture_Grass_Diffuse.png", "..\\res\\Models\\Terrain\\Texture_Dirt_Diffuse.png", "..\\res\\Models\\Terrain\\Texture_Rock_Diffuse.png", "..\\res\\Models\\Terrain\\SplatAlpha 0.png"},
		std::vector<std::string>{},
		std::vector<std::string>{"..\\res\\Models\\Terrain\\Texture_Grass_Normal.png", "..\\res\\Models\\Terrain\\Texture_Dirt_Normal.png", "..\\res\\Models\\Terrain\\Texture_Rock_Normal.png"});
	//fir_tree_ = new GameObject("..\\res\\Models\\Trees\\LowPolyFirTree.obj", glm::vec3(0.0f, 0.0f, -5.0f), glm::radians(glm::vec3(0.0f)), glm::vec3(1.0f));
	fir_tree_ = new GameObject("..\\res\\Models\\Trees\\LowPolyFirTree.obj", glm::vec3(0.0f, 0.0f, -5.0f), glm::radians(glm::vec3(0.0f)), glm::vec3(1.0f), "..\\res\\Models\\Trees\\LowPolyFirTree_Diffuse.png", "", "");

	plane_ = new GameObject("..\\res\\Plane.obj", glm::vec3(0.0f), glm::radians(glm::vec3(0.0f, 0.0f, 0.0f)), glm::vec3(1.0f), "..\\res\\brickwall.jpg", "", "..\\res\\brickwall_normal.jpg");
	wooden_bench_ = new GameObject("..\\res\\Models\\Bench\\WoodenBench.obj", glm::vec3(0.0f), glm::vec3(0.0f), glm::vec3(1.0f));
	dir_light_object_reference_ = new GameObject("..\\res\\IcoSphere.obj", glm::vec3(0.0f, 0.0f, 2.0f), glm::vec3(0.0f), glm::vec3(0.25f), "..\\res\\brickwall.jpg", "", "..\\res\\brickwall_normal.jpg");
	point_light_object_reference_0_ = new GameObject("..\\res\\IcoSphere.obj", glm::vec3(0.0f, 0.0f, 2.0f), glm::vec3(0.0f), glm::vec3(0.25f), "..\\res\\brickwall.jpg", "", "..\\res\\brickwall_normal.jpg");
	point_light_object_reference_1_ = new GameObject("..\\res\\IcoSphere.obj", glm::vec3(0.0f, 0.0f, 2.0f), glm::vec3(0.0f), glm::vec3(0.25f), "..\\res\\brickwall.jpg", "", "..\\res\\brickwall_normal.jpg");
	three_axies_ = new GameObject("..\\res\\Models\\ThreeAxies.obj", glm::vec3(0.0f), glm::vec3(0.0f), glm::vec3(0.25f));

	main_camera_->get_transform()->set_euler_angles(ToRadians(glm::vec3(0.0f, 180.0f, 0.0f)));


	// Setup Lights.
	directional_lights_[0] = DirectionalLight(kMiddayLightDirection, MIDDAY_DIRECTIONAL_LIGHT_AMBIENT, 1.0f);
	Mesh::s_shadows_depth_maps = std::vector<int>(directional_lights_.size(), -1);

	point_lights_[0] = PointLight(glm::vec3(0.0f, 1.5f, 0.0f), glm::vec3(1.0f), 3.0f, 1.0f, 0.5f);
	point_lights_[1] = PointLight(glm::vec3(0.0f, 1.5f, 0.0f), glm::vec3(1.0f), 3.0f, 1.0f, 0.5f);
	Mesh::s_shadows_depth_cubemaps = std::vector<int>(point_lights_.size(), -1);


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
	/*// Generate our Framebuffer.
	glGenFramebuffers(1, &depth_map_fbo_);
	glGenFramebuffers(1, &depth_cubemap_fbo_);

	// ----- Texture 2D (Directional Lights) -----
	// Generate the Depth Buffer Texture.
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
	glBindFramebuffer(GL_FRAMEBUFFER, depth_map_fbo_);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depth_map, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);


	// ----- Cubemap (Point Lights) -----
	// Generate the Depth Buffer Texture.
	glGenTextures(1, &depth_cubemap_);
	glBindTexture(GL_TEXTURE_CUBE_MAP, depth_cubemap_);
	for(unsigned int i = 0; i < 6; ++i)
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT, kShadowTextureWidth, kShadowTextureHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	// Set the Framebuffer's Depth Buffer as our generated texture.
	glBindFramebuffer(GL_FRAMEBUFFER, depth_cubemap_fbo_);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depth_cubemap_, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);*/
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
		point_lights_[0].set_position(glm::vec3(0.0f, 1.5f, glm::sin(counter_ / 2.0f) * 2.0f));
		point_lights_[1].set_position(glm::vec3(glm::sin(counter_ / 2.0f) * 2.0f, 1.5f, 0.0f));
		point_light_object_reference_0_->get_transform()->set_pos(point_lights_[0].get_position());
		point_light_object_reference_1_->get_transform()->set_pos(point_lights_[1].get_position());

		// Render our depth maps.
		for(unsigned int i = 0; i < point_lights_.size(); ++i)
			RenderDepthMap_PointLights(i);
		for(unsigned int i = 0; i < directional_lights_.size(); ++i)
			RenderDepthMap_DirectionalLights(i);

		// Draw the scene.
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
	/*lighting_test_shader_.set_vec_3("cameraPos", main_camera_->get_transform()->get_pos());


	// Setup the Directional Lights.
	for (int i = 0; i < directional_lights_.size(); ++i)
	{
		//directional_lights_[i].UpdateShader(lighting_test_shader_);
	}

	// Setup the Point Lights.
	for (int i = 0; i < point_lights_.size(); ++i)
	{
		//point_lights_[i].UpdateShader(lighting_test_shader_);
	}*/
}


void MainGame::RenderDepthMap_PointLights(const int& point_light_index)
{
	glViewport(0, 0, Light::kShadowTextureWidth, Light::kShadowTextureHeight);
	glBindFramebuffer(GL_FRAMEBUFFER, point_lights_[point_light_index].shadow_map_fbo);
	glClear(GL_DEPTH_BUFFER_BIT);
	glCullFace(GL_FRONT);

	// Determine matrices for rendering from the light's point of view.
	const float kAspect = (float)Light::kShadowTextureWidth / (float)Light::kShadowTextureHeight;
	glm::mat4 shadow_projection_matrix = glm::perspective(glm::radians(90.0f), kAspect, main_camera_->get_near_clip(), main_camera_->get_far_clip());
	glm::vec3 light_pos = point_lights_[point_light_index].get_position();
	std::vector<glm::mat4> shadow_transforms
	{
		shadow_projection_matrix * glm::lookAt(light_pos, light_pos + glm::vec3(1.0f, 0.0f, 0.0f),	glm::vec3(0.0f, -1.0f, 0.0f)), // Left.
		shadow_projection_matrix * glm::lookAt(light_pos, light_pos + glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)), // Right.
		shadow_projection_matrix * glm::lookAt(light_pos, light_pos + glm::vec3(0.0f, 1.0f, 0.0f),	glm::vec3(0.0f, 0.0f, 1.0f)), // Up/Top.
		shadow_projection_matrix * glm::lookAt(light_pos, light_pos + glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f)), // Down/Bottom.
		shadow_projection_matrix * glm::lookAt(light_pos, light_pos + glm::vec3(0.0f, 0.0f, 1.0f),	glm::vec3(0.0f, -1.0f, 0.0f)), // Forward.
		shadow_projection_matrix * glm::lookAt(light_pos, light_pos + glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f)), // Back.
	};


	for (unsigned int i = 0; i < 6; ++i)
	{
		depth_buffer_point_light_shader_.set_mat_4("shadow_matrices[" + std::to_string(i) + "]", shadow_transforms[i]);
	}
	depth_buffer_point_light_shader_.set_float("far_plane", main_camera_->get_far_clip());
	depth_buffer_point_light_shader_.set_vec_3("light_pos", point_lights_[point_light_index].get_position());


	// Draw all our GameObjects.
	ground_terrain_->Draw(*main_camera_, &depth_buffer_point_light_shader_);

	fir_tree_->Draw(*main_camera_, &depth_buffer_point_light_shader_);
	//plane_->Draw(*main_camera_, &depth_buffer_point_light_shader_);
	wooden_bench_->Draw(*main_camera_, &depth_buffer_point_light_shader_);
	dir_light_object_reference_->Draw(*main_camera_, &depth_buffer_point_light_shader_);
	//point_light_object_reference_->Draw(*main_camera_, &depth_buffer_point_light_shader_);
	three_axies_->Draw(*main_camera_, &depth_buffer_point_light_shader_);
	

	Mesh::s_shadows_depth_cubemaps[point_light_index] = point_lights_[point_light_index].shadow_map;


	// Revert any changes we made for rendering the depth map.
	glCullFace(GL_BACK);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
void MainGame::RenderDepthMap_DirectionalLights(const int& directional_light_index)
{
	glViewport(0, 0, Light::kShadowTextureWidth, Light::kShadowTextureHeight);
	glBindFramebuffer(GL_FRAMEBUFFER, directional_lights_[directional_light_index].shadow_map_fbo);
	glClear(GL_DEPTH_BUFFER_BIT);
	glCullFace(GL_FRONT);


	// Determine matrices for rendering from the light's point of view.
	glm::mat4 light_projection, light_view, light_space_matrix;
	light_projection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, main_camera_->get_near_clip(), main_camera_->get_far_clip());
	light_view = glm::lookAt(directional_lights_[directional_light_index].get_direction() * -7.5f, glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	light_space_matrix = light_projection * light_view;

	depth_buffer_directional_light_shader_.set_mat_4("light_space_matrix", light_space_matrix);


	// Draw all our GameObjects.
	ground_terrain_->Draw(*main_camera_, &depth_buffer_directional_light_shader_);

	fir_tree_->Draw(*main_camera_, &depth_buffer_directional_light_shader_);
	//plane_->Draw(*main_camera_, &depth_buffer_directional_light_shader_);
	wooden_bench_->Draw(*main_camera_, &depth_buffer_directional_light_shader_);
	dir_light_object_reference_->Draw(*main_camera_, &depth_buffer_directional_light_shader_);
	point_light_object_reference_0_->Draw(*main_camera_, &depth_buffer_directional_light_shader_);
	point_light_object_reference_1_->Draw(*main_camera_, &depth_buffer_directional_light_shader_);
	three_axies_->Draw(*main_camera_, &depth_buffer_directional_light_shader_);


	basic_shadows_.set_mat_4("directional_lights[" + std::to_string(directional_light_index) + "].light_space_matrix", light_space_matrix, true);
	terrain_shader_.set_mat_4("directional_lights[" + std::to_string(directional_light_index) + "].light_space_matrix", light_space_matrix, true);
	lighting_test_shader_.set_mat_4("directional_lights[" + std::to_string(directional_light_index) + "].light_space_matrix", light_space_matrix, true);

	Mesh::s_shadows_depth_maps[directional_light_index] = directional_lights_[directional_light_index].shadow_map;


	// Revert any changes we made for rendering the depth map.
	glCullFace(GL_BACK);
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

	// Setup the Point Light Shadow Paramters.
	float far_clip = main_camera_->get_far_clip();
	glm::vec3 camera_pos = main_camera_->get_transform()->get_pos();

	lighting_test_shader_.set_float("far_plane", far_clip, true);
	lighting_test_shader_.set_vec_3("camera_pos", camera_pos, true);

	terrain_shader_.set_float("far_plane", far_clip, true);
	terrain_shader_.set_vec_3("view_pos", camera_pos, true);

	basic_shadows_.set_float("far_plane", far_clip, true);
	basic_shadows_.set_vec_3("view_pos", camera_pos, true);
	

	// Setup the Directional Lights.
	for (int i = 0; i < directional_lights_.size(); ++i)
	{
		directional_lights_[i].UpdateShader(lighting_test_shader_, i);
		directional_lights_[i].UpdateShader(terrain_shader_, i);
		directional_lights_[i].UpdateShader(basic_shadows_, i);
	}

	// Setup the Point Lights.
	for (int i = 0; i < point_lights_.size(); ++i)
	{
		point_lights_[i].UpdateShader(lighting_test_shader_, i);
		point_lights_[i].UpdateShader(terrain_shader_, i);
		point_lights_[i].UpdateShader(basic_shadows_, i);
	}
}
void MainGame::RenderScene()
{
	// Draw all our GameObjects.
	ground_terrain_->Draw(*main_camera_, &terrain_shader_);

	fir_tree_->Draw(*main_camera_, active_shader_.get());
	//plane_->Draw(*main_camera_, active_shader_.get());
	wooden_bench_->Draw(*main_camera_, active_shader_.get());

	dir_light_object_reference_->Draw(*main_camera_, &default_shader_);
	point_light_object_reference_0_->Draw(*main_camera_, &default_shader_);
	point_light_object_reference_1_->Draw(*main_camera_, &default_shader_);
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
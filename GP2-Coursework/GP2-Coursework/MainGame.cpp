#include "MainGame.h"


GameState MainGame::game_state = GameState::kPlay;
MainGame::MainGame() :
	game_display_(Display()),
	input_manager_(std::make_unique<InputManager>()),
	main_camera_(new Camera(glm::vec3(0.0f, 3.5f, 0.0f), 70.0f, (float)game_display_.get_screen_width() / game_display_.get_screen_height(), 0.01f, 1000.0f)),
	depth_buffer_directional_light_shader_(	std::make_shared<Shader>("..\\res\\Shaders\\DepthRendering\\RenderToDepth_DirectionalLight.vert",	"..\\res\\Shaders\\DepthRendering\\RenderToDepth_DirectionalLight.frag")),
	depth_buffer_point_light_shader_(	std::make_shared<Shader>("..\\res\\Shaders\\DepthRendering\\RenderToDepth_PointLight.vert",	"..\\res\\Shaders\\DepthRendering\\RenderToDepth_PointLight.frag", "..\\res\\Shaders\\DepthRendering\\RenderToDepth_PointLight.gs")),
	lighting_test_shader_(	std::make_shared<Shader>("..\\res\\Shaders\\LightingTests.vert",	"..\\res\\Shaders\\LightingTests.frag")),
	terrain_shader_(		std::make_shared<Shader>("..\\res\\Shaders\\TerrainTexture.vert",	"..\\res\\Shaders\\TerrainTexture.frag")),
	default_shader_(		std::make_shared<Shader>("..\\res\\Shaders\\DefaultTexture.vert", "..\\res\\Shaders\\DefaultTexture.frag")),
	basic_shadows_(			std::make_shared<Shader>("..\\res\\Shaders\\Tests\\BasicShadows.vert", "..\\res\\Shaders\\Tests\\BasicShadows.frag")),

	skybox_(std::make_unique<Skybox>("..\\res\\Skyboxes\\PolyverseSkies-NightSky", ".jpg")),

	sun_light_dir_(glm::quat(glm::vec3(0.0f, 1.0f, 0.0f))),
	sun_diffuse_(MIDDAY_DIRECTIONAL_LIGHT_AMBIENT),
	counter_(0.0f)
{
	game_state = GameState::kPlay;
	//stbi_set_flip_vertically_on_load(true);


	active_shader_ = std::weak_ptr<Shader>(lighting_test_shader_);


	// ----- Setup Lights ----------
	directional_lights_[0] = DirectionalLight(kMiddayLightDirection, MIDDAY_DIRECTIONAL_LIGHT_AMBIENT, 1.0f);
	directional_lights_[1] = DirectionalLight(-kMiddayLightDirection, NIGHTTIME_DIRECTIONAL_LIGHT_AMBIENT, 0.01f);
	Mesh::s_shadows_depth_maps = std::vector<int>(directional_lights_.size(), -1);

	for (int i = 0; i < point_lights_.size(); ++i)
	{
		point_lights_[i] = PointLight(glm::vec3(0.0f), glm::vec3(0.965f, 0.882f, 0.678f), 5.0f, 2.0f, 1.0f);
	}
	Mesh::s_shadows_depth_cubemaps = std::vector<int>(point_lights_.size(), -1);


	// ----- Setup GameObjects & Models -----
	GameObject::PrepareForGameObjectLoad(9);
	ground_terrain_ = (new GameObject("..\\res\\Models\\Terrain\\NewMeshTerrain.obj", glm::vec3(500.0f, 0.0f, -500.0f), glm::radians(glm::vec3(0.0f)), glm::vec3(1.0f),
		std::vector<std::string>{"..\\res\\Models\\Terrain\\Texture_Grass_Diffuse.png", "..\\res\\Models\\Terrain\\Texture_Dirt_Diffuse.png", "..\\res\\Models\\Terrain\\Texture_Rock_Diffuse.png", "..\\res\\Models\\Terrain\\SplatAlpha 0.png"},
		std::vector<std::string>{},
		std::vector<std::string>{"..\\res\\Models\\Terrain\\Texture_Grass_Normal.png", "..\\res\\Models\\Terrain\\Texture_Dirt_Normal.png", "..\\res\\Models\\Terrain\\Texture_Rock_Normal.png"}))
		->set_shader_override(terrain_shader_);
	CreateTrees();

	//plane_ = new GameObject("..\\res\\Plane.obj", glm::vec3(0.0f), glm::radians(glm::vec3(0.0f, 0.0f, 0.0f)), glm::vec3(1.0f), "..\\res\\brickwall.jpg", "", "..\\res\\brickwall_normal.jpg");
	wooden_bench_ = new GameObject("..\\res\\Models\\Bench\\WoodenBench.obj", glm::vec3(2.7f, 0.0f, -0.4f), glm::radians(glm::vec3(0.0f, 140.0f, 0.0f)), glm::vec3(1.0f));
	hanging_lantern_ = new GameObject("..\\res\\Models\\Lanterns\\HangingLantern.obj", glm::vec3(8.55f, 1.0f, 0.0f), glm::radians(glm::vec3(0.0f, 96.0f, 0.0f)), glm::vec3(1.0f));
	bench_lantern_ = new GameObject("..\\res\\Models\\Lanterns\\Lantern.obj", glm::vec3(3.16f, 0.45f, -0.83f), glm::radians(glm::vec3(2.0f, 135.0f, 0.0f)), glm::vec3(1.0f));
	floor_lantern_ = new GameObject("..\\res\\Models\\Lanterns\\Lantern.obj", glm::vec3(1.8f, 0.05f, 0.1f), glm::radians(glm::vec3(2.0f, 135.0f, 0.0f)), glm::vec3(1.0f));


	// ----- Setup Reference/Debug Objects -----
	// Point Light References.
	point_light_object_references_.reserve(point_lights_.size());
	for (int i = 0; i < point_lights_.size(); ++i)
	{
		point_light_object_references_.push_back((new GameObject("..\\res\\IcoSphere.obj", glm::vec3(0.0f), glm::vec3(0.0f), glm::vec3(0.025f), "..\\res\\brickwall.jpg", "", "..\\res\\brickwall_normal.jpg"))
			->set_shader_override(default_shader_)
			->set_draw_for_parent_shader(false));

		// Setup our light references to be children of our lights.
		point_light_object_references_[i]->get_transform()->set_parent(point_lights_[i].get_transform(), true);
	}
	/*three_axies_ = (new GameObject("..\\res\\Models\\ThreeAxies.obj", glm::vec3(0.0f), glm::vec3(0.0f), glm::vec3(0.25f)))
		->set_shader_override(default_shader_);*/


	// Parent our point lights to their respective lanterns.
	point_lights_[0].get_transform()->set_parent(hanging_lantern_->get_transform());
	point_lights_[0].get_transform()->set_local_pos(LANTERN_LIGHT_LOCAL_POSITION);
	point_lights_[1].get_transform()->set_parent(bench_lantern_->get_transform());
	point_lights_[1].get_transform()->set_local_pos(LANTERN_LIGHT_LOCAL_POSITION);
	point_lights_[2].get_transform()->set_parent(floor_lantern_->get_transform());
	point_lights_[2].get_transform()->set_local_pos(LANTERN_LIGHT_LOCAL_POSITION);


	// Setup the daylight gradient.
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
	
	delete ground_terrain_;

	GameObject::DisposeAllGameObjects();
}


void MainGame::Run()
{
	glfwInit();
	GameLoop();
}

void MainGame::GameLoop()
{
	previous_time_since_start_ = glfwGetTime();
	while (game_state != GameState::kExit)
	{
		CalculateDeltaTime();

		// Process then Handle Input.
		input_manager_->ProcessInput(&game_display_);
		HandleCameraMovement();
		HandleCameraLook();

		// Calculate lighting.
		CalculateLightingValues();

		// Render our depth maps.
		for(unsigned int i = 0; i < point_lights_.size(); ++i)
			RenderDepthMap_PointLights(i);
		for (unsigned int i = 0; i < directional_lights_.size(); ++i)
		{
			if (glm::dot(directional_lights_[i].get_direction(), glm::vec3(0.0f, -1.0f, 0.0f)) < -0.1f)
			{
				// Below the horizon. No point in re-rendering shadows.
				// If there is issues with shadows after the sun is under the horizon, remove this.
				continue;
			}

			RenderDepthMap_DirectionalLights(i);
		}

		// Draw the scene.
		DrawGame();


		// Increment the Counter.
		counter_ = counter_ + ((input_manager_->get_speedup_time_held() ? kTimeSpeedupValue : 1.0f) * delta_time_);
	}

	glfwTerminate();
}

void MainGame::CalculateDeltaTime()
{
	current_time_since_start_ = glfwGetTime();
	delta_time_ = current_time_since_start_ - previous_time_since_start_;
	previous_time_since_start_ = current_time_since_start_;
}


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
	// Note: For some reason we are getting an incorrect rotation if we alter our current value directly,
	//		both for when adding this frame's change or by setting euler angles for x_rotation & y_rotation.
	//		Instead, we're resetting our rotation to 0 each frame and then rotating along our desired axies.
	camera_transform->set_euler_angles(0.0f, 0.0f, 0.0f);
	camera_transform->Rotate(glm::vec3(1.0f, 0.0f, 0.0f), glm::radians(x_rotation), Transform::kLocalSpace);
	camera_transform->Rotate(glm::vec3(0.0f, 1.0f, 0.0f), glm::radians(y_rotation), Transform::kWorldSpace);
}


void MainGame::RenderDepthMap_PointLights(const int& point_light_index)
{
	glViewport(0, 0, PointLight::kShadowTextureWidth, PointLight::kShadowTextureHeight);
	glBindFramebuffer(GL_FRAMEBUFFER, point_lights_[point_light_index].shadow_map_fbo);
	glClear(GL_DEPTH_BUFFER_BIT);
	glCullFace(GL_FRONT);

	// Determine matrices for rendering from the light's point of view.
	const float kAspect = (float)PointLight::kShadowTextureWidth / (float)PointLight::kShadowTextureHeight;
	glm::mat4 shadow_projection_matrix = glm::perspective(glm::radians(90.0f), kAspect, main_camera_->get_near_clip(), main_camera_->get_far_clip());
	glm::vec3 light_pos = point_lights_[point_light_index].get_transform()->get_pos();
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
		depth_buffer_point_light_shader_->set_mat_4("shadow_matrices[" + std::to_string(i) + "]", shadow_transforms[i]);
	}
	depth_buffer_point_light_shader_->set_float("far_plane", main_camera_->get_far_clip());
	depth_buffer_point_light_shader_->set_vec_3("light_pos", light_pos);


	// Draw all our GameObjects to the depth buffer.
	GameObject::DrawAllForDepth(*main_camera_, point_lights_[point_light_index].get_transform(), depth_buffer_point_light_shader_.get());
	

	// Set the access index for this light's shadow map.
	Mesh::s_shadows_depth_cubemaps[point_light_index] = point_lights_[point_light_index].shadow_map;


	// Revert any changes we made for rendering the depth map.
	glCullFace(GL_BACK);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
void MainGame::RenderDepthMap_DirectionalLights(const int& directional_light_index)
{
	glViewport(0, 0, DirectionalLight::kShadowTextureWidth, DirectionalLight::kShadowTextureHeight);
	glBindFramebuffer(GL_FRAMEBUFFER, directional_lights_[directional_light_index].shadow_map_fbo);
	glClear(GL_DEPTH_BUFFER_BIT);
	glCullFace(GL_FRONT);


	// Determine matrices for rendering from the light's point of view.
	float kDirectionalLightDistance = 100.0f;
	float kShadowMapArea = 50.0f; // Length of one side of the cube inside of which we capture shadow objects (Needs confirmation).
	glm::mat4 light_projection, light_view, light_space_matrix;

	light_projection = glm::ortho(-kShadowMapArea, kShadowMapArea, -kShadowMapArea, kShadowMapArea, main_camera_->get_near_clip(), main_camera_->get_far_clip());
	light_view = glm::lookAt(directional_lights_[directional_light_index].get_direction() * -kDirectionalLightDistance, glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	light_space_matrix = light_projection * light_view;


	depth_buffer_directional_light_shader_->set_mat_4("light_space_matrix", light_space_matrix);


	// Draw all our GameObjects to the depth buffer.
	GameObject::DrawAllForDepth(*main_camera_, directional_lights_[directional_light_index].get_transform(), depth_buffer_directional_light_shader_.get());


	// Link our Light Space Matrix for this light.
	basic_shadows_->set_mat_4("directional_lights[" + std::to_string(directional_light_index) + "].light_space_matrix", light_space_matrix, true);
	terrain_shader_->set_mat_4("directional_lights[" + std::to_string(directional_light_index) + "].light_space_matrix", light_space_matrix, true);
	lighting_test_shader_->set_mat_4("directional_lights[" + std::to_string(directional_light_index) + "].light_space_matrix", light_space_matrix, true);

	// Set the access index for this light's shadow map.
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
	// Setup the Point Light Shadow Paramters.
	float far_clip = main_camera_->get_far_clip();
	glm::vec3 camera_pos = main_camera_->get_transform()->get_pos();

	lighting_test_shader_->set_float("far_plane", far_clip, true);
	lighting_test_shader_->set_vec_3("camera_pos", camera_pos, true);

	terrain_shader_->set_float("far_plane", far_clip, true);
	terrain_shader_->set_vec_3("view_pos", camera_pos, true);

	basic_shadows_->set_float("far_plane", far_clip, true);
	basic_shadows_->set_vec_3("view_pos", camera_pos, true);
	

	// Setup the Directional Lights.
	for (int i = 0; i < directional_lights_.size(); ++i)
	{
		directional_lights_[i].UpdateShader(lighting_test_shader_.get(), i);
		directional_lights_[i].UpdateShader(terrain_shader_.get(), i);
		directional_lights_[i].UpdateShader(basic_shadows_.get(), i);
	}

	// Setup the Point Lights.
	for (int i = 0; i < point_lights_.size(); ++i)
	{
		point_lights_[i].UpdateShader(lighting_test_shader_.get(), i);
		point_lights_[i].UpdateShader(terrain_shader_.get(), i);
		point_lights_[i].UpdateShader(basic_shadows_.get(), i);
	}
}
void MainGame::RenderScene()
{
	// Draw all our GameObjects.
	GameObject::DrawAll(*main_camera_, active_shader_.lock().get());
	

	// Draw the skybox.
	skybox_->Draw(*main_camera_, day_percentage_time, sun_light_dir_);
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
	sun_light_dir_ = glm::angleAxis(glm::radians(desired_angle), glm::normalize(kSunRotationAxis));

	// Update the sun directional light.
	directional_lights_[0].set_direction(sun_light_dir_);
	directional_lights_[0].set_diffuse(sun_diffuse_);

	// Update the moon directional light to be shining in the opposite direction as the sun.
	const glm::quat kMoonLightDirRotation = glm::angleAxis(glm::radians(180.0f), glm::normalize(kSunRotationAxis));
	directional_lights_[1].set_direction(sun_light_dir_ * kMoonLightDirRotation);
}


inline void MainGame::LogVec3(const glm::vec3& vector)
{
	std::cout << vector.x << "," << vector.y << "," << vector.z << std::endl;
}


void MainGame::CreateTrees()
{
	const std::string kFirTreeModelPath = "..\\res\\Models\\Trees\\LowPolyFirTree.obj";
	const std::string kTreeCsvPath = "..\\res\\TreeLocations.csv";
	const int kTreeCount = 49;
	fir_trees_.reserve(kTreeCount);

	// File Pointer.
	std::fstream file_in;

	// Open the CSV File.
	file_in.open(kTreeCsvPath, std::ios::in);

	// Rad the data from the file.
	std::string line, word, temp;
	int j = 0;
	while (!file_in.eof())
	{
		// Read an entire csv row.
		std::getline(file_in, line);

		// Used for breaking up entries.
		std::stringstream s(line);

		// Read every column data of a row and store it in the variable 'word'.
		int i = 0;
	glm::vec3 pos, rot, scale;
		while (std::getline(s, word, ','))
		{
			switch (i)
			{
			// Pos.
			case 0: pos.x = std::stof(word); break;
			case 1: pos.y = std::stof(word); break;
			case 2: pos.z = std::stof(word); break;

			// Rot.
			case 3: rot.x = std::stof(word); break;
			case 4: rot.y = std::stof(word); break;
			case 5: rot.z = std::stof(word); break;

			// Scale.
			case 6: scale.x = std::stof(word); break;
			case 7: scale.y = std::stof(word); break;
			case 8: scale.z = std::stof(word); break;
			}
			++i;
		}

		// Create the tree.
		fir_trees_.push_back(new GameObject(kFirTreeModelPath, pos, glm::radians(rot), scale, "..\\res\\Models\\Trees\\LowPolyFirTree_Diffuse.png", "", ""));
	}

	file_in.close();
}
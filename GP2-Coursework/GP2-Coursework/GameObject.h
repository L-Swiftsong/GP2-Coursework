#pragma once

#include "Model.h"
#include "transform.h"
#include "Shader.h"
#include "Camera.h"

#include <algorithm>

#define DEFAULT_TEXTURE_PATH "..\\res\\bricks.jpg"

class GameObject
{
public:
	GameObject(const std::string& mesh_file_name, const glm::vec3& position, const glm::vec3& rotation, const glm::vec3& scale);
	GameObject(const std::string& mesh_file_path, const glm::vec3& position, const glm::vec3& rotation, const glm::vec3& scale,
		const std::string& diffuse_texture_path, const std::string& specular_texture_path = "", const std::string& normal_texture_path = "");
	GameObject(const std::string& mesh_file_path, const glm::vec3& position, const glm::vec3& rotation, const glm::vec3& scale,
		const std::vector<std::string>& diffuse_texture_paths, const std::vector<std::string>& specular_texture_paths, const std::vector<std::string>& normal_texture_paths);
	~GameObject();


	static void PrepareForGameObjectLoad(int expected_gameobject_count = 0);
	static void DisposeAllGameObjects();

	static void DrawAll(const Camera& camera, Shader* active_shader);
	static void DrawAllWithOverride(const Camera& camera, Shader* override_shader);
	static void DrawAllForDepth(const Camera& camera, const Transform* light_transform, Shader* override_shader);


	GameObject* set_shader_override(std::shared_ptr<Shader> shader);
	GameObject* set_draw_for_parent_shader(const bool& new_value);

	Model* get_model() const;
	Transform* get_transform() const;

private:
	GameObject() = delete; // No default constructor.
	GameObject(const GameObject& other) = delete; // No copy constructor.
	void AddToAllGameObjects();
	void RemoveFromAllGameObjects();

	
	void DrawDefault(const Camera& camera, Shader* active_shader);
	void DrawOverride(const Camera& camera, Shader* override_shader);


	std::unique_ptr<Model> model_;
	std::unique_ptr<Transform> transform_;
	std::shared_ptr<Shader> override_shader_ = nullptr;
	bool draw_for_parent_light = true;

	static std::vector<GameObject*> s_all_gameobjects_;
	bool is_disposing = false;
};
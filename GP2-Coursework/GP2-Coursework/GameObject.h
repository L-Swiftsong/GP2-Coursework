#pragma once

#include "Model.h"
#include "transform.h"
#include "Shader.h"
#include "Camera.h"

#define DEFAULT_TEXTURE_PATH "..\\res\\bricks.jpg"

class GameObject
{
public:
	GameObject(const std::string& mesh_file_name);
	GameObject(const std::string& mesh_file_name, const glm::vec3& position, const glm::vec3& rotation, const glm::vec3& scale);
	GameObject(const std::string& mesh_file_path, const glm::vec3& position, const glm::vec3& rotation, const glm::vec3& scale, const std::string& diffuse_texture_path, const std::string& specular_texture_path = "", const std::string& normal_texture_path = "");
	GameObject(const std::string& mesh_file_path, const glm::vec3& position, const glm::vec3& rotation, const glm::vec3& scale, const std::vector<std::string>& diffuse_texture_paths, const std::vector<std::string>& specular_texture_paths, const std::vector<std::string>& normal_texture_paths);
	~GameObject();

	void Draw(const Camera& camera, Shader* shader);

	Model* get_model() const;
	Transform* get_transform() const;

private:
	GameObject() = delete;

	Model* model_;
	Transform* transform_;
};
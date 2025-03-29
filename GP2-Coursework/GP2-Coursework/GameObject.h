#pragma once

#include "Model.h"
#include "transform.h"
#include "Shader.h"
#include "Camera.h"

#define DEFAULT_TEXTURE_PATH "..\\res\\bricks.jpg"

class GameObject
{
public:
	GameObject(const std::string mesh_file_name);
	GameObject(const std::string mesh_file_name, glm::vec3 position, glm::vec3 rotation, glm::vec3 scale);
	GameObject::GameObject(const std::string mesh_file_path, glm::vec3 position, glm::vec3 rotation, glm::vec3 scale, const std::string& diffuse_texture_path, const std::string& specular_texture_path = "", const std::string& height_texture_path = "");
	~GameObject();

	void Draw(const Camera& camera, Shader* shader);

	Model* get_model() const;
	Transform* get_transform() const;

private:
	GameObject() = delete;

	Model* model_;
	Transform* transform_;
};
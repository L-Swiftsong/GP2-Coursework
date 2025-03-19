#pragma once

#include "Mesh.h"
#include "transform.h"
#include "Texture.h"
#include "Shader.h"
#include "Camera.h"

#define DEFAULT_TEXTURE_PATH "..\\res\\bricks.jpg"

class GameObject
{
public:
	GameObject(const std::string mesh_file_name);
	GameObject(const std::string mesh_file_name, const std::string texture_file_path, glm::vec3 position, glm::vec3 rotation, glm::vec3 scale_);
	~GameObject();

	void Draw();
	void Draw(const Camera& camera, Shader* shader);

	Transform* get_transform() const;
	Mesh* get_mesh() const;

private:
	GameObject() = delete;

	Mesh* mesh_;
	Transform* transform_;
	Texture* texture_;
};
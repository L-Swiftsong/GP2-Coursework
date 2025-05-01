#pragma once
#include "GameObject.h"

GameObject::GameObject(const std::string mesh_file_path) : GameObject(mesh_file_path, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f))
{}
GameObject::GameObject(const std::string mesh_file_path, glm::vec3 position, glm::vec3 rotation, glm::vec3 scale) :
	model_(new Model(mesh_file_path)),
	transform_(new Transform(position, rotation, scale))
{}
GameObject::GameObject(const std::string mesh_file_path, glm::vec3 position, glm::vec3 rotation, glm::vec3 scale, const std::string& diffuse_texture_path, const std::string& specular_texture_path, const std::string& normal_texture_path) :
	transform_(new Transform(position, rotation, scale))
{
	std::vector<std::shared_ptr<Texture>> textureOverrides;
	if (diffuse_texture_path != "")
		textureOverrides.push_back(std::make_shared<Texture>(TextureType::kDiffuse, diffuse_texture_path));
	if (specular_texture_path != "")
		textureOverrides.push_back(std::make_shared<Texture>(TextureType::kSpecular, specular_texture_path));
	if (normal_texture_path != "")
		textureOverrides.push_back(std::make_shared<Texture>(TextureType::kNormal, normal_texture_path));

	model_ = new Model(mesh_file_path, textureOverrides);
}

GameObject::~GameObject()
{
	delete model_;
	delete transform_;
}


void GameObject::Draw(const Camera& camera, Shader* shader)
{
	shader->Bind();
	shader->Update(*transform_, camera);
	model_->Draw(*shader);
	//mesh_->UpdateSphereData(*transform_->get_pos(), 0.62f);
}


Transform* GameObject::get_transform() const { return transform_; }
Model* GameObject::get_model() const { return model_; }
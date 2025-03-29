#pragma once
#include "GameObject.h"

GameObject::GameObject(const std::string mesh_file_path) : GameObject(mesh_file_path, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f))
{}
GameObject::GameObject(const std::string mesh_file_path, glm::vec3 position, glm::vec3 rotation, glm::vec3 scale_) :
	model_(new Model(mesh_file_path)),
	transform_(new Transform(position, rotation, scale_))
{}

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
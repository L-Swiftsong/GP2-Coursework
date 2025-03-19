#pragma once

#include "GameObject.h"

GameObject::GameObject(const std::string mesh_file_path) : GameObject(mesh_file_path, "..\\res\\bricks.jpg", glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f))
{}
GameObject::GameObject(const std::string mesh_file_path, const std::string texture_file_path, glm::vec3 position, glm::vec3 rotation, glm::vec3 scale_) :
	mesh_(new Mesh(mesh_file_path)),
	transform_(new Transform(position, rotation, scale_)),
	texture_(new Texture(texture_file_path))
{}

GameObject::~GameObject()
{
	delete mesh_;
	delete transform_;
	delete texture_;
}


void GameObject::Draw()
{
	texture_->Bind(0);
	mesh_->Draw();
	mesh_->UpdateSphereData(*transform_->get_pos(), 0.62f);
}
void GameObject::Draw(const Camera& camera, Shader* shader)
{
	shader->Bind();
	texture_->Bind(0);
	shader->Update(*transform_, camera);
	mesh_->Draw();
	mesh_->UpdateSphereData(*transform_->get_pos(), 0.62f);
}


Transform* GameObject::get_transform() const { return transform_; }
Mesh* GameObject::get_mesh() const { return mesh_; }
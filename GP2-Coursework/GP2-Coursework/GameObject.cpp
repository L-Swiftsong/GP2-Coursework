#pragma once
#include "GameObject.h"

std::vector<GameObject*> GameObject::s_all_gameobjects_;
GameObject::GameObject(const std::string& mesh_file_path, const glm::vec3& position, const glm::vec3& rotation, const glm::vec3& scale)
	: model_(std::make_unique<Model>(mesh_file_path)),
	transform_(std::make_unique<Transform>(position, rotation, scale))
{
	AddToAllGameObjects();
}
GameObject::GameObject(const std::string& mesh_file_path, const glm::vec3& position, const glm::vec3& rotation, const glm::vec3& scale,
	const std::string& diffuse_texture_path, const std::string& specular_texture_path, const std::string& normal_texture_path)
	: transform_(std::make_unique<Transform>(position, rotation, scale))
{
	std::vector<std::shared_ptr<Texture>> textureOverrides;
	if (diffuse_texture_path != "")
		textureOverrides.push_back(std::make_shared<Texture>(TextureType::kDiffuse, diffuse_texture_path));
	if (specular_texture_path != "")
		textureOverrides.push_back(std::make_shared<Texture>(TextureType::kSpecular, specular_texture_path));
	if (normal_texture_path != "")
		textureOverrides.push_back(std::make_shared<Texture>(TextureType::kNormal, normal_texture_path));

	model_ = std::make_unique<Model>(mesh_file_path, textureOverrides);

	AddToAllGameObjects();
}
GameObject::GameObject(const std::string& mesh_file_path, const glm::vec3& position, const glm::vec3& rotation, const glm::vec3& scale,
	const std::vector<std::string>& diffuse_texture_paths, const std::vector<std::string>& specular_texture_paths, const std::vector<std::string>& normal_texture_paths)
	: transform_(new Transform(position, rotation, scale))
{
	std::vector<std::shared_ptr<Texture>> textureOverrides;
	for (int i = 0; i < diffuse_texture_paths.size(); ++i)
		textureOverrides.push_back(std::make_shared<Texture>(TextureType::kDiffuse, diffuse_texture_paths[i]));
	for (int i = 0; i < specular_texture_paths.size(); ++i)
		textureOverrides.push_back(std::make_shared<Texture>(TextureType::kSpecular, specular_texture_paths[i]));
	for (int i = 0; i < normal_texture_paths.size(); ++i)
		textureOverrides.push_back(std::make_shared<Texture>(TextureType::kNormal, normal_texture_paths[i]));

	model_ = std::make_unique<Model>(mesh_file_path, textureOverrides);

	AddToAllGameObjects();
}
GameObject::~GameObject()
{
	if (!is_disposing)
	{
		RemoveFromAllGameObjects();
	}
}


void GameObject::PrepareForGameObjectLoad(int expected_gameobject_count)
{
	DisposeAllGameObjects();
	s_all_gameobjects_.reserve(expected_gameobject_count);
}
void GameObject::DisposeAllGameObjects()
{
	for (const auto& gameobject : s_all_gameobjects_)
	{
		gameobject->is_disposing = true;
		delete gameobject;
	}
	s_all_gameobjects_.clear();
}

void GameObject::AddToAllGameObjects()
{
	s_all_gameobjects_.push_back(this);
}
void GameObject::RemoveFromAllGameObjects()
{
	s_all_gameobjects_.erase(std::remove(s_all_gameobjects_.begin(), s_all_gameobjects_.end(), this), s_all_gameobjects_.end());
}


void GameObject::DrawAll(const Camera& camera, Shader* active_shader)
{
	for (int i = 0; i < s_all_gameobjects_.size(); ++i)
	{
		s_all_gameobjects_[i]->DrawDefault(camera, active_shader);
	}
}
void GameObject::DrawAllWithOverride(const Camera& camera, Shader* override_shader)
{
	for (int i = 0; i < s_all_gameobjects_.size(); ++i)
	{
		s_all_gameobjects_[i]->DrawOverride(camera, override_shader);
	}
}
void GameObject::DrawAllForDepth(const Camera& camera, const Transform* light_transform, Shader* depth_shader)
{
	for (int i = 0; i < s_all_gameobjects_.size(); ++i)
	{
		// Only render the GameObject to the depth shader if we can either:
		//	- Be drawn as the child of the active light (Done first as is less expensive a check)
		//	OR
		//	- Are not the child of the active light
		if (s_all_gameobjects_[i]->draw_for_parent_light || !light_transform->HasChild(s_all_gameobjects_[i]->get_transform()))
		{
			s_all_gameobjects_[i]->DrawOverride(camera, depth_shader);
		}
	}
}
void GameObject::DrawDefault(const Camera& camera, Shader* active_shader)
{
	// Determine the shader we will be using (The active shader only if we don't have a per-instance override set).
	Shader* shader = override_shader_ != nullptr ? override_shader_.get() : active_shader;

	// Bind and update our rendering shader.
	shader->Bind();
	shader->Update(*transform_, camera);

	// Draw the model associated with this GO.
	model_->Draw(*shader);
}
void GameObject::DrawOverride(const Camera& camera, Shader* override_shader)
{
	// Bind and update our rendering shader.
	override_shader->Bind();
	override_shader->Update(*transform_, camera);

	// Draw the model associated with this GO.
	model_->Draw(*override_shader);
}


GameObject* GameObject::set_shader_override(std::shared_ptr<Shader> shader_override) { override_shader_ = shader_override; return this; }
GameObject* GameObject::set_draw_for_parent_shader(const bool& new_value) { draw_for_parent_light = new_value; return this; }


Transform* GameObject::get_transform() const { return transform_.get(); }
Model* GameObject::get_model() const { return model_.get(); }
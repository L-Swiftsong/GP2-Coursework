#pragma once
#include <glm\glm.hpp>
#include <GL\glew.h>
#include <string>
#include <vector>

#include "Vertex.h"
#include "Texture.h"
#include "Shader.h"


class Mesh
{
public:
	Mesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices, const std::vector<std::shared_ptr<Texture>>& textures);
	~Mesh();

	void Draw(const Shader& shader);



private:
	void SetupMesh();

	std::vector<Vertex> vertices_;
	std::vector<unsigned int> indices_;
	std::vector<std::shared_ptr<Texture>> textures_;
	unsigned int vertex_array_object_, vertex_buffer_object_, element_buffer_object_;
};
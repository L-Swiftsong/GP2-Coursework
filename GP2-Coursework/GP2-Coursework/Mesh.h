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

	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	std::vector<std::shared_ptr<Texture>> textures;
	unsigned int vertex_array_object;


private:
	void SetupMesh();

	unsigned int vertex_buffer_object, element_buffer_object;
};
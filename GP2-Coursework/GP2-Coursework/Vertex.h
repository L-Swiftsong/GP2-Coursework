#pragma once

#include <glm\glm.hpp>
#include <GL\glew.h>

#define MAX_BONE_INFLUENCE 4

struct Vertex
{
public:
	glm::vec3* get_pos() { return &Position; }
	glm::vec2* get_tex_coord() { return &TexCoords; }
	glm::vec3* get_normal() { return &Normal; }
	glm::vec3* get_tangent() { return &Tangent; }
	glm::vec3* get_bitangent() { return &Bitangent; }
	
	glm::vec3 Position;
	glm::vec2 TexCoords;
	glm::vec3 Normal;
	glm::vec3 Tangent;
	glm::vec3 Bitangent;

	//bone indexes which will influence this vertex
	int m_BoneIDs[MAX_BONE_INFLUENCE];
	//weights from each bone
	float m_Weights[MAX_BONE_INFLUENCE];

private:
};
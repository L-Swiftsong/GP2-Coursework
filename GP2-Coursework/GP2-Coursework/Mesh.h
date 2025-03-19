#pragma once
#include <glm\glm.hpp>
#include <GL\glew.h>
#include <string>
#include "obj_loader.h"

struct Vertex
{
public:
	Vertex(const glm::vec3& pos, const glm::vec2& tex_coord, const glm::vec3& normal)
	{
		this->pos_ = pos;
		this->tex_coord_ = tex_coord;
		this->normal_ = normal;
	}

	glm::vec3* get_pos()  { return &pos_; }
	glm::vec2* get_tex_coord() { return &tex_coord_; }
	glm::vec3* get_normal() { return &normal_; }

private:
	glm::vec3 pos_;
	glm::vec2 tex_coord_;
	glm::vec3 normal_;
};

struct Sphere
{
public:

	Sphere() : pos_(glm::vec3(0.0f, 0.0f, 0.0f)), radius_(0.0f) {}

	Sphere(glm::vec3& pos, float radius)
	{
		this->pos_ = pos;
		this->radius_ = radius;
	}

	glm::vec3 get_pos() const { return pos_; }
	float get_radius() const { return radius_; }

	void set_pos(glm::vec3 pos)
	{
		this->pos_ = pos;
	}

	void set_radius(float radius)
	{
		this->radius_ = radius;
	}

private:
	glm::vec3 pos_;
	float radius_;
};

class Mesh
{
public:
	Mesh(const std::string& file_name);
	Mesh(Vertex* vertices, unsigned int num_vertices, unsigned int* indices, unsigned int num_indices);
	~Mesh();


	void Draw();
	void UpdateSphereData(glm::vec3 pos, float radius);

	glm::vec3 get_sphere_pos() const { return mesh_sphere_.get_pos(); }
	float get_sphere_radius() const { return mesh_sphere_.get_radius(); }

private:
	Mesh() = delete;
	void InitModel(const IndexedModel& model);


	enum
	{
		kPositionVertexBuffer,
		kTexCoordVertexBuffer,
		kNormalVertexBuffer,
		kIndexVertexBuffer,

		kNumBuffers
	};


	Sphere mesh_sphere_;
	GLuint vertex_array_object_;

	// Create our array of buffers.
	GLuint vertex_array_buffers_[kNumBuffers];

	// How much of the vertexArrayObject do we want to draw.
	unsigned int draw_count_;
};
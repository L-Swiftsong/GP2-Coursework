#include "Mesh.h"
#include <vector>


Mesh::Mesh(const std::string& file_name)
{
	IndexedModel model = OBJModel(file_name).ToIndexedModel();
	InitModel(model);
	Sphere mesh_sphere_();
}
Mesh::Mesh(Vertex* vertices, unsigned int num_vertices, unsigned int* indices, unsigned int num_indices)
{
	IndexedModel model;

	for (unsigned int i = 0; i < num_vertices; i++)
	{
		model.positions.push_back(*vertices[i].get_pos());
		model.texCoords.push_back(*vertices[i].get_tex_coord());
		model.normals.push_back(*vertices[i].get_normal());
	}

	for (unsigned int i = 0; i < num_indices; i++)
		model.indices.push_back(indices[i]);

	InitModel(model);
}


void Mesh::InitModel(const IndexedModel& model)
{
	draw_count_ = model.indices.size();

	glGenVertexArrays(1, &vertex_array_object_); //generate a vertex array and store it in the VAO
	glBindVertexArray(vertex_array_object_); //bind the VAO (any operation that works on a VAO will work on our bound VAO - binding)

	glGenBuffers(kNumBuffers, vertex_array_buffers_); //generate our buffers based of our array of data/buffers - GLuint vertexArrayBuffers[NUM_BUFFERS];

	glBindBuffer(GL_ARRAY_BUFFER, vertex_array_buffers_[kPositionVertexBuffer]); //tell opengl what type of data the buffer is (GL_ARRAY_BUFFER), and pass the data
	glBufferData(GL_ARRAY_BUFFER, model.positions.size() * sizeof(model.positions[0]), &model.positions[0], GL_STATIC_DRAW); //move the data to the GPU - type of data, size of data, starting address (pointer) of data, where do we store the data on the GPU (determined by type)
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, vertex_array_buffers_[kTexCoordVertexBuffer]); //tell opengl what type of data the buffer is (GL_ARRAY_BUFFER), and pass the data
	glBufferData(GL_ARRAY_BUFFER, model.positions.size() * sizeof(model.texCoords[0]), &model.texCoords[0], GL_STATIC_DRAW); //move the data to the GPU - type of data, size of data, starting address (pointer) of data, where do we store the data on the GPU
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, vertex_array_buffers_[kNormalVertexBuffer]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(model.normals[0]) * model.normals.size(), &model.normals[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vertex_array_buffers_[kIndexVertexBuffer]); //tell opengl what type of data the buffer is (GL_ARRAY_BUFFER), and pass the data
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, model.indices.size() * sizeof(model.indices[0]), &model.indices[0], GL_STATIC_DRAW); //move the data to the GPU - type of data, size of data, starting address (pointer) of data, where do we store the data on the GPU

	glBindVertexArray(0); // unbind our VAO
}

Mesh::~Mesh()
{
	glDeleteVertexArrays(1, &vertex_array_object_); // delete arrays
}

void Mesh::Draw()
{
	glBindVertexArray(vertex_array_object_);

	glDrawElements(GL_TRIANGLES, draw_count_, GL_UNSIGNED_INT, 0);
	//glDrawArrays(GL_TRIANGLES, 0, drawCount);

	glBindVertexArray(0);
}

void Mesh::UpdateSphereData(glm::vec3 pos, float radius)
{
	mesh_sphere_.set_pos(pos);
	mesh_sphere_.set_radius(radius);
}


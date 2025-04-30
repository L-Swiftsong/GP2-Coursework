#include "Skybox.h"

Skybox::Skybox(const std::string& file_name_no_extensions, const std::string& file_extension) :
	skybox_texture_(std::make_unique<Texture>(Cubemap::CreateCubemapTexture(file_name_no_extensions, file_extension))),
	//skybox_shader_(std::make_unique<Shader>(Shader(SKYBOX_VERTEX_SHADER_PATH, SKYBOX_FRAGMENT_SHADER_PATH)))
	skybox_shader_(Shader(SKYBOX_VERTEX_SHADER_PATH, SKYBOX_FRAGMENT_SHADER_PATH))
{
	// Create the Skybox's Cube for projecting the texture on.
	glGenVertexArrays(1, &vertex_array_object_);
	glGenBuffers(1, &vertex_buffer_object_);
	glBindVertexArray(vertex_array_object_);
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_object_);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

	skybox_shader_.set_int("skybox", 0);
}


void Skybox::Draw(const Camera& camera)
{
	glDepthFunc(GL_LEQUAL);  // change depth function so depth test passes when values are equal to depth buffer's content
	glCullFace(false); // (Temp).

	skybox_shader_.Bind();

	glm::mat4 view = glm::mat4(glm::mat3(camera.get_view()));
	skybox_shader_.set_mat_4("view", view);
	skybox_shader_.set_mat_4("projection", camera.get_projection());

	// skybox cube
	glBindVertexArray(vertex_array_object_);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, skybox_texture_->get_texture_id());
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);
	glDepthFunc(GL_LESS); // set depth function back to default
}
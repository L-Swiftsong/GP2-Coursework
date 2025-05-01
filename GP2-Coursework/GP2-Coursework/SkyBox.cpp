#include "Skybox.h"

Skybox::Skybox(const std::string& file_name_no_extensions, const std::string& file_extension) :
	skybox_texture_(Cubemap::CreateCubemapTexture(file_name_no_extensions, file_extension)),
	skybox_shader_(std::make_unique<Shader>(SKYBOX_VERTEX_SHADER_PATH, SKYBOX_FRAGMENT_SHADER_PATH))
{
	unsigned int skybox_ebo;
	glGenVertexArrays(1, &vertex_array_object_);
	glGenBuffers(1, &vertex_buffer_object_);
	//glGenBuffers(1, &skybox_ebo);

	glBindVertexArray(vertex_array_object_);

	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_object_);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);

	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, skybox_ebo);
	//glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(skyboxIndices), &skyboxIndices, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

	//glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	skybox_shader_->set_int("skybox", 0);
}
Skybox::~Skybox()
{}


void Skybox::Draw(const Camera& camera)
{
	// Change our depth function so that we draw the skybox behind everything.
	glDepthFunc(GL_LEQUAL);


	// Update the skybox shader.
	skybox_shader_->Bind();
	glm::mat4 view = glm::mat4(glm::mat3(camera.get_view())); // We're casting the view matrix into a mat3 then back to a mat4 in order to remove the translations, but preserve rotation transformations.
	skybox_shader_->set_mat_4("viewMatrix", view);
	skybox_shader_->set_mat_4("projectionMatrix", camera.get_projection());


	// Draw the skybox cube.
	glBindVertexArray(vertex_array_object_);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, skybox_texture_->get_texture_id());
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);

	// Reset the depth function.
	glDepthFunc(GL_LESS);
}
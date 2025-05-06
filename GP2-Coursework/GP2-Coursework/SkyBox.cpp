#include "Skybox.h"

Skybox::Skybox(const std::string& file_name_no_extensions, const std::string& file_extension) :
	skybox_texture_day_(std::move(Cubemap::CreateCubemapTexture("..\\res\\Skyboxes\\PolyverseSkies-BlueSky", file_extension))),
	skybox_texture_night_(std::move(Cubemap::CreateCubemapTexture(file_name_no_extensions, file_extension))),
	skybox_shader_(std::make_unique<Shader>(SKYBOX_VERTEX_SHADER_PATH, SKYBOX_FRAGMENT_SHADER_PATH))
{
	glGenVertexArrays(1, &vertex_array_object_);
	glGenBuffers(1, &vertex_buffer_object_);

	glBindVertexArray(vertex_array_object_);

	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_object_);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

	glBindVertexArray(0);
}
Skybox::~Skybox()
{}


void Skybox::Draw(const Camera& camera, const float& blend_value)
{
	// Change our depth function so that we draw the skybox behind everything.
	glDepthFunc(GL_LEQUAL);


	// Update the skybox shader.
	skybox_shader_->Bind();
	glm::mat4 view = glm::mat4(glm::mat3(camera.get_view())); // We're casting the view matrix into a mat3 then back to a mat4 in order to remove the translations, but preserve rotation transformations.
	skybox_shader_->set_mat_4("viewMatrix", view);
	skybox_shader_->set_mat_4("projectionMatrix", camera.get_projection());

	skybox_shader_->set_float("skybox_blend_value", blend_value);


	// Prepare to draw the skybox cube.
	glBindVertexArray(vertex_array_object_);


	// Bind the Day (skybox1) & Night (skybox2) Cubemaps.
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, skybox_texture_day_->get_texture_id());
	skybox_shader_->set_int("skybox1", 0);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_CUBE_MAP, skybox_texture_night_->get_texture_id());
	skybox_shader_->set_int("skybox2", 1);


	// Draw the Skybox Cube.
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);
	glActiveTexture(GL_TEXTURE0);

	// Reset the depth function.
	glDepthFunc(GL_LESS);
}
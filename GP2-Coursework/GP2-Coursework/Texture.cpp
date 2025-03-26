#include "Texture.h"

Texture::Texture(const GLuint& texture_id, const TextureType& texture_type, const std::string& file_path) : texture_handler_(texture_id), texture_type_(texture_type), file_path_(file_path)
{}

Texture::~Texture()
{
	glDeleteTextures(1, &texture_handler_); // number of and address of textures
}
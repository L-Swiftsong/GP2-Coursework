#include "Texture.h"

Texture::Texture(const TextureType& texture_type, const std::string& file_path) : texture_id_(NULL), texture_type_(texture_type), file_path_(file_path)
{}
Texture::Texture(const GLuint& texture_id, const TextureType& texture_type, const std::string& file_path) : texture_id_(texture_id), texture_type_(texture_type), file_path_(file_path)
{}

Texture::~Texture()
{
	glDeleteTextures(1, &texture_id_); // number of and address of textures
}


void Texture::set_texture_id(const GLuint& newValue) { texture_id_ = newValue; }
GLuint Texture::get_texture_id() const { return texture_id_; }

TextureType Texture::get_texture_type() const { return texture_type_; }

std::string Texture::get_file_path() const { return file_path_; }
#include "Texture.h"

Texture::Texture(const TextureType& texture_type, const std::string& file_path) : texture_id_(NULL), texture_type_(texture_type), file_path_(file_path), file_name_(file_path.substr(0, file_path.find_last_of('\\')))
{}
Texture::Texture(const TextureType& texture_type, const std::string& file_path, const std::string& file_name) : texture_id_(NULL), texture_type_(texture_type), file_path_(file_path), file_name_(file_name)
{}
Texture::Texture(const GLuint& texture_id, const TextureType& texture_type, const std::string& file_path) : texture_id_(texture_id), texture_type_(texture_type), file_path_(file_path), file_name_(file_path.substr(0, file_path.find_last_of('\\')))
{}
Texture::Texture(const GLuint& texture_id, const TextureType& texture_type, const std::string& file_path, const std::string& file_name) : texture_id_(texture_id), texture_type_(texture_type), file_path_(file_path), file_name_(file_name)
{}

Texture::~Texture()
{
	glDeleteTextures(1, &texture_id_); // Number of and address of textures
}


Texture* Texture::GenerateTexture(const TextureType& texture_type, const std::string& file_path, const std::string& file_name)
{
	Texture* texture = new Texture(texture_type, file_path, file_name);
	SetupTexture(texture);
	return texture;
}
void Texture::SetupTexture(Texture* texture)
{
	int width, height, numComponents; //width, height, and no of components of image
	unsigned char* imageData = stbi_load((texture->get_file_path()).c_str(), &width, &height, &numComponents, 4); //load the image and store the data

	if (imageData == NULL)
	{
		std::cerr << "Texture load failed " << texture->get_file_path() << std::endl;
	}

	// Generate our texture.
	GLuint texture_id;
	glGenTextures(1, &texture_id); // number of and address of textures
	glBindTexture(GL_TEXTURE_2D, texture_id); //bind texture - define type 

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); // wrap texture outside width
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT); // wrap texture outside height

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); // linear filtering for minification (texture is smaller than area)
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); // linear filtering for magnifcation (texture is larger)

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, imageData); // Target, Mipmapping Level, Pixel Format, Width, Height, Border Size, Input Format, Data Type of Texture, Image Data.

	stbi_image_free(imageData);


	// Return our Texture ID.
	texture->set_texture_id(texture_id);
}


void Texture::set_texture_id(const GLuint& newValue) { texture_id_ = newValue; }
GLuint Texture::get_texture_id() const { return texture_id_; }

TextureType Texture::get_texture_type() const { return texture_type_; }

std::string Texture::get_file_path() const { return file_path_; }
std::string Texture::get_file_name() const { return file_name_; }
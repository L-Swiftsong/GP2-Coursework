#pragma once

#include <GL\glew.h>
#include <string>

enum TextureType
{
	kDiffuse,
	kSpecular,
	kNormal,
	kHeight,
};

class Texture
{
public:
	Texture(const GLuint& texture_id, const TextureType& texture_type, const std::string& file_path);
	~Texture();


	GLuint get_texture_id() const { return texture_handler_; }
	TextureType get_texture_type() const { return texture_type_; }
	std::string get_file_path() const { return file_path_; }


protected:
private:
	GLuint texture_handler_;
	TextureType texture_type_;
	std::string file_path_; // We're storing the path of the texture to compare when loading other textures on the same model.
};


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
	Texture(const TextureType& texture_type, const std::string& file_path);
	Texture(const GLuint& texture_id, const TextureType& texture_type, const std::string& file_path);
	~Texture();


	void set_texture_id(const GLuint& newValue);
	GLuint get_texture_id() const;

	TextureType get_texture_type() const;
	
	std::string get_file_path() const;


protected:
private:
	GLuint texture_id_;
	TextureType texture_type_;
	std::string file_path_; // We're storing the path of the texture to compare when loading other textures on the same model.
};


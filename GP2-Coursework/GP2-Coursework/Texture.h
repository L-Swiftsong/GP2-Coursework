#pragma once

#include <GL\glew.h>
#include <string>
#include "stb_image.h"
#include <iostream>

enum TextureType
{
	kDiffuse,
	kSpecular,
	kMetallic,
	kRoughness,
	kNormal,
	kDisplacement,

	kSkybox,
};


class Texture
{
public:
	Texture(const TextureType& texture_type, const std::string& file_path);
	Texture(const TextureType& texture_type, const std::string& file_path, const std::string& file_name);
	Texture(const GLuint& texture_id, const TextureType& texture_type, const std::string& file_path);
	Texture(const GLuint& texture_id, const TextureType& texture_type, const std::string& file_path, const std::string& file_name);
	~Texture();

	static Texture* GenerateTexture(const TextureType& texture_type, const std::string& file_path, const std::string& file_name);
	static void SetupTexture(Texture* texture);


	void set_texture_id(const GLuint& newValue);
	GLuint get_texture_id() const;

	TextureType get_texture_type() const;
	
	std::string get_file_path() const;
	std::string get_file_name() const;


protected:
private:
	Texture(const Texture&) = delete;

	GLuint texture_id_;
	TextureType texture_type_;
	std::string file_path_; // We're storing the path of the texture for loading after a texture has been found.
	std::string file_name_; // We're storing the name of the texture to compare when loading other textures on the same model.
};


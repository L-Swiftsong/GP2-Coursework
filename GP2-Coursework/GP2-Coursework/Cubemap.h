#pragma once
#include "Texture.h"

#include <GL\glew.h>
#include "stb_image.h"
#include <iostream>
#include <vector>

#define CUBE_FACE_COUNT 6

class Cubemap
{
public:
	static Texture* CreateCubemapTexture(const std::string& file_path_no_extension, const std::string& file_type);

private:
	static std::string GetFaceName(const std::string file_name, const unsigned int face_index, const std::string& file_type);
};


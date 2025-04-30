#include "Cubemap.h"

Texture Cubemap::CreateCubemapTexture(const std::string& file_path_no_extension, const std::string& file_type)
{
    // Load our Cubemap Faces.
    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    int width, height, nrChannels;
    for (unsigned int i = 0; i < CUBE_FACE_COUNT; i++)
    {
        std::string face_name = GetFaceName(file_path_no_extension, i, file_type);
        unsigned char* data = stbi_load(face_name.c_str(), &width, &height, &nrChannels, 0);
        if (data)
        {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data
            );
            stbi_image_free(data);

            std::cout << "Cubemap Texture Loaded from Path: " << face_name << std::endl;
        }
        else
        {
            std::cout << "Cubemap Texture Failed to Load from Path: " << face_name << std::endl;
            stbi_image_free(data);
        }
    }

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);


    // Create and return our Cubemap Texture.
    return Texture(textureID, TextureType::kSkybox, file_path_no_extension);
};

std::string Cubemap::GetFaceName(const std::string file_name, const unsigned int face_index, const std::string& file_type)
{
    switch (face_index)
    {
        case 0: return file_name + "_right" + file_type;
        case 1: return file_name + "_left" + file_type;
        case 2: return file_name + "_top" + file_type;
        case 3: return file_name + "_bottom" + file_type;
        case 4: return file_name + "_back" + file_type;
        case 5: return file_name + "_front" + file_type;
    }
}

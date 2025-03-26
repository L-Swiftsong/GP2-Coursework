#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "stb_image.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "Mesh.h"
#include "Shader.h"

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>

class Model
{
public:
	Model(const std::string& file_path, const bool gamma = false);

	void Draw(Shader& shader);
	void ReloadMaterials();


	std::vector<Texture> textures_loaded;
	std::vector<Mesh> meshes;
	std::string directory;
	bool gamma_correction;

private:
	void LoadModel(const std::string& file_path);
	void ProcessNode(const aiNode* node, const aiScene* scene);
	Mesh ProcessMesh(const aiMesh* mesh, const aiScene* scene);

	std::vector<std::shared_ptr<Texture>> LoadMaterialTextures(const aiMaterial* mat, const aiTextureType type, const TextureType texture_type);
	GLuint TextureFromFile(const std::string& file_path, const std::string& directory, bool gamma);


	std::vector<Mesh> meshes_;
	std::string directory_;
};


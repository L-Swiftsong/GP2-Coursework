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
	Model(const std::string& file_path);
	Model(const std::string& file_path, const std::vector<Texture> textureOverrides);

	void Draw(const Shader& shader);


	std::vector<std::shared_ptr<Texture>> textures_loaded;
	std::vector<Mesh> meshes;
	std::string directory;

private:
	void LoadModel(const std::string& file_path, const bool useOverrideTextures = false);
	void ProcessNode(const aiNode* node, const aiScene* scene, const bool useOverrideTextures);
	Mesh ProcessMesh(const aiMesh* mesh, const aiScene* scene, const bool useOverrideTextures);

	std::vector<Vertex> GetMeshVertices(const aiMesh* mesh);
	std::vector<unsigned int> GetMeshIndices(const aiMesh* mesh);
	std::vector<std::shared_ptr<Texture>> GetMeshTextures(const aiMesh* mesh, const aiScene* scene);


	std::vector<std::shared_ptr<Texture>> PrepareMaterialTextures(const aiMaterial* mat, const aiTextureType type, const TextureType texture_type);
	void LoadMaterialTextures(const std::string& directory);
	GLuint TextureFromFile(const std::string& file_path, const std::string& directory);


	std::vector<Mesh> meshes_;
	std::string directory_;
};


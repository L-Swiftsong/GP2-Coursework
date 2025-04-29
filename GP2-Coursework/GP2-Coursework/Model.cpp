#include "Model.h"

Model::Model(const std::string& file_path)
{
	LoadModel(file_path);

    // We're delaying the actual loading of textures from the creation of the meshes as at some point during our mesh creation process we lose access to them through OpenGL.
    // By delaying their initialisation till here, we retain the access.
    LoadMaterialTextures(this->directory_);
}
Model::Model(const std::string& file_path, const std::vector<Texture> texture_overrides)
{
    // Load the texture overrides.
    for (int i = 0; i < texture_overrides.size(); ++i)
    {
        textures_loaded_.push_back(std::make_shared<Texture>(texture_overrides[i]));
    }

    // Load our model.
    LoadModel(file_path, true);

    // Initialise our Textures.
    LoadMaterialTextures("..\\res");
}
Model::~Model()
{}



void Model::Draw(const Shader& shader)
{
	for (unsigned int i = 0; i < meshes_.size(); ++i)
	{
		meshes_[i]->Draw(shader);
	}
}


// Loads a model with supported ASSIMP extensions from file and stores the resulting meshes in the meshes vector.
void Model::LoadModel(const std::string& file_path, const bool useOverrideTextures)
{
	// Read file via ASSIMP.
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(file_path, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);

	// Check for errors,
	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		std::cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << std::endl;
		return;
	}

	// Retrieve the directory path of the filepath.
	directory_ = file_path.substr(0, file_path.find_last_of('\\'));

    // Initialise our 'meshes_' vector.
    meshes_ = std::vector<std::unique_ptr<Mesh>>(scene->mNumMeshes);

	// Process ASSIMP's root node recursively.
    int mesh_iterator = 0;
	ProcessNode(scene->mRootNode, scene, &mesh_iterator, useOverrideTextures);
}

// Processes a node in a recursive fashion. Processes each individual mesh located at the node and repeats this process on its children nodes (if any).
void Model::ProcessNode(const aiNode* node, const aiScene* scene, int* mesh_vector_iterator, const bool useOverrideTextures)
{
    // Process each mesh located at the current node.
	for (unsigned int i = 0; i < node->mNumMeshes; i++)
	{
		// The node object only contains indices to index the actual objects in the scene. 
		// The scene contains all the data, node is just to keep stuff organized (like relations between nodes).
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];

        meshes_[(*mesh_vector_iterator)++] = ProcessMesh(mesh, scene, useOverrideTextures);
	}

	// After we've processed all of the meshes directly under this node (if any) we then recursively process each of the node's children nodes.
	for (unsigned int i = 0; i < node->mNumChildren; i++)
	{
		ProcessNode(node->mChildren[i], scene, mesh_vector_iterator, useOverrideTextures);
	}
}
std::unique_ptr<Mesh> Model::ProcessMesh(const aiMesh* mesh, const aiScene* scene, const bool useOverrideTextures)
{
    if (useOverrideTextures == false)
    {
        // Return a mesh object created from the extracted mesh data
        return std::make_unique<Mesh>(Mesh(GetMeshVertices(mesh), GetMeshIndices(mesh), GetMeshTextures(mesh, scene)));
    }
    else
    {
        return std::make_unique<Mesh>(Mesh(GetMeshVertices(mesh), GetMeshIndices(mesh), textures_loaded_));
    }
}
std::vector<Vertex> Model::GetMeshVertices(const aiMesh* mesh)
{
    std::vector<Vertex> vertices;

    // Walk through each of the mesh's vertices.
    for (unsigned int i = 0; i < mesh->mNumVertices; i++)
    {
        Vertex vertex;
        glm::vec3 vector; // We declare a placeholder vector since assimp uses its own vector class that doesn't directly convert to glm's vec3 class so we transfer the data to this placeholder glm::vec3 first.

        // Get Positions.
        vector.x = mesh->mVertices[i].x;
        vector.y = mesh->mVertices[i].y;
        vector.z = mesh->mVertices[i].z;
        vertex.Position = vector;

        // Get Normals.
        if (mesh->HasNormals())
        {
            vector.x = mesh->mNormals[i].x;
            vector.y = mesh->mNormals[i].y;
            vector.z = mesh->mNormals[i].z;
            vertex.Normal = vector;
        }

        // Get Texture Coordinates
        if (mesh->mTextureCoords[0]) // Does the mesh contain texture coordinates?
        {
            glm::vec2 vec;
            // A vertex can contain up to 8 different texture coordinates. We thus make the assumption that we won't 
            //      use models where a vertex can have multiple texture coordinates so we always take the first set (0).
            vec.x = mesh->mTextureCoords[0][i].x;
            vec.y = mesh->mTextureCoords[0][i].y;
            vertex.TexCoords = vec;

            // Tangent
            vector.x = mesh->mTangents[i].x;
            vector.y = mesh->mTangents[i].y;
            vector.z = mesh->mTangents[i].z;
            vertex.Tangent = vector;

            // Bitangent
            vector.x = mesh->mBitangents[i].x;
            vector.y = mesh->mBitangents[i].y;
            vector.z = mesh->mBitangents[i].z;
            vertex.Bitangent = vector;
        }
        else
        {
            // No Texture Coordinates.
            vertex.TexCoords = glm::vec2(0.0f, 0.0f);
        }

        vertices.push_back(vertex);
    }

    return vertices;
}
std::vector<unsigned int> Model::GetMeshIndices(const aiMesh* mesh)
{
    std::vector<unsigned int> indices;

    // Now wak through each of the mesh's faces (a face is a mesh its triangle) and retrieve the corresponding vertex indices.
    for (unsigned int i = 0; i < mesh->mNumFaces; i++)
    {
        aiFace face = mesh->mFaces[i];

        // Retrieve all indices of the face and store them in the indices vector.
        for (unsigned int j = 0; j < face.mNumIndices; j++)
            indices.push_back(face.mIndices[j]);
    }

    return indices;
}
std::vector<std::shared_ptr<Texture>> Model::GetMeshTextures(const aiMesh* mesh, const aiScene* scene)
{
    std::vector<std::shared_ptr<Texture>> textures;

    // Process materials.
    aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

    // Note: We're assuming a convention for sampler names in the shaders.
    // View the '.ReadMe' for more information.
    // Diffuse maps.
    std::vector<std::shared_ptr<Texture>> diffuseMaps = PrepareMaterialTextures(material, aiTextureType_DIFFUSE, TextureType::kDiffuse);
    textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
    // Specular maps.
    std::vector<std::shared_ptr<Texture>> specularMaps = PrepareMaterialTextures(material, aiTextureType_SPECULAR, TextureType::kSpecular);
    textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
    // Metallic maps.
    std::vector<std::shared_ptr<Texture>> metallicMaps = PrepareMaterialTextures(material, aiTextureType_METALNESS, TextureType::kMetallic);
    textures.insert(textures.end(), metallicMaps.begin(), metallicMaps.end());
    // Roughness maps.
    std::vector<std::shared_ptr<Texture>> roughnessMaps = PrepareMaterialTextures(material, aiTextureType_DIFFUSE_ROUGHNESS, TextureType::kRoughness);
    textures.insert(textures.end(), roughnessMaps.begin(), roughnessMaps.end());
    // Normal maps.
    std::vector<std::shared_ptr<Texture>> normalMaps = PrepareMaterialTextures(material, aiTextureType_HEIGHT, TextureType::kNormal);
    textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
    // Displacement maps.
    std::vector<std::shared_ptr<Texture>> displacementMaps = PrepareMaterialTextures(material, aiTextureType_DISPLACEMENT, TextureType::kDisplacement);
    textures.insert(textures.end(), displacementMaps.begin(), displacementMaps.end());

    return textures;
}

std::vector<std::shared_ptr<Texture>> Model::PrepareMaterialTextures(const aiMaterial* mat, const aiTextureType type, const TextureType texture_type)
{
    std::vector<std::shared_ptr<Texture>> textures;

    // (Debug) Output Type Name.
    std::string type_name = "";
    switch (type)
    {
        case aiTextureType_DIFFUSE:     type_name = "Diffuse"; break;
        case aiTextureType_SPECULAR:    type_name = "Specular"; break;
        case aiTextureType_AMBIENT:     type_name = "Ambient"; break;
        case aiTextureType_EMISSIVE:    type_name = "Emissive"; break;
        case aiTextureType_HEIGHT:      type_name = "Height"; break;
        case aiTextureType_NORMALS:     type_name = "Normals"; break;
        case aiTextureType_SHININESS:   type_name = "Shininess"; break;
        case aiTextureType_OPACITY:     type_name = "Transparency"; break;
        case aiTextureType_DISPLACEMENT:type_name = "Displacement"; break;
        case aiTextureType_LIGHTMAP:    type_name = "Lightmap"; break;
        case aiTextureType_REFLECTION:  type_name = "Reflection"; break;

        // PBR.
        case aiTextureType_BASE_COLOR:          type_name = "(PBR) Base Colour"; break;
        case aiTextureType_NORMAL_CAMERA:       type_name = "(PBR) Normal"; break;
        case aiTextureType_EMISSION_COLOR:      type_name = "(PBR) Emissive Colour"; break;
        case aiTextureType_METALNESS:           type_name = "(PBR) Metalness"; break;
        case aiTextureType_DIFFUSE_ROUGHNESS:   type_name = "(PBR) Roughness"; break;
        case aiTextureType_AMBIENT_OCCLUSION:   type_name = "(PBR) Ambient Occlusion"; break;

        //case aiTextureType_DIFFUSE:     type_name = ""; break;
        default: type_name = "<Unset Debug Value>"; break;
    }
    std::cout << "Searching For Type: " << type_name << std::endl;


    for (unsigned int i = 0; i < mat->GetTextureCount(type); ++i)
    {
        aiString str;
        mat->GetTexture(type, i, &str);
        
        std::cout << "Searching For Texture: " << str.C_Str() << std::endl;

        // Check if a texture was loaded before and if so, continue to next iteration (Skip loading already loaded textures).
        bool skip = false;
        for (unsigned int j = 0; j < textures_loaded_.size(); ++j)
        {

            if (std::strcmp(textures_loaded_[j]->get_file_path().data(), str.C_Str()) == 0)
            {
                // A texture with the same filepath has already been loaded, continue to next one (Optimization).
                textures.push_back(textures_loaded_[j]);
                skip = true;
                break;
            }
        }

        if (!skip)
        {
            // The texture hasn't been loaded already. Load it but we defer initilising it till after the whole model has loaded.
            std::shared_ptr<Texture> texture = std::make_shared<Texture>(Texture(texture_type, str.C_Str()));
            textures.push_back(texture);

            std::cout << "Loaded Texture: " << str.C_Str() << std::endl;

            // Store it as a texture loaded for entire model so that we can initialise it later.
            textures_loaded_.push_back(texture);
        }
    }

    return textures;
}

void Model::LoadMaterialTextures(const std::string& directory)
{
    for (unsigned int i = 0; i < textures_loaded_.size(); ++i)
    {
        textures_loaded_[i]->set_texture_id(TextureFromFile(textures_loaded_[i]->get_file_path(), directory));
    }
}
GLuint Model::TextureFromFile(const std::string& file_name, const std::string& directory)
{
    std::string filePath = directory + '\\' + file_name;

    int width, height, numComponents; //width, height, and no of components of image
    unsigned char* imageData = stbi_load((filePath).c_str(), &width, &height, &numComponents, 4); //load the image and store the data

    if (imageData == NULL)
    {
        std::cerr << "texture load failed" << filePath << std::endl;
    }

    GLuint texture_id;
    glGenTextures(1, &texture_id); // number of and address of textures
    glBindTexture(GL_TEXTURE_2D, texture_id); //bind texture - define type 

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); // wrap texture outside width
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT); // wrap texture outside height

    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); // linear filtering for minification (texture is smaller than area)
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); // linear filtering for magnifcation (texture is larger)

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, imageData); //Target, Mipmapping Level, Pixel Format, Width, Height, Border Size, Input Format, Data Type of Texture, Image Data

    stbi_image_free(imageData);

    return texture_id;
}
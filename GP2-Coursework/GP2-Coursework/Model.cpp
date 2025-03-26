#include "Model.h"

Model::Model(const std::string& file_path, const bool gamma) : gamma_correction(gamma)
{
	LoadModel(file_path);

    // For some reason, between the time that the materials are generated and the model is finished creation we lose access to them through OpenGL.
    // Reloading (But not rebinding) these materials solves this, but we should find out why this is occuring and try to fix it.
    ReloadMaterials();
}

//void Model::Draw(const Shader& shader)
void Model::Draw(Shader& shader)
{
    //shader.set_int("texture_diffuse1", 0);

	for (unsigned int i = 0; i < meshes.size(); ++i)
	{
		meshes[i].Draw(shader);
	}
}
void Model::ReloadMaterials()
{
    for (int i = 0; i < textures_loaded.size(); ++i)
    {
        std::string filePath = directory + '\\' + textures_loaded[i].get_file_path();
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
    }
}


// Loads a model with supported ASSIMP extensions from file and stores the resulting meshes in the meshes vector.
void Model::LoadModel(const std::string& file_path)
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
	directory = file_path.substr(0, file_path.find_last_of('\\'));

	// Process ASSIMP's root node recursively.
	ProcessNode(scene->mRootNode, scene);
}

// Processes a node in a recursive fashion. Processes each individual mesh located at the node and repeats this process on its children nodes (if any).
void Model::ProcessNode(const aiNode* node, const aiScene* scene)
{
	// Process each mesh located at the current node.
	for (unsigned int i = 0; i < node->mNumMeshes; i++)
	{
		// The node object only contains indices to index the actual objects in the scene. 
		// The scene contains all the data, node is just to keep stuff organized (like relations between nodes).
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		meshes.push_back(ProcessMesh(mesh, scene));
	}

	// After we've processed all of the meshes directly under this node (if any) we then recursively process each of the node's children nodes.
	for (unsigned int i = 0; i < node->mNumChildren; i++)
	{
		ProcessNode(node->mChildren[i], scene);
	}
}
Mesh Model::ProcessMesh(const aiMesh* mesh, const aiScene* scene)
{
    // data to fill
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<std::shared_ptr<Texture>> textures;

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

    // Now wak through each of the mesh's faces (a face is a mesh its triangle) and retrieve the corresponding vertex indices.
    for (unsigned int i = 0; i < mesh->mNumFaces; i++)
    {
        aiFace face = mesh->mFaces[i];

        // Retrieve all indices of the face and store them in the indices vector.
        for (unsigned int j = 0; j < face.mNumIndices; j++)
            indices.push_back(face.mIndices[j]);
    }

    // Process materials.
    aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
    // Note: We're assuming a convention for sampler names in the shaders. Each diffuse texture should
    //  be named as 'texture_diffuseN' where N is a sequential number ranging from 1 to MAX_SAMPLER_NUMBER. 
    //  Same applies to other textures as the following list summarizes:
    //      diffuse: texture_diffuseN
    //      specular: texture_specularN
    //      normal: texture_normalN
    //      height: texture_heightN

    // 1. Diffuse maps.
    std::vector<std::shared_ptr<Texture>> diffuseMaps = LoadMaterialTextures(material, aiTextureType_DIFFUSE, TextureType::kDiffuse);
    textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
    // 2. Specular maps.
    std::vector<std::shared_ptr<Texture>> specularMaps = LoadMaterialTextures(material, aiTextureType_SPECULAR, TextureType::kSpecular);
    textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
    // 3. Normal maps.
    std::vector<std::shared_ptr<Texture>> normalMaps = LoadMaterialTextures(material, aiTextureType_HEIGHT, TextureType::kNormal);
    textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
    // 4. Height maps.
    std::vector<std::shared_ptr<Texture>> heightMaps = LoadMaterialTextures(material, aiTextureType_AMBIENT, TextureType::kHeight);
    textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());

    // return a mesh object created from the extracted mesh data
    return Mesh(vertices, indices, textures);
}

std::vector<std::shared_ptr<Texture>> Model::LoadMaterialTextures(const aiMaterial* mat, const aiTextureType type, const TextureType texture_type)
{
    std::vector<std::shared_ptr<Texture>> textures;
    for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
    {
        aiString str;
        mat->GetTexture(type, i, &str);
        
        // Check if a texture was loaded before and if so, continue to next iteration (Skip loading already loaded textures).
        bool skip = false;
        for (unsigned int j = 0; j < textures_loaded.size(); j++)
        {
            if (std::strcmp(textures_loaded[j].get_file_path().data(), str.C_Str()) == 0)
            {
                // A texture with the same filepath has already been loaded, continue to next one (Optimization).
                textures.push_back(std::make_shared<Texture>(textures_loaded[j]));
                skip = true;
                break;
            }
        }

        if (!skip)
        {
            // The texture hasn't been loaded already. Load it.
            Texture texture = Texture(TextureFromFile(str.C_Str(), this->directory, gamma_correction), texture_type, str.C_Str());
            textures.push_back(std::make_shared<Texture>(texture));

            // Store it as a texture loaded for entire model to ensure we won't unnecessarily load duplicate textures.
            textures_loaded.push_back(texture);
        }
    }

    return textures;
}
GLuint Model::TextureFromFile(const std::string& file_name, const std::string& directory, bool gamma)
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

    /*GLuint texture_id;
    glGenTextures(1, &texture_id);
    std::cout << file_name << ": " << texture_id << std::endl;

    int width, height, nrComponents;
    unsigned char* data = stbi_load(filePath.c_str(), &width, &height, &nrComponents, 0);
    if (data)
    {
        GLenum format = GL_RED;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, texture_id);
        //glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);

        stbi_image_free(data);
    }
    else
    {
        std::cout << "Texture failed to load at path: " << file_name << std::endl;
        stbi_image_free(data);
    }*/

    return texture_id;
}
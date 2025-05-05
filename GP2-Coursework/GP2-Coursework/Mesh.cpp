#include "Mesh.h"

Mesh::Mesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices, const std::vector<std::shared_ptr<Texture>>& textures)
{
	this->vertices_ = vertices;
	this->indices_ = indices;
	this->textures_ = textures;

	SetupMesh();
}
Mesh::~Mesh()
{}


void Mesh::Draw(const Shader& shader)
{
    // bind appropriate textures
    unsigned int diffuseNr = 1;
    unsigned int specularNr = 1;
    unsigned int metallicNr = 1;
    unsigned int roughnessNr = 1;
    unsigned int normalNr = 1;
    unsigned int displacementNr = 1;
    for (unsigned int i = 0; i < textures_.size(); i++)
    {
        glActiveTexture(GL_TEXTURE0 + i); // active proper texture unit before binding

        // Retrieve texture number to format name (the N in diffuse_textureN)
        std::string name;
        TextureType type = textures_[i]->get_texture_type();
        switch (type)
        {
            case (TextureType::kDiffuse):       name = "texture_diffuse" + std::to_string(diffuseNr++);             break;
            case (TextureType::kSpecular):      name = "texture_specular" + std::to_string(specularNr++);           break;
            case (TextureType::kMetallic):      name = "texture_metallic" + std::to_string(metallicNr++);           break;
            case (TextureType::kRoughness):     name = "texture_roughness" + std::to_string(roughnessNr++);         break;
            case (TextureType::kNormal):        name = "texture_normal" + std::to_string(normalNr++);               break;
            case (TextureType::kDisplacement):  name = "texture_displacement" + std::to_string(displacementNr++);   break;
        }

        // now set the sampler to the correct texture unit
        glUniform1i(glGetUniformLocation(shader.get_shader_id(), name.c_str()), i);

        // and finally bind the texture
        glBindTexture(GL_TEXTURE_2D, textures_[i]->get_texture_id());
    }

    glUniform1i(glGetUniformLocation(shader.get_shader_id(), "has_normal"), normalNr > 1);


    // draw mesh
    glBindVertexArray(vertex_array_object_);
    glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(indices_.size()), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    // always good practice to set everything back to defaults once configured.
    glActiveTexture(GL_TEXTURE0);
}


void Mesh::SetupMesh()
{
    // create buffers/arrays
    glGenVertexArrays(1, &vertex_array_object_);
    glGenBuffers(1, &vertex_buffer_object_);
    glGenBuffers(1, &element_buffer_object_);

    glBindVertexArray(vertex_array_object_);
    // load data into vertex buffers
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_object_);
    // A great thing about structs is that their memory layout is sequential for all its items.
    // The effect is that we can simply pass a pointer to the struct and it translates perfectly to a glm::vec3/2 array which
    // again translates to 3/2 floats which translates to a byte array.
    glBufferData(GL_ARRAY_BUFFER, vertices_.size() * sizeof(Vertex), &vertices_[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, element_buffer_object_);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices_.size() * sizeof(unsigned int), &indices_[0], GL_STATIC_DRAW);

    // set the vertex attribute pointers
    // vertex Positions
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    // vertex normals
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
    // vertex texture coords
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));
    // vertex tangent
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Tangent));
    // vertex bitangent
    glEnableVertexAttribArray(4);
    glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Bitangent));
    // ids
    glEnableVertexAttribArray(5);
    glVertexAttribIPointer(5, 4, GL_INT, sizeof(Vertex), (void*)offsetof(Vertex, m_BoneIDs));

    // weights
    glEnableVertexAttribArray(6);
    glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, m_Weights));
    


    glBindVertexArray(0);
}

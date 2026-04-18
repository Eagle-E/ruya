#ifndef GPUVAULT_H
#define GPUVAULT_H


#include <memory>
#include <vector>
#include <string>
#include <unordered_map>

#include "render/texture.hpp"
#include "scene/image.hpp"
#include "scene/mesh.h"
#include "scene/vault.hpp"

using ruya::scene::Image;
using ruya::scene::ImageID;
using ruya::scene::Mesh;
using ruya::scene::MeshID;
using ruya::scene::Vault;
using ruya::render::Texture;



// using ruya::scene::Image;
// using ruya::Texture;


namespace ruya::render
{
    struct MeshGPU
    {
        GLuint vao = 0;
        GLuint vbo = 0;
        GLuint ebo = 0;
        GLsizei index_count = 0;

        bool is_valid() const
        {
            return vao != 0; // 0 is universally invalid in OpenGL
        }
    };
    
    constexpr MeshGPU INVALID_MESH_HANDLE = {
        .vao = 0,
        .vbo = 0,
        .ebo = 0,
        .index_count = 0
    };


    /* Responsible for loading assets to the gpu and keeping track of their handles.

    TODO - optimization: pack meshes and textures into a single buffer
    */
    struct GPUVault
    {
        std::vector<MeshGPU> meshes;
        std::vector<Texture> textures;

        ~GPUVault()
        {
            for (MeshGPU& mesh : meshes)
            {
                glDeleteVertexArrays(1, &mesh.vao);
                glDeleteBuffers(1, &mesh.vbo);
                glDeleteBuffers(1, &mesh.ebo);
            }
            for (Texture& texture : textures)
                glDeleteTextures(1, &texture.id);
        }
    };

    inline void release_buffers(MeshGPU & mesh_handle)
    {
        glDeleteVertexArrays(1, &mesh_handle.vao);
        glDeleteBuffers(1, &mesh_handle.vbo);
        glDeleteBuffers(1, &mesh_handle.ebo);
        mesh_handle.vao = 0;
        mesh_handle.vbo = 0;
        mesh_handle.ebo = 0;
        mesh_handle.index_count = 0;
    }

    /*
    * Does the necessary OpenGL calls the create VAO, VBO and EBO for the given mesh.
    * @returns the VAO ID that contains info about the buffers containing the mesh's data.
    */
    inline MeshGPU buffer_mesh(const Mesh& mesh)
    {
        // create a vertex buffer object (VAO) so we don't have to repeat VBO and vertex attribute stuff
        GLuint vaoID;
        glGenVertexArrays(1, &vaoID);
        glBindVertexArray(vaoID);

        // element buffer
        GLuint eboID;
        glGenBuffers(1, &eboID);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, eboID);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh.size_faces(), mesh.faces.data(), GL_STATIC_DRAW);

        // vertex buffer, concatenate mesh data: first vertex data then texture data, ...
        GLuint vboID;
        long int sizeVert = mesh.size_vertices();
        long int sizeNormals = mesh.size_normals();
        long int sizeTex = mesh.size_texture_coords();
        long int meshSize = sizeVert + sizeNormals + sizeTex;
        
        glGenBuffers(1, &vboID); // create a buffer
        glBindBuffer(GL_ARRAY_BUFFER, vboID); // set buffer's type to array buffer
        glBufferData(GL_ARRAY_BUFFER, meshSize, NULL, GL_STATIC_DRAW); // allocate memory and copy vertices to GPU
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeVert, mesh.vertices.data());
        glBufferSubData(GL_ARRAY_BUFFER, sizeVert, sizeNormals, mesh.normals.data());
        glBufferSubData(GL_ARRAY_BUFFER, sizeVert + sizeNormals, sizeTex, mesh.textureCoordinates.data());
        

        // specify vertex attributes, how the data in the VBO should be evaluated
        const GLuint INDEX_VERTEX = 0; // indexes of the attributes used in the vertex shader
		const GLuint INDEX_NORMAL = 1;
		const GLuint INDEX_TEXTURE = 2;

        // TODO: buffer the data such that they're interleaving instead of being concatenated
        glVertexAttribPointer(INDEX_VERTEX, 3, GL_FLOAT, GL_FALSE, 0, (void*)0); // loc data
        glEnableVertexAttribArray(INDEX_VERTEX);
        void * size_vert_offset = reinterpret_cast<void *>(static_cast<uintptr_t>(sizeVert));
        glVertexAttribPointer(INDEX_NORMAL, 3, GL_FLOAT, GL_FALSE, 0, size_vert_offset); // normal data
        glEnableVertexAttribArray(INDEX_NORMAL);
        void * size_vert_pls_normals_offset = reinterpret_cast<void *>(static_cast<uintptr_t>(sizeVert+sizeNormals));
        glVertexAttribPointer(INDEX_TEXTURE, 2, GL_FLOAT, GL_FALSE, 0, size_vert_pls_normals_offset); // texture data
        glEnableVertexAttribArray(INDEX_TEXTURE);

        // unbind vertex array buffer
        glBindVertexArray(0);
        MeshGPU mesh_gpu = {
            .vao = vaoID,
            .vbo = vboID,
            .ebo = eboID,
            .index_count = static_cast<GLsizei>(mesh.faces.size() * 3)
        };
        return mesh_gpu;
    }
    

    inline void sync_vaults(Vault& cpu_vault, GPUVault& gpu_vault)
    {
        if (gpu_vault.meshes.size() < cpu_vault.meshes.size())
            gpu_vault.meshes.resize(cpu_vault.meshes.size());
        if (gpu_vault.textures.size() < cpu_vault.images.size())
            gpu_vault.textures.resize(cpu_vault.images.size());
    }


    /*
    * Get mesh handle corresponding to mesh with given ID, upload to gpu if needed.
    */
    inline MeshGPU resolve_mesh(MeshID mesh_id, const Vault& vault, GPUVault& gpu_vault)
    {
        assert(mesh_id >= 0 && mesh_id < gpu_vault.meshes.size());

        MeshGPU current_handle = gpu_vault.meshes[mesh_id];
        if (current_handle.is_valid())
            return current_handle;
        
        // buffer given mesh
        MeshGPU new_handle = buffer_mesh(vault.meshes[mesh_id]);
        gpu_vault.meshes[mesh_id] = new_handle;
        return new_handle;
    }
    

    inline Texture generate_texture(const Image& image)
    {
        GLuint texture_handle_id=0;
            
        // create opengl texture
        glGenTextures(1, &texture_handle_id);
        glBindTexture(GL_TEXTURE_2D, texture_handle_id);
        
        // texture settings
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); // wrap around in s- and t-axi
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST); // nearest neighbor filtering with best fitting mipmap when minifying
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); // bilinear interpolation when magnifying

        // generate opengl texture and "move it to the GPU" (whether it actually gets moved is driver dependent)
        GLint sourceColorType;
        image.channels == 3 ? sourceColorType = GL_RGB : sourceColorType = GL_RGBA;
        glTexImage2D(GL_TEXTURE_2D, 0, sourceColorType, image.width, image.height, 0, sourceColorType, GL_UNSIGNED_BYTE, image.pixels.data());
        glGenerateMipmap(GL_TEXTURE_2D);

        Texture new_texture = {
            .id = texture_handle_id
        };
        return new_texture;
    }

    /* 
     * Get texture handle corresponding to image with given ID, upload to gpu if needed.
    */
    inline Texture resolve_texture(
        ImageID image_id,
        const Vault& vault,
        GPUVault& gpu_vault
    )
    {
        assert(image_id >= 0 && image_id < gpu_vault.textures.size());

        Texture current_handle = gpu_vault.textures[image_id];
        if (current_handle.is_valid())
            return current_handle;

        // texture doesn't exist, create one and return handle
        Texture new_texture = generate_texture(vault.images[image_id]);
        gpu_vault.textures[image_id] = new_texture;
        return new_texture;
    }

    
} // namespace ruya


#endif // GPUVAULT_H
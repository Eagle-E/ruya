#ifndef VAULT_H
#define VAULT_H


#include <memory>
#include <vector>
#include <string>
#include <unordered_map>
#include <filesystem>
#include "render/texture.hpp"
#include "scene/image.hpp"
#include "scene/mesh.h"
#include "io/image_io.hpp"

using ruya::scene::Mesh;
using ruya::render::Texture;
using ruya::scene::Image;
namespace fs = std::filesystem;


namespace ruya::scene
{
    using MeshID = uint32_t;
    using ImageID = uint32_t;
    using TextureID = uint32_t;

    

    struct Vault
    {
        std::vector<Mesh> meshes; // cpu side
        std::vector<Image> images;
        std::vector<Texture> textures_gpu; // !!!TODO!!! remove gpu stuff from vault, move to renderer.

        std::unordered_map<fs::path, MeshID> mesh_cache;
        std::unordered_map<fs::path, TextureID> texture_cache;

        // TODO
        // MeshID load_mesh(const std::string& path)
        // {
        // }

        ImageID load_image(const fs::path& image_path)
        {
            // load texture image
            Image image = ruya::io::load_image(image_path);
            if (image.pixels.empty())
            {
                std::cerr   << "Error loading texture: " 
                            << stbi_failure_reason() 
                            << "\nTexture path: " 
                            << image_path << std::endl;
                return ImageID(-1); // return invalid ID
            }

            images.push_back(image);
            ImageID new_id = static_cast<ImageID>(images.size() - 1);
            return new_id;
        }

        // TextureID load_texture(const fs::path& texture_path)
        // {
        //     // load texture image
        //     Image image = ruya::io::load_image(texture_path);
        //     if (image.pixels.empty())
        //     {
        //         std::cerr   << "Error loading texture: " 
        //                     << stbi_failure_reason() 
        //                     << "\nTexture path: " 
        //                     << texture_path << std::endl;
        //         return TextureID(-1); // return invalid ID
        //     }

        //     images.push_back(image);
        //     GLuint texture_handle_id=0;
            
        //     // create opengl texture
        //     glGenTextures(1, &texture_handle_id);
        //     glBindTexture(GL_TEXTURE_2D, texture_handle_id);
            
        //     // texture settings
        //     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); // wrap around in s- and t-axi
        //     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        //     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST); // nearest neighbor filtering with best fitting mipmap when minifying
        //     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); // bilinear interpolation when magnifying

        //     // generate opengl texture and "move it to the GPU" (whether it actually gets moved is driver dependent)
        //     GLint sourceColorType;
        //     image.channels == 3 ? sourceColorType = GL_RGB : sourceColorType = GL_RGBA;
        //     glTexImage2D(GL_TEXTURE_2D, 0, sourceColorType, image.width, image.height, 0, sourceColorType, GL_UNSIGNED_BYTE, image.pixels.data());
        //     glGenerateMipmap(GL_TEXTURE_2D);

        //     Texture newTexture = {
        //         .id = texture_handle_id
        //     };
        //     textures_gpu.push_back(newTexture);
        //     TextureID newTextureID = textures_gpu.size()-1;
        //     return newTextureID;
        // }

        MeshID add_mesh(Mesh mesh, const fs::path& mesh_path = "")
        {
            meshes.push_back(std::move(mesh));
            MeshID new_id = static_cast<MeshID>(meshes.size() - 1);

            if (!mesh_path.empty())
                mesh_cache[mesh_path] = new_id;
            
            return new_id;
        }
    };
    
} // namespace ruya


#endif // VAULT_H
#ifndef VAULT_H
#define VAULT_H


#include <filesystem>
#include <limits>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "io/image_io.hpp"
#include "render/texture.hpp"
#include "scene/image.hpp"
#include "scene/mesh.h"

using ruya::render::Texture;
using ruya::scene::Image;
using ruya::scene::Mesh;

namespace fs = std::filesystem;


namespace ruya::scene
{
    using IDType = uint32_t;
    inline IDType NULL_ID = std::numeric_limits<IDType>::max();

    using MeshID = IDType;
    using ImageID = IDType;
    using TextureID = IDType;


    struct Vault
    {
        std::vector<Mesh> meshes; // cpu side
        std::vector<Image> images;

        std::unordered_map<fs::path, MeshID> mesh_cache;
        std::unordered_map<fs::path, TextureID> texture_cache;


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
                return ImageID(NULL_ID); // return invalid ID
            }

            images.push_back(image);
            ImageID new_id = static_cast<ImageID>(images.size() - 1);
            return new_id;
        }

        // TODO
        // MeshID load_mesh(const std::string& path)
        // {
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
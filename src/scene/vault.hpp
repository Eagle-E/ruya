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
    using ImageID = IDType; // TODO: convert to struct?
    constexpr MeshID INVALID_MESH_ID = 0;


    struct Vault
    {
    private:
        std::vector<Mesh> _meshes; // cpu side
        std::vector<Image> _images;
    
    public:
        std::unordered_map<fs::path, MeshID> mesh_cache;
        std::unordered_map<fs::path, ImageID> image_cache;

        Vault()
        {
            Image default_image {
                .width = 1,
                .height = 1,
                .channels = 4, // rgba
                .pixels = {
                    std::byte{225}, // r
                    std::byte{225}, // g
                    std::byte{225}, // b
                    std::byte{255}  // a
                }
            };
            _images.push_back(default_image);
        }

        Mesh& mesh(MeshID id)
        {
            assert(0 <= id && id < _meshes.size());
            return _meshes[id];
        }

        Image& image(ImageID id)
        {
            assert(0 <= id && id < _images.size());
            return _images[id];
        }
        
        size_t size_images() const
        {
            return _images.size();
        } 

        size_t size_meshes() const
        {
            return _meshes.size();
        }

        ImageID load_image(const fs::path& image_path)
        {
            if (image_cache.find(image_path) != image_cache.end())
                return image_cache[image_path];

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

            _images.push_back(image);
            ImageID new_id = static_cast<ImageID>(_images.size() - 1);
            return new_id;
        }

        // TODO
        // MeshID load_mesh(const std::string& path)
        // {
        // }

        MeshID add_mesh(Mesh mesh, const fs::path& mesh_path = "")
        {
            // TODO: check if mesh_path already exists
            _meshes.push_back(std::move(mesh));
            MeshID new_id = static_cast<MeshID>(_meshes.size() - 1);

            if (!mesh_path.empty())
                mesh_cache[mesh_path] = new_id;
            
            return new_id;
        }
    };
    
} // namespace ruya


#endif // VAULT_H
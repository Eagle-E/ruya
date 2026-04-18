#ifndef IMAGE_IO_HPP
#define IMAGE_IO_HPP

#include "scene/image.hpp"
#include "io/stb_image.h"
#include <vector>
#include <filesystem>

using ruya::scene::Image;
namespace fs = std::filesystem;

namespace ruya::io
{
    inline Image load_image(const fs::path& path)
    {
        // load texture data
        int width=0, height=0, channels=0;
        stbi_set_flip_vertically_on_load(true);
        fs::path canonical_path = fs::canonical(path);
        unsigned char* img_data = stbi_load(canonical_path.string().c_str(), &width, &height, &channels, 0);
        
        // check if loading succeeded
        if (img_data == nullptr)
        return Image{};
        
        size_t byte_count = width * height * channels * sizeof(unsigned char);
        Image image = {
            .width = width,
            .height = height,
            .channels = channels,
            .pixels = std::vector<std::byte>(
                reinterpret_cast<std::byte*>(img_data),
                reinterpret_cast<std::byte*>(img_data) + byte_count
            )
        };
        stbi_image_free(img_data);
        return image;
    }
    
}


#endif // IMAGE_IO_HPP
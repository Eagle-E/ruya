#ifndef IMAGE_H
#define IMAGE_H

#include <vector>


namespace ruya::scene
{
    struct Image
    {
        int width    = 0;
        int height   = 0;
        int channels = 0;
        std::vector<std::byte> pixels;
    };

    
}


#endif // IMAGE_H
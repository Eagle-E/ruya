#ifndef PATHS_HPP
#define PATHS_HPP

#include <whereami/whereami++.h>
#include <filesystem>

namespace fs = std::filesystem;


namespace ruya::io
{
    inline const fs::path DIR_EXE {whereami::getExecutablePath().dirname()};
    inline const fs::path DIR_SHADERS {DIR_EXE / "shaders"};
    inline const fs::path DIR_RESOURCES {DIR_EXE / "resources"};
}


#endif //PATHS_HPP
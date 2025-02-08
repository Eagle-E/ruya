#include <iostream>
#include <filesystem>

#include "sample_app.hpp"
#include "engine/core/window.h"
#include <whereami/whereami++.h>

namespace fs = std::filesystem;

int main()
{
	ruya::Window window(1450, 875);
	window.make_context_current();

    fs::path exeDir{whereami::getModulePath().dirname()};
    fs::path moduleDir{whereami::getModulePath().dirname()};

    std::cout << exeDir << std::endl;
    std::cout << moduleDir << std::endl;
    std::cout << std::endl;
    std::cout << whereami::getModulePath() << std::endl;
    std::cout << whereami::getModulePath().basename() << std::endl;
    std::cout << whereami::getModulePath().dirname() << std::endl;
    std::cout << whereami::getExecutablePath() << std::endl;
    std::cout << whereami::getExecutablePath().basename() << std::endl;
    std::cout << whereami::getExecutablePath().dirname() << std::endl;

	ruya::TestApp app(window);

	try
	{
		app.run();
	}
	catch (std::exception e)
	{
		std::cerr << e.what() << std::endl;
		return 1;
	}
	
	return 0;
}

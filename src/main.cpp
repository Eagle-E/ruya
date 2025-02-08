#include <iostream>
#include <filesystem>

#include "test_app.hpp"
#include "engine/core/window.h"
#include <whereami/whereami++.h>

namespace fs = std::filesystem;

int main()
{
	ruya::Window window(1450, 875);
	window.make_context_current();

    fs::path exeDir{whereami::getModulePath().dirname()};
    fs::path moduleDir{whereami::getModulePath().dirname()};

    std::cout << "[INFO] exe dir: " << exeDir << std::endl;
    std::cout << "[INFO] module dir: " << moduleDir << std::endl;

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

#include <iostream>
#include <filesystem>

#include "test_app.hpp"
#include "snake.hpp"
#include "io/window.h"
#include <whereami/whereami++.h>

namespace fs = std::filesystem;

int main()
{
    fs::path exeDir{whereami::getModulePath().dirname()};
    fs::path moduleDir{whereami::getModulePath().dirname()};

    std::cout << "[INFO] exe dir: " << exeDir << std::endl;
    std::cout << "[INFO] module dir: " << moduleDir << std::endl;

	
	try
	{
		// ruya::TestApp app;
		ruya::Snake app;
		app.run();
	}
	catch (const std::runtime_error& e)
	{
        std::cerr << "Runtime error: " << e.what() << '\n';
    }
	catch (std::exception e)
	{
		std::cerr << e.what() << std::endl;
		return 1;
	}
	
	return 0;
}

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






/*
#include <iostream>
#include <glad/glad.h> 
#include <GLFW/glfw3.h>

#include "engine/core/window.h"
#include "engine/scene/mesh.h"
#include "engine/scene/texture.h"
#include "utils/uuid.h"
#include "utils/timer.h"

#include "engine/render/renderer.h"
#include "engine/render/shader.h"

#include "engine/scene/camera.h"
#include "engine/scene/light_source.h"
#include "engine/scene/object.h"
#include "engine/scene/scene.h"
#include "engine/scene/models/cube.h"
#include "engine/scene/models/icosahedron.h"
#include "engine/scene/models/icosphere.hpp"
#include "engine/scene/models/square.h"

using namespace std;


static void glfw_error_callback(int error, const char* description)
{
    fprintf(stderr, "GLFW Error %d: %s\n", error, description);
}


int main()
{
    ruya::Mesh mesh1{};
    ruya::UUID id1{};
    ruya::Texture tex1{};
    ruya::Timer timer{};

    ruya::Window window{1000, 1000};
    ruya::Camera cam{};

    ruya::Object obj{};
    ruya::LightSource light{};
    ruya::Scene scene{};

    ruya::Renderer renderer{nullptr, nullptr, nullptr, nullptr};
    ruya::Shader shader{};

    ruya::models::Cube cube{};
    ruya::models::Icosahedron icosahedron{};
    ruya::models::Icosphere icosphere{};
    ruya::models::Square square{};

    timer.start();
    timer.stop();


    while(!window.should_close())
    {
        glfwPollEvents();

        // Rendering
        int display_w, display_h;
        glfwGetFramebufferSize(window.get_GLFW_window(), &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        glfwSwapBuffers(window.get_GLFW_window());
    }

    return 0;
}
*/
#include "window.h"
#include <iostream>
#include <stdexcept>
#include <string>

#include <imgui/imgui.h>
#include <imgui/backends/imgui_impl_glfw.h>
#include <imgui/backends/imgui_impl_opengl3.h>

namespace
{
	void glfw_error_callback(int error, const char* description)
	{
		fprintf(stderr, "GLFW Error %d: %s\n", error, description);
	}
} 



ruya::io::Window::Window(int width, int height)
	: _width(width), _height(height)
{
	glfwSetErrorCallback(glfw_error_callback);
	if (!glfwInit())
    {
        std::cout << "[GLFW3] initialization failed." << std::endl;
		return;
    }
    else
        std::cout << "[GLFW3] successfully initialized" << std::endl;

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	_glfw_window = glfwCreateWindow(width, height, "opengl window", nullptr, nullptr);
	if (_glfw_window == nullptr)
	{
		// TODO: make it possible to check whether the creation of the window glfw object has failed or not.
		std::cerr << "Could not create GLFW window." << std::endl;
		return;
	}

	make_context_current();

	// init functions pointers to opengl functions
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		throw std::runtime_error("Failed to initialize GLAD (which is responsible for the opengl function pointers)");
	}

	// opengl settings
	glViewport(0, 0, width, height);

	// callback when window is resized
	glfwSetWindowUserPointer(_glfw_window, this);
	glfwSetFramebufferSizeCallback(_glfw_window, resize_callback_static);

    // init cursor
    set_cursor_mode(_cursor_mode);
}

void ruya::io::Window::set_should_close(bool should_close)
{
	glfwSetWindowShouldClose(_glfw_window, should_close);
}

void ruya::io::Window::set_cursor_mode(CursorMode mode)
{
    _cursor_mode = mode;
    auto glfw_cursor_mode = GLFW_CURSOR_DISABLED;

    switch (mode)
    {
    case CursorMode::NORMAL :
        glfw_cursor_mode = GLFW_CURSOR_NORMAL;
        break;
    case CursorMode::HIDDEN :
        glfw_cursor_mode = GLFW_CURSOR_HIDDEN;
        break;
    case CursorMode::DISABLED :
        glfw_cursor_mode = GLFW_CURSOR_DISABLED;
        break;
    default:
        break;
    }

    glfwSetInputMode(_glfw_window, GLFW_CURSOR, glfw_cursor_mode);
}


ruya::io::Window::~Window()
{
    // ImGui_ImplOpenGL3_Shutdown();
    // ImGui_ImplGlfw_Shutdown();
    // ImGui::DestroyContext();

	glfwDestroyWindow(_glfw_window);
	glfwTerminate(); // TODO: should this be removed in case multiple windows are being used ?
}

GLFWwindow* ruya::io::Window::get_GLFW_window()
{
	return _glfw_window;
}

void ruya::io::Window::update()
{
	// update frame
	glfwSwapBuffers(_glfw_window); // swap buffer to the new to-be-rendered buffer

	// check for events
	glfwPollEvents(); // check if there are any events
}


void ruya::io::Window::resize_callback_static(GLFWwindow* window, int width, int height)
{
	static_cast<Window*>(glfwGetWindowUserPointer(window))->resize_callback(window, width, height);
}

void ruya::io::Window::resize_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
	_width = width;
	_height = height;
}






#include "window.h"
#include <iostream>
#include <stdexcept>
#include <string>

namespace
{
	void glfw_error_callback(int error, const char* description)
	{
		fprintf(stderr, "GLFW Error %d: %s\n", error, description);
	}
} 



ruya::Window::Window(int width, int height)
	: mWidth(width), mHeight(height)
{
	// Hint windowing system the version of opengl used. This code is tested on gtx 1050m which supports
	// opengl 4.6 at the moment.
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

	mGLFWwindow = glfwCreateWindow(width, height, "opengl window", nullptr, nullptr);
	if (mGLFWwindow == nullptr)
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
	glfwSetWindowUserPointer(mGLFWwindow, this);
	glfwSetFramebufferSizeCallback(mGLFWwindow, resize_callback_static); // when the window gets resized, the viewport has to be updated
}

ruya::Window::~Window()
{
	glfwDestroyWindow(mGLFWwindow);
	glfwTerminate(); // ? should this be removed in case multiple windows are being used ?
}

GLFWwindow* ruya::Window::get_GLFW_window()
{
	return mGLFWwindow;
}

void ruya::Window::update()
{
	// update frame
	glfwSwapBuffers(mGLFWwindow); // swap buffer to the new to-be-rendered buffer

	// check for events
	glfwPollEvents(); // check if there are any events
	process_inputs();
}

void ruya::Window::add_event_callback(int glfwEventID, VOID_FPTR callback)
{
	// event type is not registered yet
	if (mEventCallbackMap.find(glfwEventID) == mEventCallbackMap.end())
	{
		mEventCallbackMap[glfwEventID] = list<VOID_FPTR>();
	}

	mEventCallbackMap[glfwEventID].push_back(callback);
}

void ruya::Window::remove_event_callback(VOID_FPTR callback)
{
	unordered_map<int, list<VOID_FPTR>>::iterator it = mEventCallbackMap.begin();
	for (it; it != mEventCallbackMap.end(); it++)
	{
		it->second.remove(callback);
	}
}

void ruya::Window::resize_callback_static(GLFWwindow* window, int width, int height)
{
	static_cast<Window*>(glfwGetWindowUserPointer(window))->resize_callback(window, width, height);
}

void ruya::Window::resize_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
	mWidth = width;
	mHeight = height;
}


void ruya::Window::process_inputs()
{		
	// Close window if ESC key is pressed
	if (glfwGetKey(mGLFWwindow, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(mGLFWwindow, true);

	if (glfwGetKey(mGLFWwindow, GLFW_KEY_1) == GLFW_PRESS && allowRenderModeChange)
	{
		switch (renderMode)
		{
		case RenderMode::FILL:
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			renderMode = RenderMode::WIREFRAME;
			break;
		case RenderMode::WIREFRAME:
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			renderMode = RenderMode::FILL;
			break;
		}

		allowRenderModeChange = false;
	}
	else if (glfwGetKey(mGLFWwindow, GLFW_KEY_1) == GLFW_RELEASE)
	{
		allowRenderModeChange = true;
	}

	call_handlers_on_key_press(GLFW_KEY_UP);
	call_handlers_on_key_press(GLFW_KEY_DOWN);
	call_handlers_on_key_press(GLFW_KEY_LEFT);
	call_handlers_on_key_press(GLFW_KEY_RIGHT);
	call_handlers_on_key_press(GLFW_KEY_W);
	call_handlers_on_key_press(GLFW_KEY_S);
	call_handlers_on_key_press(GLFW_KEY_A);
	call_handlers_on_key_press(GLFW_KEY_D);
	call_handlers_on_key_press(GLFW_KEY_SPACE);
	call_handlers_on_key_press(GLFW_KEY_LEFT_SHIFT);
}


void ruya::Window::call_handlers_on_key_press(int glfwKeyEventID)
{
	if (glfwGetKey(mGLFWwindow, glfwKeyEventID) == GLFW_PRESS)
	{
		for (auto callback : mEventCallbackMap[glfwKeyEventID])
			callback();
	}
}
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <glm/glm.hpp>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <unordered_map>
#include <list>

using std::list;
using std::unordered_map;

/*
* The MainWindow object encapsulates GLFW code to create and manage a window.
*/
namespace ruya
{
	class Window
	{
	public:
		enum class CursorMode { NORMAL, HIDDEN, DISABLED }; // corresponds with the glfwSetInputMode() values 

		// CONSTRUCTORS & DESTRUCTOR
		Window(int width = 1480, int height = 720);
		~Window();

		// GETTERS & SETTERS
		GLFWwindow* get_GLFW_window();
		int height() { return _height; }
		int width() { return _width; }
		float aspect_ratio() { return static_cast<float>(_width) / static_cast<float>(_height); }

		// QUERRIES
		bool should_close() { return glfwWindowShouldClose(_glfw_window); }

		// MANIPULATORS
		void make_context_current() { glfwMakeContextCurrent(_glfw_window); }
		void update();
        void set_cursor_mode(CursorMode mode);

	private:

		// VARIABLES
		int _width = 0;
		int _height = 0;
		GLFWwindow* _glfw_window = nullptr;
        CursorMode _cursor_mode = CursorMode::NORMAL;

		// PRIVATE FUNCTIONS
		static void resize_callback_static(GLFWwindow* window, int width, int height);
		void resize_callback(GLFWwindow* window, int width, int height);
	};


} // end namespace 


#endif // !MAINWINDOW_H
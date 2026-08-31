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
	namespace io
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

		enum class Key : int {
			A = GLFW_KEY_A,
			B = GLFW_KEY_B,
			C = GLFW_KEY_C,
			D = GLFW_KEY_D,
			E = GLFW_KEY_E,
			F = GLFW_KEY_F,
			G = GLFW_KEY_G,
			H = GLFW_KEY_H,
			I = GLFW_KEY_I,
			J = GLFW_KEY_J,
			K = GLFW_KEY_K,
			L = GLFW_KEY_L,
			M = GLFW_KEY_M,
			N = GLFW_KEY_N,
			O = GLFW_KEY_O,
			P = GLFW_KEY_P,
			Q = GLFW_KEY_Q,
			R = GLFW_KEY_R,
			S = GLFW_KEY_S,
			T = GLFW_KEY_T,
			U = GLFW_KEY_U,
			V = GLFW_KEY_V,
			W = GLFW_KEY_W,
			X = GLFW_KEY_X,
			Y = GLFW_KEY_Y,
			Z = GLFW_KEY_Z,
			UP = GLFW_KEY_UP,
			DOWN = GLFW_KEY_DOWN,
			LEFT = GLFW_KEY_LEFT,
			RIGHT = GLFW_KEY_RIGHT
		};

		
		inline bool key_pressed(Window& window, Key key)
		{
			return glfwGetKey(window.get_GLFW_window(), static_cast<int>(key)) == GLFW_PRESS;
		}
	}
} // end namespace 


#endif // !MAINWINDOW_H
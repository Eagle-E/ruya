#ifndef MAINWINDOW_H
#define MAINWINDOW_H

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
	typedef void (*VOID_FPTR)() ;

	class Window
	{
	public:
		// CONSTRUCTORS & DESTRUCTOR
		Window(int width = 1480, int height = 720);
		~Window();

		// GETTERS & SETTERS
		GLFWwindow* get_GLFW_window();
		int height() { return mHeight; }
		int width() { return mWidth; }
		float aspect_ratio() { return static_cast<float>(mWidth) / static_cast<float>(mHeight); }

		// QUERRIES
		bool should_close() { return glfwWindowShouldClose(mGLFWwindow); }

		// MANIPULATORS
		void make_context_current() { glfwMakeContextCurrent(mGLFWwindow); }
		void update();
		void add_event_callback(int glfwEventID, VOID_FPTR callback);
		void remove_event_callback(VOID_FPTR callback);

	private:
		enum class RenderMode { FILL, WIREFRAME };
		
		// VARIABLES
		int mWidth = 0;
		int mHeight = 0;
		GLFWwindow* mGLFWwindow = nullptr;
		RenderMode renderMode = RenderMode::FILL;
		bool allowRenderModeChange = true;

		// TODO: this is a temporary event handling method, implement a proper event handling system
		void call_handlers_on_key_press(int glfwKeyEventID);
		unordered_map<int, list<VOID_FPTR>> mEventCallbackMap;

		// PRIVATE FUNCTIONS
		static void resize_callback_static(GLFWwindow* window, int width, int height);
		void resize_callback(GLFWwindow* window, int width, int height);
		void process_inputs();
	};


} // end namespace 


#endif // !MAINWINDOW_H
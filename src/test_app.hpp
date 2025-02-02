#ifndef TEST_APP_H
#define TEST_APP_H

#include <iostream>
#include <fstream>
#include <string>
#include <exception>
#include <memory>
#include <filesystem>

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>
#include <whereami/whereami++.h>

// #include <imgui/imgui.h>
// #include <imgui/imgui_impl_glfw.h>
// #include <imgui/imgui_impl_opengl3.h>

#include "io/stb_image.h"
#include "app.h"
#include "engine/core/window.h"
#include "engine/render/shader.h"
#include "engine/scene/object.h"
#include "engine/scene/models/square.h"
#include "engine/scene/models/cube.h"
#include "engine/scene/models/icosahedron.h"
#include "engine/scene/models/icosphere.hpp"
#include "engine/scene/mesh.h"
#include "engine/render/renderer.h"
#include "engine/scene/texture.h"
#include "engine/scene/camera.h"
#include "engine/scene/scene.h"
#include "engine/scene/light_source.h"
#include "utils/timer.h"


namespace fs = std::filesystem;
using std::vector;

using glm::vec3;		using glm::vec2;		using glm::dvec2;
using ruya::models::Square;		using ruya::Shader;
using ruya::Mesh;		using ruya::Renderer;
using ruya::Texture;	using ruya::Camera;
using ruya::models::Cube;		using ruya::Timer;
using ruya::models::Icosahedron; using ruya::Scene;
using ruya::LightSource; using ruya::models::Icosphere;


namespace ruya
{
	class TestApp : public App
	{
	private: // VARIABLES
		Camera mCamera;
		Timer mFrameTimer;
		Window& mWindow;
		dvec2 mOldMousePos;
		bool mAllowShadingModeChange;
		Renderer* mRenderer;

	public: // FUNCTIONS
		/*** CONSTRUCT ***/
		TestApp(Window& window) : mWindow(window), mOldMousePos(-1.0, -1.0), mAllowShadingModeChange(true)
		{
			glfwSetInputMode(window.get_GLFW_window(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		}

		/*** DESTRUCT ***/
		~TestApp()
		{
			glfwTerminate(); // clean up all reasources allocated by glfw.
		}



		/*** MAINLOOP ***/
		void run()
		{
			// init renderer and shaders
			std::cout << "Run start" << std::endl;
			fs::path baseDir {whereami::getExecutablePath().dirname()};
			fs::path phongDir {baseDir / "shaders" / "phong"};
			fs::path flatDir {baseDir / "shaders" / "flat"};

			fs::path phongVertShader {phongDir / "object.vert"};
			fs::path phongFragShader {phongDir / "object.frag"};
			fs::path phongFragShaderLights {phongDir / "light_source.frag"};

			fs::path flatVertShader {flatDir / "flat_vert.vert"};
			fs::path flatFragShader {flatDir / "flat_frag.frag"};
			fs::path flatGeomShader {flatDir / "flat_geom.geom"};

			Shader shaderPhongObjects(phongVertShader.string().c_str(), phongFragShader.string().c_str());
			Shader shaderPhongLights(phongVertShader.string().c_str(), phongFragShaderLights.string().c_str());
			Shader shaderFlat(flatVertShader.string().c_str(), flatGeomShader.string().c_str(), flatFragShader.string().c_str());
			std::cout << "Init shaders" << std::endl;

			Renderer renderer(&shaderPhongObjects, &shaderPhongLights, &mWindow, &mCamera);
			renderer.set_flat_shader(&shaderFlat);
			mRenderer = &renderer;
			std::cout << "Init renderer" << std::endl;

			// init scene
			Scene scene;
			std::cout << "Init Scene" << std::endl;

			// the object to render
			// vector< shared_ptr<Texture>> textures;
			// textures.push_back(std::make_shared<Texture>("resources/Wood049_1K-PNG/Wood049_1K_Color.png"));
			// textures.push_back(std::make_shared<Texture>("resources/Leather026_1K-PNG/Leather026_1K_Color.png"));
			// textures.push_back(std::make_shared<Texture>("resources/Marble023_1K-PNG/Marble023_1K_Color.png"));
			// textures.push_back(std::make_shared<Texture>("resources/Metal032_1K-PNG/Metal032_1K_Color.png"));
			// textures.push_back(std::make_shared<Texture>("resources/Fabric004_1K-PNG/Fabric004_1K_Color.png"));
			// std::cout << "Init textures" << std::endl;

			vector<Object*> objects;
			int radius = 2; // radius of grid, so grid will have 2r+1 cols and rows
			float d = 7.5f;
			for (float i = -radius; i <= radius; i++)
			{
				for (float j = -radius; j <= radius; j++)
				{
					if (false)
					{
						for (float k = -radius; k <= radius; k++)
						{
							//Object* newObjptr = nullptr;
							//if ( (int)(i+j+k) % 2 == 0)
							//	newObjptr = new Cube();
							//else 
							//	newObjptr = new Icosahedron();

							Object* newObjptr = new Icosphere(i + radius);
							float g = (i + radius) / (2 * radius) * 0.8 + 0.1; // map i and j from [-r, r] to [0.1, 0.8]
							float b = (j + radius) / (2 * radius) * 0.8 + 0.1;
							float r = (k + radius) / (2 * radius) * 0.8 + 0.1;
							//float r = (b + g) / 2.0f;
							newObjptr->set_color(vec3(r, g, b));
							newObjptr->set_position(vec3(d * i, d * j, d * k));
							newObjptr->set_scale(3.0f);
							newObjptr->material().ambient = vec3(1.0f);
							newObjptr->material().diffuse = vec3(1.0f);
							newObjptr->material().specular = vec3(1.0f);
							newObjptr->material().shininess = 1.0f;
							//newCube.set_texture(textures[i % textures.size()]);
							objects.push_back(newObjptr);
							scene.add_object(newObjptr);
						}
					}
					else
					{
						Object* newObjptr = new Icosphere(i + radius);
						float g = (i + radius) / (2 * radius) * 0.8 + 0.1; // map i and j from [-r, r] to [0.1, 0.8]
						float b = (j + radius) / (2 * radius) * 0.8 + 0.1;
						//float r = (k + radius) / (2 * radius) * 0.8 + 0.1;
						float r = (b + g) / 2.0f;
						newObjptr->set_color(vec3(r, g, b));
						newObjptr->set_position(vec3(d * i, d * j, -5.0f));
						newObjptr->set_scale(3.0f);
						objects.push_back(newObjptr);
						scene.add_object(newObjptr);
					}
				}
			}

			// add spheres in a line
			Timer timer;
			printf("Old algo:\n");
			for (int i = 0; i <= 5; i++)
			{
				timer.start();
				Icosphere* sphere = new Icosphere(i);
				timer.stop();
				sphere->set_position((i-3.0f) * 2.5f, 2.5f, -1.0f);
				scene.add_object(sphere);

				printf("sphere level %d = %d vertices, %d faces (%f s)\n", i, sphere->mesh()->vertices.size(), sphere->mesh()->faces.size(), timer.elapsed_time_s());
			}
	
			/*
			for (int i = 0; i <= 5; i++)
			{
				for (int j = 0; j < 5; j++)
				{
					Icosphere* sphere = new Icosphere(i);
					sphere->set_position((i - 3.0f) * 2.5f, 2.5f + j * 2.5f, -1.0f);
					scene.add_object(sphere);
				}
			}
			*/
			

			Cube ico;
			Cube* newCubeptr = new Cube();
			Cube* floor = new Cube();
			Icosahedron* newIco= new Icosahedron();

			newCubeptr->set_position(3.0f, -1.0f, -2.0f);
			newIco->set_position(-3.0f, -1.0f, -2.0f);

			floor->set_scale(100.0f);
			floor->set_position(0, -floor->scale().y/2 - 10.0f, 0);
			floor->set_color(0.65f, 0.85f, 0.95f);
			floor->set_material(Materials::chrome);
			//newCubeptr->set_scale(vec3(3.0f, 3.0f, 3.0f));

			//ico.set_color(vec3(1.0f, 0.5f, 0.31f));
			LightSource * light = new LightSource(vec3(1.0f, 1.0f, 1.0f));
			light->model().set_mesh(ico.mesh()); // the icosahedron obj and the lightsource will share the same mesh
			light->model().set_position(0.0f, 5.0f, 3.0f);
			light->model().set_color(vec3(1.0f, 1.0f, 1.0f));
			//light->model().set_color(vec3(1.0f, 1.0f, 1.0f));
			light->set_ambient(vec3(0.2f, 0.2f, 0.2f));
			light->set_diffuse(vec3(0.7f, 0.7f, 0.7f));
			light->set_specular(vec3( 1.0f, 1.0f, 1.0f));
			
			scene.add_object(newCubeptr);
			scene.add_object(newIco);
			scene.add_light(light);
			//scene.add_object(floor);


			glm::vec4 bgColor(0.9f, 0.9f, 0.9f, 1.0f); // background color
			ruya::Timer timerOutput;
			timerOutput.start();

			while (!mWindow.should_close())
			{
				mFrameTimer.start();
				// change window color
				glClearColor(bgColor.r, bgColor.g, bgColor.b, bgColor.a);
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

				// some transforming
				float xs = 0.45f; // rotation speeds
				float ys = 0.90f;
				float zs = 0.15f;

				float degrees = glm::degrees((float)glfwGetTime());
				for (Object* obj : objects)
					obj->set_rotation(vec3(xs * degrees, ys * degrees, zs * degrees));
				
				newCubeptr->set_rotation(vec3(xs * degrees, ys * degrees, zs * degrees));
				newIco->set_rotation(vec3(xs * degrees, ys * degrees, zs * degrees));
				mat4 rot(1.0); 
				rot = glm::rotate(rot, glm::radians(zs * degrees), vec3(1.0f, 0.0f, 0.0f));
				rot = glm::rotate(rot, glm::radians(xs * degrees), vec3(0.0f, 1.0f, 0.0f));
				rot = glm::rotate(rot, glm::radians(ys * degrees), vec3(0.0f, 0.0f, 1.0f));
				vec4 pos = rot * vec4(50, 0, 0, 1);
				//light->model().set_position(vec3(pos.x, pos.y, pos.z) / pos.w);
				light->model().set_position(cos(glfwGetTime()) * 7.5f, 5.0f, 3.0f);


				// RENDER!!!
				renderer.render_scene(scene);

				// update frame => swaps buffers = starts showing newly rendered buffer
				// + checks for input events and calls handlers
				mWindow.update();

				// calc FPS
				mFrameTimer.stop();
				double frameTime = mFrameTimer.elapsed_time_s();
				double fps = 1 / frameTime;
				//double fps = frameTime;

				if (timerOutput.elapsed_time_s() > 1.0)
				{
					std::cout << fps << " fps"
						<< "\tElapsed time: " << timerOutput.time_since_creation_s() << "s" 
						<< "\tmouse pos: ("<< mOldMousePos.x <<","<< mOldMousePos.y <<")\n";
					timerOutput.start();
				}

				poll_events();
			}
		}
		
		void poll_events()
		{
			GLFWwindow* glfwWindow = mWindow.get_GLFW_window();
			
			// move camera forward/backward/left/right perpendicular with the xz plane
			// move camera up/down along y-axis
			float moveSpeed = 6.0f; // units per second
			float dt = mFrameTimer.elapsed_time_s();

			if (glfwGetKey(glfwWindow, GLFW_KEY_W) == GLFW_PRESS)
			{
				vec3 direction = mCamera.cam_front();
				vec2 moveDirection = glm::normalize(glm::vec2(direction.x, direction.z));
				vec3 pos = mCamera.position();
				pos.x += moveDirection.x * moveSpeed * dt;
				pos.z += moveDirection.y * moveSpeed * dt;
				mCamera.set_position(pos);
			}

			if (glfwGetKey(glfwWindow, GLFW_KEY_S) == GLFW_PRESS)
			{
				vec3 direction = mCamera.cam_front();
				vec2 moveDirection = glm::normalize(glm::vec2(direction.x, direction.z));
				vec3 pos = mCamera.position();
				pos.x -= moveDirection.x * moveSpeed * dt;
				pos.z -= moveDirection.y * moveSpeed * dt;
				mCamera.set_position(pos);
			}

			if (glfwGetKey(glfwWindow, GLFW_KEY_A) == GLFW_PRESS)
			{
				vec3 direction = mCamera.cam_front();
				vec2 moveDirection = glm::normalize(glm::vec2(direction.x, direction.z));
				moveDirection = vec2(moveDirection.y, -moveDirection.x); // turn clockwise 90deg from forward direction
				vec3 pos = mCamera.position();
				pos.x += moveDirection.x * moveSpeed * dt;
				pos.z += moveDirection.y * moveSpeed * dt;
				mCamera.set_position(pos);
			}

			if (glfwGetKey(glfwWindow, GLFW_KEY_D) == GLFW_PRESS)
			{
				vec3 direction = mCamera.cam_front();
				vec2 moveDirection = glm::normalize(glm::vec2(direction.x, direction.z));
				moveDirection = vec2(-moveDirection.y, moveDirection.x); // turn clockwise 90deg from forward direction
				vec3 pos = mCamera.position();
				pos.x += moveDirection.x * moveSpeed * dt;
				pos.z += moveDirection.y * moveSpeed * dt;
				mCamera.set_position(pos);
			}

			if (glfwGetKey(glfwWindow, GLFW_KEY_SPACE) == GLFW_PRESS)
			{
				vec3 pos = mCamera.position();
				pos.y += moveSpeed * mFrameTimer.elapsed_time_s();
				mCamera.set_position(pos);
			}

			if (glfwGetKey(glfwWindow, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
			{
				vec3 pos = mCamera.position();
				pos.y -= moveSpeed * mFrameTimer.elapsed_time_s();
				mCamera.set_position(pos);
			}

			if (glfwGetKey(glfwWindow, GLFW_KEY_2) == GLFW_PRESS && mAllowShadingModeChange)
			{
				if (mRenderer != nullptr)
				{
					if (mRenderer->shading_mode() == Renderer::ShadingMode::FLAT)
						mRenderer->set_shading_mode(Renderer::ShadingMode::SMOOTH);
					else 
						mRenderer->set_shading_mode(Renderer::ShadingMode::FLAT);
					mAllowShadingModeChange = false;
				}
			}
			if (glfwGetKey(glfwWindow, GLFW_KEY_2) == GLFW_RELEASE)
			{
				mAllowShadingModeChange = true;
			}


			// MOUSE MOVEMENT
			update_camera_look_direction();
		}

		void update_camera_look_direction()
		{
			// get new pos
			dvec2 pos;
			glfwGetCursorPos(mWindow.get_GLFW_window(), &(pos.x), &(pos.y));

			// init mouse pos if this is the first time
			if (-1 <= mOldMousePos.x && mOldMousePos.x <= -0.95
				&& -1 <= mOldMousePos.y && mOldMousePos.y <= -0.95)
			{
				mOldMousePos = pos;
			}

			// movement difference
			dvec2 deltaPos = pos - mOldMousePos;
			//deltaPos.y *= -1; // mouse y is negative upwards, flip y-axis

			// turn camera
			double turnSpeed = 0.0005;
			mCamera.update_angle(deltaPos.x * turnSpeed, deltaPos.y * turnSpeed);

			mOldMousePos = pos;
		}
	};
}

#endif // TEST_APP_H
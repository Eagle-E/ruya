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

#include <imgui/imgui.h>
#include <imgui/backends/imgui_impl_glfw.h>
#include <imgui/backends/imgui_impl_opengl3.h>

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

using glm::vec3;					using glm::vec2;		using glm::dvec2;
using ruya::models::Square;			using ruya::Shader;
using ruya::Mesh;					using ruya::Renderer;
using ruya::Texture;				using ruya::Camera;
using ruya::models::Cube;			using ruya::Timer;
using ruya::models::Icosahedron; 	using ruya::Scene;
using ruya::LightSource; 			using ruya::models::Icosphere;


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
            // glfwTerminate(); // clean up all reasources allocated by glfw.
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
            Texture::print_max_texture_slots_info();

			fs::path resDir {baseDir / "resources"};
			fs::path texPathWoodColor {resDir / "asphalt_1k" / "asphalt010_1k_color.png"};

			vector< shared_ptr<Texture>> textures;
			textures.push_back(std::make_shared<Texture>(texPathWoodColor.c_str()));
			// textures.push_back(std::make_shared<Texture>("resources/Leather026_1K-PNG/Leather026_1K_Color.png"));
			// textures.push_back(std::make_shared<Texture>("resources/Marble023_1K-PNG/Marble023_1K_Color.png"));
			// textures.push_back(std::make_shared<Texture>("resources/Metal032_1K-PNG/Metal032_1K_Color.png"));
			// textures.push_back(std::make_shared<Texture>("resources/Fabric004_1K-PNG/Fabric004_1K_Color.png"));
			std::cout << "Init textures" << std::endl;

            // add spheres in a line
            for (size_t i = 0; i <= 5; i++)
            {
                Cube* cube = new Cube();
                cube->set_texture(textures[0]);

                cube->set_position((i-3.0f) * 2.5f, 2.5f, -1.0f);
                scene.add_object(cube);
            }

            Cube ico;
            Cube* newCubeptr = new Cube();
            Cube* floor = new Cube();

            newCubeptr->set_position(3.0f, -1.0f, -2.0f);

            floor->set_scale(100.0f);
            floor->set_position(0, -floor->scale().y/2 - 10.0f, 0);
            floor->set_color(0.9f, 0.9f, 0.9f);
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
            scene.add_light(light);
            scene.add_object(floor);


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
                
                newCubeptr->set_rotation(vec3(xs * degrees, ys * degrees, zs * degrees));
                light->model().set_position(cos(glfwGetTime()) * 7.5f, 5.0f, 3.0f);

                // prepare ui
                ImGui_ImplOpenGL3_NewFrame();
                ImGui_ImplGlfw_NewFrame();
                ImGui::NewFrame();
                {
                    ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.
                    ImGui::Button("Button");
                    ImGui::End();
                }
                {
                    ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.
                    ImGui::Text("Hello from another window!");
                    ImGui::Button("Close Me");
                    ImGui::End();
                }
                
				// RENDER!!!
				ImGui::Render();
                renderer.render_scene(scene);
                ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

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
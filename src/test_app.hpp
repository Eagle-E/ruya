#ifndef TEST_APP_H
#define TEST_APP_H

#include <iostream>
#include <fstream>
#include <string>
#include <exception>
#include <memory>
#include <filesystem>
#include <format>

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
#include "core/timer.h"
#include "core/window.h"
#include "render/shader.h"
#include "scene/object.h"
#include "scene/mesh.h"
#include "render/renderer.h"
#include "render/texture.hpp"
#include "scene/camera.h"
#include "scene/scene.h"
#include "scene/light.hpp"
#include "scene/vault.hpp"
#include "scene/gen.hpp"
#include "scene/components.hpp"

#include "ui/object_widget.h"
#include "ui/scene_widget.h"

namespace fs = std::filesystem;
namespace gen = ruya::scene::gen;
using std::vector;

using glm::dvec2;
using glm::vec2;
using glm::vec3;	

using ruya::Camera;			                        
using ruya::Timer;
using ruya::render::Renderer;
using ruya::render::Shader;
using ruya::render::Texture;		
using ruya::scene::Element;
using ruya::scene::LightBasic;
using ruya::scene::Mesh;			
using ruya::scene::MeshID;
using ruya::scene::Model;
using ruya::scene::Scene;
using ruya::scene::TextureID;
using ruya::scene::Vault;
using ruya::scene::materials::Phong;
using ruya::scene::materials::PhongMaterials;

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
        int count = 0;

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
            fs::path base_dir {whereami::getExecutablePath().dirname()};
            fs::path phong_dir {base_dir / "shaders" / "phong"};
            fs::path flat_dir {base_dir / "shaders" / "flat"};
            
            fs::path phongVertShader {phong_dir / "object.vert"};
            fs::path phongFragShader {phong_dir / "object.frag"};
            fs::path phongFragShaderLights {phong_dir / "light_source.frag"};
            
            fs::path flatVertShader {flat_dir / "flat_vert.vert"};
            fs::path flatFragShader {flat_dir / "flat_frag.frag"};
            fs::path flatGeomShader {flat_dir / "flat_geom.geom"};
            
            Shader shaderPhongObjects(phongVertShader, phongFragShader);
            Shader shaderPhongLights(phongVertShader, phongFragShaderLights);
            Shader shaderFlat(flatVertShader, flatGeomShader, flatFragShader);
            std::cout << "Init shaders" << std::endl;

            Renderer renderer(&shaderPhongObjects, &shaderPhongLights, &mWindow, &mCamera);
            renderer.set_flat_shader(&shaderFlat);
            mRenderer = &renderer;
            std::cout << "Init renderer" << std::endl;

            // init scene
            Scene scene;
            scene.background_color = vec4(.1f, .1f, .1f, 1.0f);
            std::cout << "Init Scene" << std::endl;

			// the object to render
            // ruya::render::print_max_texture_units_info();

			fs::path resDir {base_dir / "resources"};
			fs::path texPathBarrelColor {resDir / "barrel" / "barrel_color.png"};
			fs::path texPathBarrelSpecular {resDir / "barrel" / "barrel_specular.png"};
			fs::path texPathEmojiColor {resDir / "emoji" / "emoji_color.png"};

            Vault vault;
            ImageID barrel_img_id_color = vault.load_image(texPathBarrelColor);
            ImageID barrel_img_id_specular = vault.load_image(texPathBarrelSpecular);
            ImageID emoji_img_id = vault.load_image(texPathEmojiColor);
            vault.add_mesh(gen::cube(), "gen::cube");
            vault.add_mesh(gen::square(), "gen::square");
            vault.add_mesh(gen::icosahedron(), "gen::icosahedron");
            vault.add_mesh(gen::icosphere(), "gen::icosphere");
			std::cout << "Init textures" << std::endl;

            // add spheres in a line
            Phong custom_material = Phong{
                vec3(1.0f,1.0f,1.0f),
                vec3(1.0f,1.0f,1.0f),
                1.0f
            };

            int n_cubes = 5;
            for (size_t i = 0; i < n_cubes; i++)
            {
                Object* cube = new Object();
                cube->model.elements.push_back(
                    Element{
                        .mesh = vault.mesh_cache["gen::cube"],
                        .material = Phong{
                            .diffuse_map = barrel_img_id_color,
                            .specular_map = barrel_img_id_specular
                        },
                        .transform = Transform{
                            .position = vec3{(i-3.0f) * 2.5f, 2.5f, -1.0f},
                            .scale = vec3(1.0f)
                        }
                    }
                );
                cube->model.elements.push_back(
                    Element{
                        .mesh = vault.mesh_cache["gen::cube"],
                        .material = Phong{
                            .diffuse_map = emoji_img_id,
                            .specular_map = barrel_img_id_specular
                        },
                        .transform = Transform{
                            .position = vec3{(i-3.0f) * 2.5f, 2.5f, 1.0f},
                            .scale = vec3(.5f)
                        }
                    }
                );
                scene.add_object(cube);
            }
            
            
            LightBasic * light = new LightBasic();
            light->model.elements.push_back(
                Element{
                    .mesh = vault.mesh_cache["gen::cube"],
                }
            );
            light->ambient = vec3(0.2f, 0.2f, 0.2f);
            light->diffuse = vec3(0.7f, 0.7f, 0.7f);
            light->specular = vec3(1.0f, 1.0f, 1.0f);
            scene.add_light(light);
            
            ruya::Timer timerOutput;
            timerOutput.start();
            
            while (!mWindow.should_close())
            {
                mFrameTimer.start();

                // some transforming
                float xs = 0.45f; // rotation speeds
                float ys = 0.90f;
                float zs = 0.15f;

                float degrees = glm::degrees((float)glfwGetTime());
                
                light->position = {cos(glfwGetTime()) * 7.5f, 5.0f, 3.0f};

                // prepare ui
                ImGui_ImplOpenGL3_NewFrame();
                ImGui_ImplGlfw_NewFrame();
                ImGui::NewFrame();
                {
                    // In your ImGui window:
                    ImGui::Begin("Settings");
                    {
                        ruya::ui::scene_widget(scene);
                    }
                    ImGui::End();
                }
                

				// RENDER!!!
                mWindow.clear_frame_buffer(scene.background_color);
                ImGui::Render();
                renderer.render_scene(scene, vault);
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
            ImGuiIO& imgui_io = ImGui::GetIO();
            
            GLFWwindow* glfwWindow = mWindow.get_GLFW_window();
            
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


            // CAMERA CONTROL
            
            // update camera interaction when user is not interacting with imgui
            if (!imgui_io.WantCaptureMouse) 
            {
                update_camera_position();
                update_camera_look_direction();
            }
        }

        void update_camera_position()
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
        }

        void update_camera_look_direction()
        {
            // get new pos
            dvec2 pos;
            glfwGetCursorPos(mWindow.get_GLFW_window(), &(pos.x), &(pos.y));

            // look around when mouse is pressed
            GLFWwindow* glfwWindow = mWindow.get_GLFW_window();
            if (glfwGetMouseButton(glfwWindow, GLFW_MOUSE_BUTTON_1) == GLFW_RELEASE)
            {
                mWindow.set_cursor_mode(Window::CursorMode::NORMAL);
                mOldMousePos = pos; // so that the view doesn't jump when pressing the button to rotate camera
                return;
            }
            if (glfwGetMouseButton(glfwWindow, GLFW_MOUSE_BUTTON_1) == GLFW_PRESS)
            {
                mWindow.set_cursor_mode(Window::CursorMode::DISABLED);
            }


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

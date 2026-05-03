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
#include <entt/entt.hpp>

#include <imgui/imgui.h>
#include <imgui/backends/imgui_impl_glfw.h>
#include <imgui/backends/imgui_impl_opengl3.h>

#include "io/stb_image.h"
#include "io/paths.hpp"
#include "app.h"
#include "core/timer.h"
#include "core/window.h"
#include "render/shader.h"
#include "scene/mesh.h"
#include "render/renderer.h"
#include "render/texture.hpp"
#include "scene/camera.h"
#include "scene/scene.hpp"
#include "scene/light.hpp"
#include "scene/vault.hpp"
#include "scene/gen.hpp"
#include "scene/components.hpp"

#include "ui/model_widget.h"
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
using ruya::scene::BasicLight;
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
        Camera _camera;
        Timer _frame_timer;
        Timer _frame_output_timer;
        Window& _window;
        dvec2 _old_mouse_pos {-1.0, -1.0};
        bool _allow_shading_mode_change = true;
        Renderer* _renderer;

    public: // FUNCTIONS
        /*** CONSTRUCT ***/
        TestApp(Window& window) : _window(window)
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
            
            fs::path phong_dir {ruya::io::DIR_SHADERS / "phong"};
            Shader shader_phong{
                {phong_dir / "object.vert"},
                {phong_dir / "object.frag"}
            };
            Shader shader_phong_lights{
                {phong_dir / "object.vert"},
                {phong_dir / "light_source.frag"}
            };
            
            fs::path flat_dir {ruya::io::DIR_SHADERS / "flat"};
            Shader shader_flat{
                {flat_dir / "flat_vert.vert"},
                {flat_dir / "flat_geom.geom"},
                {flat_dir / "flat_frag.frag"}
            };
            std::cout << "Init shaders" << std::endl;

            Renderer renderer(&shader_phong, &shader_phong_lights, &_window, &_camera);
            renderer.set_flat_shader(&shader_flat);
            _renderer = &renderer;
            std::cout << "Init renderer" << std::endl;

            // init scene
            Scene scene;
            scene.background_color = vec4(.1f, .1f, .1f, 1.0f);
            std::cout << "Init Scene" << std::endl;

			// the object to render
            // ruya::render::print_max_texture_units_info();
			fs::path texPathBarrelColor {ruya::io::DIR_RESOURCES / "barrel" / "barrel_color.png"};
			fs::path texPathBarrelSpecular {ruya::io::DIR_RESOURCES / "barrel" / "barrel_specular.png"};
			fs::path texPathEmojiColor {ruya::io::DIR_RESOURCES / "emoji" / "emoji_color.png"};

            Vault vault;
            ImageID barrel_img_id_color = vault.load_image(texPathBarrelColor);
            ImageID barrel_img_id_specular = vault.load_image(texPathBarrelSpecular);
            ImageID emoji_img_id = vault.load_image(texPathEmojiColor);
            vault.add_mesh(gen::cube(), "gen::cube");
            vault.add_mesh(gen::square(), "gen::square");
            vault.add_mesh(gen::icosahedron(), "gen::icosahedron");
            vault.add_mesh(gen::icosphere(), "gen::icosphere");
			std::cout << "Init textures" << std::endl;

            // add cubes in a line
            int n_cubes = 5;
            for (size_t i = 0; i < n_cubes; i++)
            {
                auto entity = scene.registry.create();
                Model model;
                model.elements.push_back(
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
                model.elements.push_back(
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
                scene.registry.emplace<Model>(entity, model);
            }
            
            // LIGHT 1
            BasicLight light {
                .position = vec3{(2-3.0f) * 2.5f, 1.0f, 3.0f},
                .ambient = vec3(0.2f, 0.2f, 0.2f),
                .diffuse = vec3(0.7f, 0.7f, 0.7f),
                .specular = vec3(1.0f, 1.0f, 1.0f),
            };
            Model light_model;
            light_model.elements.push_back(
                Element{
                    .mesh = vault.mesh_cache["gen::cube"],
                    .material = Phong{},
                    .transform = Transform{
                        .position = vec3{(2-3.0f) * 2.5f, 1.0f, 3.0f},
                    }
                }
            );
            auto light_entity = scene.registry.create();
            scene.registry.emplace<BasicLight>(light_entity, light);
            scene.registry.emplace<Model>(light_entity, light_model);

            // LIGHT 2
            BasicLight light2 {
                .position = vec3{(2-3.0f) * 2.5f, 1.0f, 3.0f},
                .ambient = vec3(0.2f, 0.2f, 0.2f),
                .diffuse = vec3(0.7f, 0.7f, 0.7f),
                .specular = vec3(1.0f, 1.0f, 1.0f),
            };
            Model light_model2;
            light_model2.elements.push_back(
                Element{
                    .mesh = vault.mesh_cache["gen::cube"],
                    .material = Phong{},
                    .transform = Transform{
                        .position = vec3{2.0f, 1.0f, 3.0f},
                    }
                }
            );
            auto light_entity2 = scene.registry.create();
            scene.registry.emplace<BasicLight>(light_entity2, light2);
            scene.registry.emplace<Model>(light_entity2, light_model2);

            // MAIN LOOP
            _frame_output_timer.start();
            while (!_window.should_close())
            {
                _frame_timer.start();

                // move the light around
                // scene.registry.get<BasicLight>(light_entity).position = {cos(glfwGetTime()) * 7.5f, 5.0f, 3.0f};

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
                _window.clear_frame_buffer(scene.background_color);
                renderer.render_scene(scene, vault);
                ImGui::Render();
                ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

                // update frame => swaps buffers = starts showing newly rendered buffer
                // + checks for input events and calls handlers
                _window.update();

                // calc FPS
                log_fps();
                poll_and_process_events();
            }
        }

        void log_fps()
        {
            _frame_timer.stop();
            double frameTime = _frame_timer.elapsed_time_s();
            double fps = 1 / frameTime;
            //double fps = frameTime;

            if (_frame_output_timer.elapsed_time_s() > 1.0)
            {
                std::cout << fps << " fps"
                    << "\tElapsed time: " << _frame_output_timer.time_since_creation_s() << "s" 
                    << "\tmouse pos: ("<< _old_mouse_pos.x <<","<< _old_mouse_pos.y <<")\n";
                _frame_output_timer.start();
            }
        }

        void poll_and_process_events()
        {
            ImGuiIO& imgui_io = ImGui::GetIO();
            
            GLFWwindow* glfwWindow = _window.get_GLFW_window();
            
            if (glfwGetKey(glfwWindow, GLFW_KEY_2) == GLFW_PRESS && _allow_shading_mode_change)
            {
                if (_renderer != nullptr)
                {
                    if (_renderer->shading_mode() == Renderer::ShadingMode::FLAT)
                        _renderer->set_shading_mode(Renderer::ShadingMode::SMOOTH);
                    else 
                        _renderer->set_shading_mode(Renderer::ShadingMode::FLAT);
                    _allow_shading_mode_change = false;
                }
            }
            if (glfwGetKey(glfwWindow, GLFW_KEY_2) == GLFW_RELEASE)
            {
                _allow_shading_mode_change = true;
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
            GLFWwindow* glfwWindow = _window.get_GLFW_window();
            
            // move camera forward/backward/left/right perpendicular with the xz plane
            // move camera up/down along y-axis
            float moveSpeed = 6.0f; // units per second
            float dt = _frame_timer.elapsed_time_s();

            if (glfwGetKey(glfwWindow, GLFW_KEY_W) == GLFW_PRESS)
            {
                vec3 direction = _camera.cam_front();
                vec2 moveDirection = glm::normalize(glm::vec2(direction.x, direction.z));
                vec3 pos = _camera.position();
                pos.x += moveDirection.x * moveSpeed * dt;
                pos.z += moveDirection.y * moveSpeed * dt;
                _camera.set_position(pos);
            }

            if (glfwGetKey(glfwWindow, GLFW_KEY_S) == GLFW_PRESS)
            {
                vec3 direction = _camera.cam_front();
                vec2 moveDirection = glm::normalize(glm::vec2(direction.x, direction.z));
                vec3 pos = _camera.position();
                pos.x -= moveDirection.x * moveSpeed * dt;
                pos.z -= moveDirection.y * moveSpeed * dt;
                _camera.set_position(pos);
            }

            if (glfwGetKey(glfwWindow, GLFW_KEY_A) == GLFW_PRESS)
            {
                vec3 direction = _camera.cam_front();
                vec2 moveDirection = glm::normalize(glm::vec2(direction.x, direction.z));
                moveDirection = vec2(moveDirection.y, -moveDirection.x); // turn clockwise 90deg from forward direction
                vec3 pos = _camera.position();
                pos.x += moveDirection.x * moveSpeed * dt;
                pos.z += moveDirection.y * moveSpeed * dt;
                _camera.set_position(pos);
            }

            if (glfwGetKey(glfwWindow, GLFW_KEY_D) == GLFW_PRESS)
            {
                vec3 direction = _camera.cam_front();
                vec2 moveDirection = glm::normalize(glm::vec2(direction.x, direction.z));
                moveDirection = vec2(-moveDirection.y, moveDirection.x); // turn clockwise 90deg from forward direction
                vec3 pos = _camera.position();
                pos.x += moveDirection.x * moveSpeed * dt;
                pos.z += moveDirection.y * moveSpeed * dt;
                _camera.set_position(pos);
            }

            if (glfwGetKey(glfwWindow, GLFW_KEY_SPACE) == GLFW_PRESS)
            {
                vec3 pos = _camera.position();
                pos.y += moveSpeed * _frame_timer.elapsed_time_s();
                _camera.set_position(pos);
            }

            if (glfwGetKey(glfwWindow, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
            {
                vec3 pos = _camera.position();
                pos.y -= moveSpeed * _frame_timer.elapsed_time_s();
                _camera.set_position(pos);
            }
        }

        void update_camera_look_direction()
        {
            // get new pos
            dvec2 pos;
            glfwGetCursorPos(_window.get_GLFW_window(), &(pos.x), &(pos.y));

            // look around when mouse is pressed
            GLFWwindow* glfwWindow = _window.get_GLFW_window();
            if (glfwGetMouseButton(glfwWindow, GLFW_MOUSE_BUTTON_1) == GLFW_RELEASE)
            {
                _window.set_cursor_mode(Window::CursorMode::NORMAL);
                _old_mouse_pos = pos; // so that the view doesn't jump when pressing the button to rotate camera
                return;
            }
            if (glfwGetMouseButton(glfwWindow, GLFW_MOUSE_BUTTON_1) == GLFW_PRESS)
            {
                _window.set_cursor_mode(Window::CursorMode::DISABLED);
            }


            // init mouse pos if this is the first time
            if (-1 <= _old_mouse_pos.x && _old_mouse_pos.x <= -0.95
                && -1 <= _old_mouse_pos.y && _old_mouse_pos.y <= -0.95)
            {
                _old_mouse_pos = pos;
            }

            // movement difference
            dvec2 deltaPos = pos - _old_mouse_pos;
            //deltaPos.y *= -1; // mouse y is negative upwards, flip y-axis

            // turn camera
            double turnSpeed = 0.0005;
            _camera.update_angle(deltaPos.x * turnSpeed, deltaPos.y * turnSpeed);

            _old_mouse_pos = pos;
        }
    };
}

#endif // TEST_APP_H

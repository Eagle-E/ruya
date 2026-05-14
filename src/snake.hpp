#ifndef SNAKE_H
#define SNAKE_H

/* Notes when making the snake game


- Wanted 2 functions head() and body() which would create a new Model instance 
    for the head and body part of the snake. However, access to the vault is needed
    along with the mesh and material IDs.
    => provide global access to vault?



*/


#include <iostream>
#include <fstream>
#include <string>
#include <exception>
#include <memory>
#include <filesystem>
#include <format>
#include <utility>
#include <vector>

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
#include "io/window.h"
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
#include "ui/ui.hpp"

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
using ruya::scene::BasicLight;
using ruya::scene::DirectionalLight;
using ruya::scene::PointLight;
using ruya::scene::Mesh;			
using ruya::scene::MeshID;
using ruya::scene::Element;
using ruya::scene::Model;
using ruya::scene::Scene;
using ruya::scene::Vault;
using ruya::scene::materials::Phong;
using ruya::scene::materials::PhongMaterials;

namespace
{
    enum class RenderMode { FILL, WIREFRAME };

    namespace Key {
        inline constexpr int A = GLFW_KEY_A;
        inline constexpr int B = GLFW_KEY_B;
        inline constexpr int C = GLFW_KEY_C;
        inline constexpr int D = GLFW_KEY_D;
        inline constexpr int E = GLFW_KEY_E;
        inline constexpr int F = GLFW_KEY_F;
        inline constexpr int G = GLFW_KEY_G;
        inline constexpr int H = GLFW_KEY_H;
        inline constexpr int I = GLFW_KEY_I;
        inline constexpr int J = GLFW_KEY_J;
        inline constexpr int K = GLFW_KEY_K;
        inline constexpr int L = GLFW_KEY_L;
        inline constexpr int M = GLFW_KEY_M;
        inline constexpr int N = GLFW_KEY_N;
        inline constexpr int O = GLFW_KEY_O;
        inline constexpr int P = GLFW_KEY_P;
        inline constexpr int Q = GLFW_KEY_Q;
        inline constexpr int R = GLFW_KEY_R;
        inline constexpr int S = GLFW_KEY_S;
        inline constexpr int T = GLFW_KEY_T;
        inline constexpr int U = GLFW_KEY_U;
        inline constexpr int V = GLFW_KEY_V;
        inline constexpr int W = GLFW_KEY_W;
        inline constexpr int X = GLFW_KEY_X;
        inline constexpr int Y = GLFW_KEY_Y;
        inline constexpr int Z = GLFW_KEY_Z;
    }
}

namespace ruya
{

    class Snake : public App
    {
    private: // VARIABLES
        Camera _camera;
        Timer _frame_timer;
        Timer _frame_output_timer;
        Window _window {1450, 875};
        dvec2 _old_mouse_pos {-1.0, -1.0};
        bool _allow_shading_mode_change = true;
        bool _allow_render_mode_change = true;
        RenderMode _render_mode = RenderMode::FILL;
        Renderer* _renderer;
        Vault _vault;
        Scene _scene;
        Model _snake_body_part;
        std::vector<entt::entity> _snek;

    public: // FUNCTIONS

        void clear_frame_buffer(glm::vec3 color)
        {
            glClearColor(color.r, color.g, color.b, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        }

        /*** MAINLOOP ***/
        void run()
        {
            // window settings
            _window.make_context_current();
            glfwSetInputMode(_window.get_GLFW_window(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);

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
            

			// the object to render
            // ruya::render::print_max_texture_units_info();
            
            init_scene(_scene);
            _camera.set_position(vec3{5, 5, 17.5f});
        	ruya::ui::initialize(_window.get_GLFW_window());

            // MAIN LOOP
            _frame_output_timer.start();
            while (!_window.should_close())
            {
                _frame_timer.start();

                // prepare ui
                ruya::ui::new_frame();
                {
                    ruya::ui::settings_pane(_scene, _vault);
                }
                
				// RENDER!!!
                clear_frame_buffer(_scene.background_color);
                renderer.render_scene(_scene, _vault);
                ruya::ui::render_frame();

                // update frame => swaps buffers = starts showing newly rendered buffer
                // + checks for input events and calls handlers
                _window.update();

                // calc FPS
                log_fps();
                poll_and_process_events();
            }

            // cleanup
            ruya::ui::shutdown();
            glfwTerminate(); // clean up all reasources allocated by glfw.
        }



        void init_scene(Scene& scene)
        {
            std::cout << "Initializing Scene" << std::endl;


			fs::path texPathBarrelColor {ruya::io::DIR_RESOURCES / "barrel" / "barrel_color.png"};
			fs::path texPathBarrelSpecular {ruya::io::DIR_RESOURCES / "barrel" / "barrel_specular.png"};
			fs::path texPathEmojiColor {ruya::io::DIR_RESOURCES / "emoji" / "emoji_color.png"};
            ImageID barrel_img_id_color = _vault.load_image(texPathBarrelColor);
            ImageID barrel_img_id_specular = _vault.load_image(texPathBarrelSpecular);
            ImageID emoji_img_id = _vault.load_image(texPathEmojiColor);
            _vault.add_mesh(gen::cube(), "gen::cube");
            _vault.add_mesh(gen::square(), "gen::square");
            _vault.add_mesh(gen::icosahedron(), "gen::icosahedron");
            _vault.add_mesh(gen::icosphere(), "gen::icosphere");
			std::cout << "Init textures" << std::endl;

            scene.background_color = vec3(.1f, .1f, .1f);

            // snake parts
            Model model;
            model.elements.push_back(
                Element{
                    .mesh = _vault.mesh_cache["gen::cube"],
                    .material = Phong{
                        .diffuse_map = barrel_img_id_color,
                        .specular_map = barrel_img_id_specular
                    },
                    .transform = Transform{
                        .position = vec3{0},
                        .scale = vec3(1.0f)
                    }
                }
            );
            _snake_body_part = model;


            Model snake_head;
            snake_head.elements.push_back(
                Element{
                    .mesh = _vault.mesh_cache["gen::cube"],
                    .material = Phong{
                        .diffuse_map = emoji_img_id,
                        .specular_map = barrel_img_id_specular
                    },
                    .transform = Transform{
                        .position = vec3{5, 5, .0f},
                        .scale = vec3{1.0f, 1.0f, 1.0f}
                    }
                }
            );
            auto head_entity = scene.registry.create();
            scene.registry.emplace<Model>(head_entity, snake_head);
            _snek.clear();
            _snek.push_back(head_entity);

            

            // directional light
            DirectionalLight dir_light {
                .direction = vec3{0.0f, -1.0f, -1.0f},
                .ambient = vec3(0.2f, 0.2f, 0.2f),
                .diffuse = vec3(0.0f, 0.0f, 0.0f),
                .specular = vec3(0.0f, 0.0f, 0.0f),
            };
            auto light_entity3 = scene.registry.create();
            scene.registry.emplace<DirectionalLight>(light_entity3, dir_light);

            // point lights
            float d = 11.0f;
            PointLight point_light_tl {.position {0, d, 0.0f}};
            PointLight point_light_tr {.position {d, d, 0.0f}};
            PointLight point_light_bl {.position {0, 0, 0.0f}};
            PointLight point_light_br {.position {d, 0, 0.0f}};
            PointLight point_light_tm {.position {d/2, d, 0.0f}};
            PointLight point_light_rm {.position {d, d/2, 0.0f}};
            PointLight point_light_bm {.position {d/2, 0, 0.0f}};
            PointLight point_light_lm {.position {0, d/2, 0.0f}};
            Model point_light_model
            {
                .elements = {
                    Element{
                        .mesh = _vault.mesh_cache["gen::cube"],
                        .material = Phong{},
                        .transform = Transform{
                            .position = vec3{0},
                            .rotation = vec3(0),
                            .scale = vec3(1.0f)
                        }
                    }
                }
            };
            auto point_light_entity_0 = scene.registry.create();
            auto point_light_entity_1 = scene.registry.create();
            auto point_light_entity_2 = scene.registry.create();
            auto point_light_entity_3 = scene.registry.create();
            auto point_light_entity_4 = scene.registry.create();
            auto point_light_entity_5 = scene.registry.create();
            auto point_light_entity_6 = scene.registry.create();
            auto point_light_entity_7 = scene.registry.create();
            scene.registry.emplace<PointLight>(point_light_entity_0, point_light_tl);
            scene.registry.emplace<PointLight>(point_light_entity_1, point_light_tr);
            scene.registry.emplace<PointLight>(point_light_entity_2, point_light_bl);
            scene.registry.emplace<PointLight>(point_light_entity_3, point_light_br);
            scene.registry.emplace<PointLight>(point_light_entity_4, point_light_tm);
            scene.registry.emplace<PointLight>(point_light_entity_5, point_light_rm);
            scene.registry.emplace<PointLight>(point_light_entity_6, point_light_bm);
            scene.registry.emplace<PointLight>(point_light_entity_7, point_light_lm);
            scene.registry.emplace<Model>(point_light_entity_0, point_light_model);
            scene.registry.emplace<Model>(point_light_entity_1, point_light_model);
            scene.registry.emplace<Model>(point_light_entity_2, point_light_model);
            scene.registry.emplace<Model>(point_light_entity_3, point_light_model);
            scene.registry.emplace<Model>(point_light_entity_4, point_light_model);
            scene.registry.emplace<Model>(point_light_entity_5, point_light_model);
            scene.registry.emplace<Model>(point_light_entity_6, point_light_model);
            scene.registry.emplace<Model>(point_light_entity_7, point_light_model);
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
            
            GLFWwindow* glfw_window = _window.get_GLFW_window();
            
            if (glfwGetKey(glfw_window, GLFW_KEY_2) == GLFW_PRESS && _allow_shading_mode_change)
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

            if (glfwGetKey(glfw_window, GLFW_KEY_2) == GLFW_RELEASE)
            {
                _allow_shading_mode_change = true;
            }

            if (glfwGetKey(glfw_window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
                glfwSetWindowShouldClose(glfw_window, true);


            // CAMERA CONTROL
            // update camera interaction when user is not interacting with imgui
            if (!imgui_io.WantCaptureMouse) 
            {
                // update_camera_position();
                update_snake_position();
                update_camera_look_direction();
                update_render_mode();
            }
        }

        void update_render_mode()
        {
            GLFWwindow* glfw_window = _window.get_GLFW_window();

            if (glfwGetKey(glfw_window, GLFW_KEY_1) == GLFW_PRESS && _allow_render_mode_change)
            {
                switch (_render_mode)
                {
                case RenderMode::FILL:
                    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
                    _render_mode = RenderMode::WIREFRAME;
                    break;
                case RenderMode::WIREFRAME:
                    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
                    _render_mode = RenderMode::FILL;
                    break;
                }

                _allow_render_mode_change = false;
            }
            else if (glfwGetKey(glfw_window, GLFW_KEY_1) == GLFW_RELEASE)
            {
                _allow_render_mode_change = true;
            }
        }


        void update_snake_position()
        {
            GLFWwindow* glfw_window = _window.get_GLFW_window();
            float step = .05f;
            float delta_x = 0;
            float delta_y = 0;
            if (glfwGetKey(glfw_window, Key::W) == GLFW_PRESS)
            {
                delta_y += step;
            }
            
            if (glfwGetKey(glfw_window, Key::S) == GLFW_PRESS)
            {
                delta_y -= step;
            }
            
            if (glfwGetKey(glfw_window, Key::A) == GLFW_PRESS)
            {
                delta_x -= step;
            }
            
            if (glfwGetKey(glfw_window, Key::D) == GLFW_PRESS)
            {
                delta_x += step;
            }
            Model& head = _scene.registry.get<Model>(_snek[0]);
            head.elements[0].transform.position += vec3{delta_x, delta_y, 0};
        }

        void update_camera_position()
        {
            GLFWwindow* glfw_window = _window.get_GLFW_window();
            
            // move camera forward/backward/left/right perpendicular with the xz plane
            // move camera up/down along y-axis
            float moveSpeed = 6.0f; // units per second
            float dt = _frame_timer.elapsed_time_s();

            if (glfwGetKey(glfw_window, GLFW_KEY_W) == GLFW_PRESS)
            {
                vec3 direction = _camera.cam_front();
                vec2 moveDirection = glm::normalize(glm::vec2(direction.x, direction.z));
                vec3 pos = _camera.position();
                pos.x += moveDirection.x * moveSpeed * dt;
                pos.z += moveDirection.y * moveSpeed * dt;
                _camera.set_position(pos);
            }

            if (glfwGetKey(glfw_window, GLFW_KEY_S) == GLFW_PRESS)
            {
                vec3 direction = _camera.cam_front();
                vec2 moveDirection = glm::normalize(glm::vec2(direction.x, direction.z));
                vec3 pos = _camera.position();
                pos.x -= moveDirection.x * moveSpeed * dt;
                pos.z -= moveDirection.y * moveSpeed * dt;
                _camera.set_position(pos);
            }

            if (glfwGetKey(glfw_window, GLFW_KEY_A) == GLFW_PRESS)
            {
                vec3 direction = _camera.cam_front();
                vec2 moveDirection = glm::normalize(glm::vec2(direction.x, direction.z));
                moveDirection = vec2(moveDirection.y, -moveDirection.x); // turn clockwise 90deg from forward direction
                vec3 pos = _camera.position();
                pos.x += moveDirection.x * moveSpeed * dt;
                pos.z += moveDirection.y * moveSpeed * dt;
                _camera.set_position(pos);
            }

            if (glfwGetKey(glfw_window, GLFW_KEY_D) == GLFW_PRESS)
            {
                vec3 direction = _camera.cam_front();
                vec2 moveDirection = glm::normalize(glm::vec2(direction.x, direction.z));
                moveDirection = vec2(-moveDirection.y, moveDirection.x); // turn clockwise 90deg from forward direction
                vec3 pos = _camera.position();
                pos.x += moveDirection.x * moveSpeed * dt;
                pos.z += moveDirection.y * moveSpeed * dt;
                _camera.set_position(pos);
            }

            if (glfwGetKey(glfw_window, GLFW_KEY_SPACE) == GLFW_PRESS)
            {
                vec3 pos = _camera.position();
                pos.y += moveSpeed * _frame_timer.elapsed_time_s();
                _camera.set_position(pos);
            }

            if (glfwGetKey(glfw_window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
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
            GLFWwindow* glfw_window = _window.get_GLFW_window();
            if (glfwGetMouseButton(glfw_window, GLFW_MOUSE_BUTTON_1) == GLFW_RELEASE)
            {
                _window.set_cursor_mode(Window::CursorMode::NORMAL);
                _old_mouse_pos = pos; // so that the view doesn't jump when pressing the button to rotate camera
                return;
            }
            if (glfwGetMouseButton(glfw_window, GLFW_MOUSE_BUTTON_1) == GLFW_PRESS)
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

#endif // SNAKE_H

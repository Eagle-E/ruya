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
#include <cmath>
#include <ranges>

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
#include "core/utils.hpp"
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
using glm::ivec2;
using glm::vec2;
using glm::ivec3;
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
using ruya::io::Window;
using ruya::io::key_pressed;
using ruya::io::Key;

namespace
{
    enum class RenderMode { FILL, WIREFRAME };

}

namespace ruya
{
    class Snake : public App
    {
    private: // VARIABLES
        Camera _camera;
        Window _window {1450, 875};
        dvec2 _old_mouse_pos {-1.0, -1.0};
        bool _allow_shading_mode_change = true;
        bool _allow_render_mode_change = true;
        RenderMode _render_mode = RenderMode::FILL;
        Renderer* _renderer;
        
        // -------------------- snake state --------------------
        Vault _vault;
        Scene _scene; // GRAPHICS
        
        Model _snake_body_part; // GRAPHICS
        std::vector<entt::entity> _snek; // GRAPHICS
        std::vector<ivec2> _snek_pos; // GAME STATE
        
        Timer _snek_timer_apples;
        uint32_t _max_apples = 6; // GAME STATE
        std::vector<ivec2> _apple_locs; // GAME STATE
        std::vector<entt::entity> _apples; // GRAPHICS
        
        Timer _snek_timer_movement;
        float _remaining_cells{0.0f}; // GRAPHICS
        ivec2 _last_dir {.0f, .0f}; // ?
        const float CELLS_PER_SECOND = 4;
        const float CELL_SIZE = 1.0f; // GRAPHICS
        const float ROWS = 8; // GAME STATE
        const float COLS = 12; // GAME STATE
        
        // -----------------------------------------------------



    public: // FUNCTIONS

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
            
            // ----------------------------------------------------------------------------------------------------
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

            _scene.background_color = vec3(.1f, .1f, .1f);

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
                        .scale = vec3(.9f)
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
                        // .position = vec3{.0f},
                        .scale = vec3{1.0f, 1.0f, 1.0f}
                    }
                }
            );
            auto head_entity = _scene.registry.create();
            _scene.registry.emplace<Model>(head_entity, snake_head);
            _snek.clear();
            _snek.push_back(head_entity);
            _snek_pos.clear();
            _snek_pos.push_back(ivec2{0, 0});


            // floor
            Model floor_model;
            floor_model.elements.push_back(
                Element{
                    .mesh = _vault.mesh_cache["gen::cube"],
                    .material = Phong{
                        .diffuse = vec3{1.0f},
                        .specular = vec3{0.0f},
                    },
                    .transform = Transform{
                        .position = vec3{COLS*CELL_SIZE/2, ROWS*CELL_SIZE/2, -CELL_SIZE},
                        .scale = vec3{COLS*CELL_SIZE, ROWS*CELL_SIZE, 1.0f}
                    }
                }
            );
            auto floor_entity = _scene.registry.create();
            _scene.registry.emplace<Model>(floor_entity, floor_model);

            // walls
            Model wall;
            wall.elements.push_back(
                Element{
                    .mesh = _vault.mesh_cache["gen::cube"],
                    .material = Phong{
                        .diffuse = vec3{0.7f, 0.7f, 0.3f},
                        .specular = vec3{0.0f},
                    }
                }
            );
            auto top_wall_entity = _scene.registry.create();
            _scene.registry.emplace<Model>(top_wall_entity, wall);
            _scene.registry.get<Model>(top_wall_entity).elements[0].transform = Transform{
                .position = vec3{COLS*CELL_SIZE/2, (ROWS+1)*CELL_SIZE, 0},
                .scale = vec3{COLS*CELL_SIZE, CELL_SIZE, 1.0f}
            };
            auto bottom_wall_entity = _scene.registry.create();
            _scene.registry.emplace<Model>(bottom_wall_entity, wall);
            _scene.registry.get<Model>(bottom_wall_entity).elements[0].transform = Transform{
                .position = vec3{COLS*CELL_SIZE/2, -CELL_SIZE, 0},
                .scale = vec3{COLS*CELL_SIZE, CELL_SIZE, 1.0f}
            };
            auto right_wall_entity = _scene.registry.create();
            _scene.registry.emplace<Model>(right_wall_entity, wall);
            _scene.registry.get<Model>(right_wall_entity).elements[0].transform = Transform{
                .position = vec3{(COLS+1)*CELL_SIZE, ROWS*CELL_SIZE/2, 0},
                .scale = vec3{CELL_SIZE, ROWS*CELL_SIZE, 1.0f}
            };
            auto left_wall_entity = _scene.registry.create();
            _scene.registry.emplace<Model>(left_wall_entity, wall);
            _scene.registry.get<Model>(left_wall_entity).elements[0].transform = Transform{
                .position = vec3{-CELL_SIZE, ROWS*CELL_SIZE/2, 0},
                .scale = vec3{CELL_SIZE, ROWS*CELL_SIZE, 1.0f}
            };

            // directional light
            DirectionalLight dir_light {
                .direction = vec3{0.0f, 0.0f, -1.0f},
                .ambient = vec3(0.2f, 0.2f, 0.2f),
                .diffuse = vec3(0.9f, 0.9f, 0.9f),
                .specular = vec3(0.0f, 0.0f, 0.0f),
            };
            auto light_entity3 = _scene.registry.create();
            _scene.registry.emplace<DirectionalLight>(light_entity3, dir_light);
            _camera.set_position(vec3{5, 5, 17.5f});
        	ruya::ui::initialize(_window.get_GLFW_window());

            // MAIN LOOP
            _snek_timer_movement.start();
            _snek_timer_apples.start();
            while (!_window.should_close())
            {
                // prepare ui
                ruya::ui::new_frame();
                {
                    ruya::ui::settings_pane(_scene, _vault);
                }
                
				// RENDER!!!
                renderer.render_scene(_scene, _vault);
                ruya::ui::render_frame();
                _window.update();

                // game logic
                input_system(_scene.registry);
                step();
                // TODO: separate game state from graphics state and add a sync phase
            }
            
            // cleanup
            ruya::ui::shutdown();
            glfwTerminate(); // clean up all reasources allocated by glfw.
        }
        
        void spawn_snake_body_part(ivec2 body_part_loc)
        {
            _snek_pos.push_back(body_part_loc);
            entt::entity new_part_entity = _scene.registry.create();
            _scene.registry.emplace<Model>(new_part_entity, _snake_body_part);
            _snek.push_back(new_part_entity);

            // sync snake part coordinate with world position
            Model & body_model = _scene.registry.get<Model>(new_part_entity);
            for (auto & elem : body_model.elements)
            {
                elem.transform.position = vec3{
                    body_part_loc.x * CELL_SIZE,
                    body_part_loc.y * CELL_SIZE,
                    0
                };
            }
        }

        void input_system(entt::registry& registry)
        {
            if(key_pressed(_window, Key::ESCAPE))
                _window.set_should_close(true);

            if (!ImGui::GetIO().WantCaptureMouse) 
            {
                // scene interaction should come here when the ui doesn't capture the input
                poll_snake_direction();
            }
        }

        

        /** A single step in the snake game
         * TODO: game over
         */
        void step()
        {
            // check if snake head overlaps with body
            // TODO impl
            
            // check if snake head overlaps with an apple
            ivec2 head_pos = _snek_pos[0];
            for (auto [idx, apple_pos] : std::views::enumerate(_apple_locs))
            {
                if (head_pos == apple_pos)
                {
                    // TODO: consume apply + grow
                    // consume apple
                    std::swap(_apple_locs[idx], _apple_locs.back());
                    _apple_locs.pop_back();
                    std::swap(_apples[idx], _apples.back());
                    entt::entity apple_to_destroy = _apples.back();
                    _apples.pop_back();
                    _scene.registry.destroy(apple_to_destroy);
                    
                    // grow snake
                    spawn_snake_body_part(_snek_pos.back());

                    break;
                }
            }

            update_snake_position();
            spawn_apple();
        }

        ivec2 random_position(ivec2 bound_min, ivec2 bound_max)
        {
            int x = rand_int(bound_min.x, bound_max.x);
            int y = rand_int(bound_min.y, bound_max.y);
            return ivec2{x, y};
        }


        /** Spawns a single apple on the map
         * TODO: fix apples spawning outside the map
         * TODO: make sure apples don't spawn on top of each other
         * TODO: make sure an apple doesn't spawn on the snake
         * TODO: create apple component and manage data in entt
         */
        void spawn_apple()
        {
            if (_apple_locs.size() >= _max_apples)
            return;
            
            // TODO: move location storage of apples to entt
            ivec2 new_pos = random_position(ivec2{0}, ivec2{COLS, ROWS});
            _apple_locs.push_back(new_pos);

            // create and register the apple entity
            Model apple_model;
            apple_model.elements.push_back(
                Element{
                    .mesh = _vault.mesh_cache["gen::cube"],
                    .material = Phong{
                        .diffuse = vec3{1.0f, 0.0f, 0.0f},
                    },
                    .transform = Transform{
                        .position = vec3{new_pos.x * CELL_SIZE, new_pos.y * CELL_SIZE, 0.0f},
                        .scale = vec3{0.75f, 0.75f, 0.75f}
                    }
                }
            );

            auto apple_entity = _scene.registry.create();
            _scene.registry.emplace<Model>(apple_entity, apple_model);
            _apples.push_back(apple_entity);
        }
        


        void poll_snake_direction()
        {
            auto _pressed = [&window = _window](Key k){
                return key_pressed(window, k);
            };

            if(_pressed(Key::W) || _pressed(Key::UP))
            {
                if (_last_dir.y >= 0)
                _last_dir = {0, 1};
            }
            
            if(_pressed(Key::S) || _pressed(Key::DOWN))
            {
                if (_last_dir.y <= 0)
                _last_dir = {0, -1};
            }
            
            if(_pressed(Key::A) || _pressed(Key::LEFT))
            {
                if (_last_dir.x <= 0)
                _last_dir = {-1, 0};
            }
            
            if(_pressed(Key::D) || _pressed(Key::RIGHT))
            {
                if (_last_dir.x >= 0)
                    _last_dir = {1, 0};
            }
        }

        void update_snake_position()
        {
            // movement speed
            _snek_timer_movement.stop();
            float seconds = static_cast<float>(_snek_timer_movement.elapsed_time_s());
            float cells_to_move = CELLS_PER_SECOND * seconds;// + _remaining_cells;
            float move_now_f = std::trunc(cells_to_move);
            int move_now = static_cast<int>(move_now_f);
            // _remaining_cells = cells_to_move - move_now_f; // TODO ??
            if (move_now == 0)
                return;

            // Update coordinates of each snek part
            ivec2 prev_pos;
            for (auto [idx, pos] : std::views::enumerate(_snek_pos))
            {
                if (idx == 0)
                {
                    prev_pos = pos;
                    pos.x += _last_dir.x * move_now;
                    pos.y += _last_dir.y * move_now;
                }
                else
                {
                    ivec2 pos_for_next_part = pos;
                    pos.x = prev_pos.x;
                    pos.y = prev_pos.y;
                    prev_pos = pos_for_next_part;
                }
            }
            
            
            // update world coordinate of snek based on the 2d coordinates
            for (auto [idx, pos] : std::views::enumerate(_snek_pos))
            {
                Model& head = _scene.registry.get<Model>(_snek[idx]);
                for (auto &elem : head.elements)
                {
                    elem.transform.position.x = pos.x * CELL_SIZE;
                    elem.transform.position.y = pos.y * CELL_SIZE;
                    // break;
                }
                // head.elements[0].transform.position.x = pos.x * CELL_SIZE;
                // head.elements[0].transform.position.y = pos.y * CELL_SIZE;
            }
            _snek_timer_movement.start();
        }
    };
}

#endif // SNAKE_H

#ifndef RENDERER_H
#define RENDERER_H

#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>
#include <list>
#include <memory>
#include <unordered_map>

#include "core/window.h"
#include "render/gpu_vault.hpp"
#include "render/shader.h"
#include "render/texture.hpp"
#include "scene/camera.h"
#include "scene/components.hpp"
#include "scene/light.hpp"
#include "scene/mesh.h"
#include "scene/scene.hpp"
#include "scene/vault.hpp"

using glm::mat4;
using ruya::Camera;
using ruya::Window;
using ruya::render::MeshGPU;
using ruya::render::Shader;
using ruya::scene::Element;
using ruya::scene::Model;
using ruya::scene::BasicLight;
using ruya::scene::MeshID;
using ruya::scene::Scene;
using ruya::scene::Vault;
using std::list;
using std::shared_ptr;
using std::unordered_map;

namespace ruya::render
{

	

	/*
	* A basic temporary renderer which will render a given scene
	*/
	class Renderer
	{
	public:
		enum class ShadingMode { SMOOTH, FLAT };

		Renderer(Shader* shader_objects, Shader* shader_lights, Window* window, Camera* camera);
		~Renderer();
		void render_scene(Scene& scene, Vault& vault);
		void set_flat_shader(Shader* flatShader) { _shader_objects_flat = flatShader; }
		void set_shading_mode(ShadingMode mode) { _shading_mode = mode; }
		ShadingMode shading_mode() const { return _shading_mode; }

		GPUVault gpu_vault;
		const int TEXTURE_SLOT_DIFFUSE  = 0;
		const int TEXTURE_SLOT_SPECULAR = 1;
		const int MAX_BASIC_LIGHTS = 8;
		const int MAX_DIRECTIONAL_LIGHTS = 4;
		const int MAX_POINT_LIGHTS = 32;

	private:
		static void GLAPIENTRY debug_mesage_callback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, 
														const GLchar* message, const void* userParam);

		void render_models(entt::registry& registry, Vault& vault, const mat4& VP, Shader& activeShader);
		void render_light_source(vec3 position, vec3 color, Model& model, const mat4& VP, Vault& vault);
		void draw_mesh(MeshID mesh, Vault& vault);

		// MeshGPU buffer_mesh(const Mesh& mesh);
		void unload_mesh(MeshGPU& mesh_gpu);
		Shader* _shader_objects_smooth;
		Shader* _shader_lights;
		Shader* _shader_objects_flat;
		Window* _window;
		Camera* _camera;
		ShadingMode _shading_mode;
	};
}

#endif

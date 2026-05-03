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

		Renderer(Shader* shaderObjects, Shader* shaderLights, Window* window, Camera* camera);
		~Renderer();
		void render_scene(Scene& scene, Vault& vault);
		void set_flat_shader(Shader* flatShader) { mFlatShaderObjects = flatShader; }
		void set_shading_mode(ShadingMode mode) { mShadingMode = mode; }
		ShadingMode shading_mode() const { return mShadingMode; }

		GPUVault gpu_vault;
		const int TEXTURE_SLOT_DIFFUSE  = 0;
		const int TEXTURE_SLOT_SPECULAR = 1;

	private:
		static void GLAPIENTRY debug_mesage_callback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, 
														const GLchar* message, const void* userParam);

		void render_models(entt::registry& registry, Vault& vault, const mat4& VP, Shader& activeShader);
		void render_model(Model& obj, const mat4& VP, const BasicLight& light, Shader * activeShader, Vault& vault);
		void render_element(Element& obj, const mat4& VP, const BasicLight& light, Shader * activeShader, Vault& vault);
		void render_light_source(BasicLight& light, const mat4& VP, Vault& vault, Model& model);
		void draw_mesh(MeshID mesh, Vault& vault);

		// MeshGPU buffer_mesh(const Mesh& mesh);
		void unload_mesh(MeshGPU& mesh_gpu);
		Shader* mSmoothShaderObjects;
		Shader* mShaderLights;
		Shader* mFlatShaderObjects;
		Window* mWindow;
		Camera* mCamera;
		ShadingMode mShadingMode;

		unordered_map<MeshID, MeshGPU> mMeshHandleMap;
		// TextureSlotManager mSlotManager;
	};
}

#endif

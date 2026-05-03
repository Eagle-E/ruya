#include <list>
#include <iostream>
#include <format>

#include <entt/entt.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "glm/gtx/string_cast.hpp"

#include "render/renderer.h"
#include "render/texture.hpp"
#include "scene/components.hpp"
#include "scene/vault.hpp"
#include "render/gpu_vault.hpp"

using ruya::render::MeshGPU;
using ruya::render::resolve_mesh;
using ruya::render::resolve_texture;
using ruya::scene::Model;
using ruya::scene::TextureID;
using ruya::scene::Vault;
using ruya::scene::materials::Phong;

using std::list;
using glm::mat4;	using glm::mat3;

ruya::render::Renderer::Renderer(Shader* shaderObjects, Shader* shaderLights, Window* window, Camera* camera)
	: mWindow(window), mCamera(camera), mSmoothShaderObjects(shaderObjects), mShaderLights(shaderLights),
	  mFlatShaderObjects(nullptr), mShadingMode(ShadingMode::SMOOTH)
{
	// enable depth test
	glEnable(GL_DEPTH_TEST);

	// During init, enable debug output
	glEnable(GL_DEBUG_OUTPUT);
	glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
	glDebugMessageCallback(debug_mesage_callback, 0);
}

ruya::render::Renderer::~Renderer()
{
}

void ruya::render::Renderer::render_scene(Scene& scene, Vault& vault)
{
	// OBJECTS
	// activate object shader to render objects
	Shader* activeObjectShader = nullptr;
	switch (mShadingMode)
	{
		case ShadingMode::SMOOTH:	activeObjectShader = mSmoothShaderObjects;	break;
		case ShadingMode::FLAT:		activeObjectShader = mFlatShaderObjects;	break;
	}
	activeObjectShader->use();

	ruya::render::sync_vaults(vault, gpu_vault);

	// get view-projection matrix
	mat4 projection = glm::perspective(glm::radians(mCamera->fov()), mWindow->aspect_ratio(), 0.1f, 300.0f);
	mat4 VP = projection * mCamera->view_matrix();

	// render scene objects
	//! auto lights_view = scene.registry.view<BasicLight>();
	//! auto light_entity = lights_view.front();
	//! if (light_entity == entt::null)
	//! {
	//! 	return;
	//! }
	//! BasicLight& light = lights_view.get<BasicLight>(light_entity);

	//! auto model_view = scene.registry.view<Model>(entt::exclude<BasicLight>);
	//! for (auto [entity, model] : model_view.each())
	//! {
	//! 	render_model(model, VP, light, activeObjectShader, vault);
	//! }
	render_models(scene.registry, vault, VP, *activeObjectShader);

	// LIGHT SOURCES
	mShaderLights->use();
	auto lights_with_model_view = scene.registry.view<BasicLight, Model>();
	lights_with_model_view.each([&](auto entity, BasicLight& light, Model& model) {
		render_light_source(light, VP, vault, model);
	});
}


void ruya::render::Renderer::render_models(entt::registry& registry, Vault& vault, const mat4& VP, Shader& active_shader)
{
	// set the lights, as they're the same for all models
	auto lights_view = registry.view<BasicLight>();
	int idx = 0;
	lights_view.each(
		[&](auto entity, BasicLight& light)
		{
			active_shader.set_vec3(std::format("simple_lights[{}].ambient", idx), light.ambient);
			active_shader.set_vec3(std::format("simple_lights[{}].diffuse", idx), light.diffuse);
			active_shader.set_vec3(std::format("simple_lights[{}].specular", idx), light.specular);
			active_shader.set_vec3(std::format("simple_lights[{}].position", idx), light.position);
			idx++;
		}
	);

	// camera stuff, same for all models
	active_shader.set_vec3("camera_position", mCamera->position());

	// render the models
	auto model_view = registry.view<Model>(entt::exclude<BasicLight>);
	for (auto [entity, model] : model_view.each())
	{
		for (auto & elem : model.elements)
		{
			// Bind the textures and set their uniform location
			ImageID diffuse_id = elem.material.diffuse_map;
			Texture texture_diffuse = resolve_texture(diffuse_id, vault, gpu_vault);
			glActiveTexture(GL_TEXTURE0 + TEXTURE_SLOT_DIFFUSE);
			glBindTexture(GL_TEXTURE_2D, texture_diffuse.id);
			active_shader.set_int("material.diffuse_map", TEXTURE_SLOT_DIFFUSE);

			ImageID specular_id = elem.material.specular_map;
			Texture texture_specular = resolve_texture(specular_id, vault, gpu_vault);
			glActiveTexture(GL_TEXTURE0 + TEXTURE_SLOT_SPECULAR);
			glBindTexture(GL_TEXTURE_2D, texture_specular.id);
			active_shader.set_int("material.specular_map", TEXTURE_SLOT_SPECULAR);

			// material uniform
			Phong& material = elem.material;
			active_shader.set_vec3("material.diffuse", material.diffuse);
			active_shader.set_vec3("material.specular", material.specular);
			active_shader.set_float("material.shininess", material.shininess);

			// calc model-view-projection matrix
			mat4 M = ruya::math::model_matrix(elem.transform);
			mat4 N = glm::transpose(glm::inverse(M));
			active_shader.set_mat4("M", M);
			active_shader.set_mat4("N", N);
			active_shader.set_mat4("VP", VP);

			// render mesh
			draw_mesh(elem.mesh, vault);
		}	
	}
}


/*
* Handles the necessary OpenGL calls to render the model with the shader program
* that this Renderer has. If the Mesh of the model is being rendered for the first
* time, the necessary buffers (VAO, VBO & EBO) will be created automatically.
* 
* Args:
* - model: model to render
* - VP: view projection matrix
* - ... the rest is self explanatory
*
* @pre the correct shader program needs to be made current before calling this function.
*/
void ruya::render::Renderer::render_model(
	Model& model,
	const mat4& VP,
	const BasicLight& light,
	Shader* activeShader,
	Vault& vault
)
{	
	for (auto & elem : model.elements)
	{
		render_element(elem, VP, light, activeShader, vault);
	}
}

void ruya::render::Renderer::render_element(
	Element& element,
	const mat4& VP,
	const BasicLight& light,
	Shader* activeShader,
	Vault& vault
)
{	
	// Bind the textures and set their uniform location
	ImageID diffuse_id = element.material.diffuse_map;
	Texture texture_diffuse = resolve_texture(diffuse_id, vault, gpu_vault);
	glActiveTexture(GL_TEXTURE0 + TEXTURE_SLOT_DIFFUSE);
	glBindTexture(GL_TEXTURE_2D, texture_diffuse.id);
	activeShader->set_int("material.diffuse_map", TEXTURE_SLOT_DIFFUSE);

	ImageID specular_id = element.material.specular_map;
	Texture texture_specular = resolve_texture(specular_id, vault, gpu_vault);
	glActiveTexture(GL_TEXTURE0 + TEXTURE_SLOT_SPECULAR);
	glBindTexture(GL_TEXTURE_2D, texture_specular.id);
	activeShader->set_int("material.specular_map", TEXTURE_SLOT_SPECULAR);

	// pass uniform data
	activeShader->set_vec3("camera_position", mCamera->position());
	
	// material uniform
	Phong& material = element.material;
	activeShader->set_vec3("material.diffuse", material.diffuse);
	activeShader->set_vec3("material.specular", material.specular);
	activeShader->set_float("material.shininess", material.shininess);
	
	// light uniform
	activeShader->set_vec3("simple_light.ambient", light.ambient);
	activeShader->set_vec3("simple_light.diffuse", light.diffuse);
	activeShader->set_vec3("simple_light.specular", light.specular);
	activeShader->set_vec3("simple_light.position", light.position);
	
	// calc model-view-projection matrix
	mat4 M = ruya::math::model_matrix(element.transform);
	mat4 N = glm::transpose(glm::inverse(M));
	activeShader->set_mat4("M", M);
	activeShader->set_mat4("N", N);
	activeShader->set_mat4("VP", VP);

	// render mesh
	draw_mesh(element.mesh, vault);
}

void ruya::render::Renderer::render_light_source(
	BasicLight& light,
	const mat4& VP,
	Vault& vault,
	Model& model
)
{
	mShaderLights->set_vec3("obj_color", light.diffuse);
	
	for (const Element& element : model.elements)
	{
		// TODO: remove `position` member var from light
		// edge case: light has model.transform component but also a separate vec3 position.
		Transform T = element.transform;
		T.position += light.position;

		mat4 M = ruya::math::model_matrix(T);
		mShaderLights->set_mat4("M", M);
		mShaderLights->set_mat4("VP", VP);
		draw_mesh(element.mesh, vault);
	}
}




void GLAPIENTRY ruya::render::Renderer::debug_mesage_callback(
	GLenum source,
	GLenum type,
	GLuint id,
	GLenum severity,
	GLsizei length,
	const GLchar* message,
	const void* userParam
){
	if (severity == GL_DEBUG_SEVERITY_NOTIFICATION) return;

	const char* strSeverity = "Unknown";
	switch (severity)
	{
		case GL_DEBUG_SEVERITY_HIGH: strSeverity = "High"; break;
		case GL_DEBUG_SEVERITY_MEDIUM: strSeverity = "Medium"; break;
		case GL_DEBUG_SEVERITY_LOW: strSeverity = "Low"; break;
		case GL_DEBUG_SEVERITY_NOTIFICATION: strSeverity = "Notification"; break;
	}

	const char* strType = "Unknown";
	switch (type)
	{
		case GL_DEBUG_TYPE_ERROR: strType = "** ERROR **";
		case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: strType = "Depracated Behavior";
		case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR: strType = "Undefined Behavior";
		case GL_DEBUG_TYPE_PORTABILITY: strType = "Portability";
		case GL_DEBUG_TYPE_PERFORMANCE: strType = "Performance";
		case GL_DEBUG_TYPE_OTHER: strType = "Other";
		case GL_DEBUG_TYPE_MARKER: strType = "Marker";
		case GL_DEBUG_TYPE_PUSH_GROUP: strType = "Push Group";
		case GL_DEBUG_TYPE_POP_GROUP: strType = "Pop Group";
	}

	const char* strSource = "Unknown";
	switch (source)
	{
		case GL_DEBUG_SOURCE_API: strSource = "API";
		case GL_DEBUG_SOURCE_WINDOW_SYSTEM: strSource = "Window System";
		case GL_DEBUG_SOURCE_SHADER_COMPILER: strSource = "Shader Compiler";
		case GL_DEBUG_SOURCE_THIRD_PARTY: strSource = "Third Party";
		case GL_DEBUG_SOURCE_APPLICATION: strSource = "Application";
		case GL_DEBUG_SOURCE_OTHER: strSource = "Other";
	}

	

	std::cerr << "[DEBUG] type = " << strType
		<< "\n\t source = " << strSource
		<< "\n\t severity = " << strSeverity
		<< "\n\t message = " << message
		<< "\n\n";
	//fprintf(stderr, " % s \n\tsource = 0x \n\ttype = 0x % x \n\tseverity = 0x % x \n\tmessage = % s\n\n",
	//				strError, source, type, severity, message);
}

/*
* Renders the given mesh by binding the vao and making the draw call.
* Is also responsible for checking if the mesh has been buffered yet.
* @pre the mesh must have been buffered earlier with buffer_mesh()
*/
void ruya::render::Renderer::draw_mesh(MeshID mesh_id, Vault& vault)
{
	MeshGPU mesh_handle = resolve_mesh(mesh_id, vault, gpu_vault);
	glBindVertexArray(mesh_handle.vao);
	glDrawElements(GL_TRIANGLES, mesh_handle.index_count, GL_UNSIGNED_INT, 0);
	assert(glGetError() == GL_NO_ERROR);
}





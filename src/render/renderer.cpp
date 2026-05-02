#include <list>
#include <iostream>

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
	auto lights_view = scene.registry.view<LightBasic>();
	auto light_entity = lights_view.front();
	if (light_entity == entt::null)
	{
		return;
	}
	LightBasic& light = lights_view.get<LightBasic>(light_entity);

	auto model_view = scene.registry.view<Model>(entt::exclude<LightBasic>);
	for (auto [entity, model] : model_view.each())
	{
		render_model(model, VP, light, activeObjectShader, vault);
	}

	// LIGHT SOURCES
	mShaderLights->use();
	auto lights_with_model_view = scene.registry.view<LightBasic, Model>();
	lights_with_model_view.each([&](auto entity, LightBasic& light, Model& model) {
		render_light_source(light, VP, vault, model);
	});
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
	const LightBasic& light,
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
	const LightBasic& light,
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
	activeShader->set_vec3("light.ambient", light.ambient);
	activeShader->set_vec3("light.diffuse", light.diffuse);
	activeShader->set_vec3("light.specular", light.specular);
	activeShader->set_vec3("light.position", light.position);
	
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
	LightBasic& light,
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



/************************************************************************************************
*
*	CLASS TextureSlotManager
*	 
************************************************************************************************/
ruya::render::TextureSlotManager::TextureSlotManager()
{
	// init slot maps and priority list
	int fragShaderMaxSlots = get_max_texture_units();
	for (int i = GL_TEXTURE0; i < GL_TEXTURE0 + fragShaderMaxSlots; i++)
	{
		// init all slots to 0 (= contains no texture)
		mSlotTextureMap[i] = 0; 
		// init priorities in numerical order
		list<GLuint>::iterator slotIter = mSlotPriority.insert(mSlotPriority.end(), i); 
		// init slot iterators
		mSlotPriorityRefMap[i] = slotIter;
	}
}

/*
* Binds given texture to one of the slots and returns the slot number so that the
* caller set the uniform location of the texture sampler to the correct slot number.
* 
* @pre: the given texture must have already been created with glTexImage2D()
*		(which is done by default by the Texture class' constructor)
*/
GLuint ruya::render::TextureSlotManager::bind_texture(const ruya::render::Texture& texture)
{
	// Check whether the texture is already bound
	if (mTextureSlotMap[texture.id] == 0)
	{
		// free new slot and set map values to new texture id
		GLuint newSlot = free_slot();
		mSlotTextureMap[newSlot] = texture.id;
		mTextureSlotMap[texture.id] = newSlot;

		// make new slot top priority
		set_top_priority(mSlotPriorityRefMap[newSlot]);

		// bind texture to new slot
		glActiveTexture(newSlot);
		glBindTexture(GL_TEXTURE_2D, texture.id);
	}
	else
	{
		// texture is already bound to a slot, increment slot priority
		GLuint textureSlot = mTextureSlotMap[texture.id];
		increment_priority(mSlotPriorityRefMap[textureSlot]);
	}

	// return slot number the texture has been (or was already) bound to
	return mTextureSlotMap[texture.id];
}

/*
* Frees a texture slot to be used by a new Texture.
*	Is responsible for internal state changes of TextureSlotManager() when freeing a slot
* @returns slot number that has been freed.
*/
GLuint ruya::render::TextureSlotManager::free_slot()
{
	// get number of least priority slot
	GLuint slot = mSlotPriority.back();

	// unregister texture residing in that slot
	GLuint oldTextureId = mSlotTextureMap[slot];
	mTextureSlotMap[oldTextureId] = 0; // tex id might be 0 (invalid) but does no harm
	
	// mark texture slot as free by setting its mapped texture value to 0
	mSlotTextureMap[slot] = 0;
	glDeleteTextures(1, &slot);

	// return slot num
	return slot;
}

/*
* Moves given slot up one position in the priority list.
* (this is done when an already-bound texture is being rendered again)
*/
void ruya::render::TextureSlotManager::increment_priority(list<GLuint>::iterator& slotIt)
{
	// can't increment position if it's the first in the list
	if (slotIt == mSlotPriority.begin())
		return;

	// swap values of elements at pos "slotIt" and the one before
	GLuint temp1 = *slotIt;
	slotIt--;
	GLuint temp2 = *slotIt;
	*slotIt = temp1;
	slotIt++;
	*slotIt = temp2;
}

/*
* Makes given slot from the mSlotPriority list top priority by placing it at the front of the list.
* @post: slotIt has been updated but is still pointing to the same slot (that is now at the front of the priority list)
*/
void ruya::render::TextureSlotManager::set_top_priority(list<GLuint>::iterator& slotIt)
{
	// save slot number and remove slot from priority list
	GLuint slot = *slotIt;
	mSlotPriority.erase(slotIt);

	// reinsert slot to the front
	mSlotPriority.push_front(slot);

	// update slotIt iterator since it was invalidated with the erase operation
	slotIt = mSlotPriority.begin();
}





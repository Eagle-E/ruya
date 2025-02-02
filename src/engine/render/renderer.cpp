#include <list>
#include <iostream>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "glm/gtx/string_cast.hpp"

#include "engine/render/renderer.h"
#include "engine/scene/texture.h"


using std::list;
using glm::mat4;	using glm::mat3;

ruya::Renderer::Renderer(Shader* shaderObjects, Shader* shaderLights, Window* window, Camera* camera)
	: mWindow(window), mCamera(camera), mSmoothShaderObjects(shaderObjects), mShaderLights(shaderLights),
	  mFlatShaderObjects(nullptr), mShadingMode(ShadingMode::SMOOTH),
	INDEX_VERTEX_ATTRIB(0),
	INDEX_NORMAL_ATTRIB(1),
	INDEX_TEXTURE_ATTRIB(2)
{
	// enable depth test
	glEnable(GL_DEPTH_TEST);

	// During init, enable debug output
	glEnable(GL_DEBUG_OUTPUT);
	glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
	glDebugMessageCallback(debug_mesage_callback, 0);
}

void ruya::Renderer::render_scene(Scene& scene)
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

	// get view-projection matrix
	mat4 projection = glm::perspective(glm::radians(mCamera->fov()), mWindow->aspect_ratio(), 0.1f, 300.0f);
	mat4 VP = projection * mCamera->view_matrix();

	// render scene objects
	list<Object*>& objects = scene.get_scene_objects();
	for (Object* obj : objects)
	{
		render_object(*obj, VP, **(scene.get_light_sources().begin()), activeObjectShader);
	}

	// LIGHT SOURCES
	mShaderLights->use();
	list<LightSource*> lights = scene.get_light_sources();
	for (LightSource* light : lights)
	{
		render_light_source(*light, VP);
	}
}

/*
* Handles the necessary OpenGL calls to render the object with the shader program
* that this Renderer has. If the Mesh of the Object is being rendered for the first
* time, the necessary buffers (VAO, VBO & EBO) will be created automatically.
* 
* @pre the correct shader program needs to be made current before calling this function.
*/
void ruya::Renderer::render_object(Object& obj, const mat4& viewProjectTransform, const LightSource& light, Shader* activeShader)
{	
	// Bind the textures and set their uniform location
	if (obj.texture())
	{
		GLuint textureSlot = mSlotManager.bind_texture(*obj.texture());
		activeShader->setInt("ourTexture", textureSlot - GL_TEXTURE0); // TODO: save texture uniform name in Shader class instead of hardcoding
	}

	std::pair<mat4, mat4> Model_ModelInv = obj.model_matrix_and_inverse();

	// pass uniform data
	activeShader->setVec3("objColor", obj.color());
	activeShader->setVec3("lightColor", light.color());

	mat4 inverseModelMat = glm::inverse(Model_ModelInv.first);
	vec4 lightPosInObjSpace = inverseModelMat * vec4(light.position(), 1.0f);
	vec4 cameraPosInObjSpace = inverseModelMat * vec4(mCamera->position(), 1.0f);
	activeShader->setVec3("lightPosInObjSpace", vec3(lightPosInObjSpace) / lightPosInObjSpace.w);
	activeShader->setVec3("cameraPosInObjSpace", vec3(cameraPosInObjSpace) / cameraPosInObjSpace.w);

	// material uniform
	Material& material = obj.material();
	activeShader->setVec3("material.ambient", material.ambient);
	activeShader->setVec3("material.diffuse", material.diffuse);
	activeShader->setVec3("material.specular", material.specular);
	activeShader->setFloat("material.shininess", material.shininess);

	// light uniform
	activeShader->setVec3("light.ambient", light.ambient());
	activeShader->setVec3("light.diffuse", light.diffuse());
	activeShader->setVec3("light.specular", light.specular());

	// calc model-view-projection matrix
	mat4 MVP = viewProjectTransform * Model_ModelInv.first;
	activeShader->setMatrix4D("MVP", MVP);

	// render mesh
	draw_mesh(obj.mesh());
}

void ruya::Renderer::render_light_source(LightSource& light, const mat4& viewProjectTransform)
{
	// color uniform
	mShaderLights->setVec3("objColor", light.model().color());

	// calc model-view-projection matrix
	mat4 MVP = viewProjectTransform * light.model().model_matrix();
	mShaderLights->setMatrix4D("MVP", MVP);

	// render mesh
	draw_mesh(light.model().mesh());
}



void ruya::Renderer::render_object(Object& obj)
{
	// TODO:	when a mesh gets deleted that is in the unordered_map, it needs to be removed.
	//			Consider attaching a custom destroctor on the shared_ptr that is being used as
	//			key, that will remove the 'null' shared_ptr.

	// Check if the Mesh of the object already had been buffered, if not create the buffers
	if (mMeshVaoMap.find(obj.mesh()) == mMeshVaoMap.end())
	{
		GLuint meshVaoID = buffer_mesh(*obj.mesh());
		mMeshVaoMap[obj.mesh()] = meshVaoID;
	}

	// Bind the textures and set their uniform location
	if (obj.texture())
	{
		GLuint textureSlot = mSlotManager.bind_texture(*obj.texture());
		mSmoothShaderObjects->setInt("ourTexture", textureSlot - GL_TEXTURE0); // TODO: save texture uniform name in Shader class instead of hardcoding
	}

	// pass the color
	mSmoothShaderObjects->setVec3("objColor", obj.color());

	// calc model-view-projection matrix
	mat4 projection = glm::perspective(glm::radians(mCamera->fov()), mWindow->aspect_ratio(), 0.1f, 300.0f);
	mat4 MVP = projection * mCamera->view_matrix() * obj.model_matrix();
	mSmoothShaderObjects->setMatrix4D("MVP", MVP);

	// Bind the vao and render
	draw_mesh(obj.mesh());
}

void GLAPIENTRY ruya::Renderer::debug_mesage_callback(GLenum source, GLenum type, GLuint id, GLenum severity, 
													  GLsizei length, const GLchar* message, const void* userParam)
{
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
void ruya::Renderer::draw_mesh(const shared_ptr<Mesh>& mesh)
{
	// TODO:	when a mesh gets deleted that is in the unordered_map, it needs to be removed.
	//			Consider attaching a custom destroctor on the shared_ptr that is being used as
	//			key, that will remove the 'null' shared_ptr.
	
	// Check if the Mesh of the object already had been buffered, if not create the buffers
	if (mMeshVaoMap.find(mesh) == mMeshVaoMap.end())
	{
		GLuint meshVaoID = buffer_mesh(*mesh);
		mMeshVaoMap[mesh] = meshVaoID;
	}
	int error = glGetError();


	size_t numIndexes = mesh->faces.size() * 3;
	glBindVertexArray(mMeshVaoMap[mesh]);
	glDrawElements(GL_TRIANGLES, numIndexes, GL_UNSIGNED_INT, 0);
	error = glGetError();
	int a = 0;
}

/*
* Does the necessary OpenGL calls the create VAO, VBO and EBO for the given mesh.
* @returns the VAO ID that contains info about the buffers containing the mesh's data.
*/
GLuint ruya::Renderer::buffer_mesh(const Mesh& mesh)
{
	// create a vertex buffer object (VAO) so we don't have to repeat VBO and vertex attribute stuff
	GLuint vaoID;
	glGenVertexArrays(1, &vaoID);
	glBindVertexArray(vaoID);

	// element buffer
	GLuint eboID;
	glGenBuffers(1, &eboID);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, eboID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh.size_faces(), mesh.faces.data(), GL_STATIC_DRAW);

	// vertex buffer, concatenate mesh data: first vertex data then texture data, ...
	GLuint vboID;
	long int sizeVert = mesh.size_vertices();
	long int sizeNormals = mesh.size_normals();
	long int sizeTex = mesh.size_texture_coords();
	long int meshSize = sizeVert + sizeNormals + sizeTex;
	
	glGenBuffers(1, &vboID); // create a buffer
	glBindBuffer(GL_ARRAY_BUFFER, vboID); // set buffer's type to array buffer
	glBufferData(GL_ARRAY_BUFFER, meshSize, NULL, GL_STATIC_DRAW); // allocate memory and copy vertices to GPU
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeVert, mesh.vertices.data());
	glBufferSubData(GL_ARRAY_BUFFER, sizeVert, sizeNormals, mesh.normals.data());
	glBufferSubData(GL_ARRAY_BUFFER, sizeVert + sizeNormals, sizeTex, mesh.textureCoordinates.data());
	

	// specify vertex attributes, how the data in the VBO should be evaluated
	// UPDATED according to the concatenated data format
	glVertexAttribPointer(INDEX_VERTEX_ATTRIB, 3, GL_FLOAT, GL_FALSE, 0, (void*)0); // loc data
	glEnableVertexAttribArray(INDEX_VERTEX_ATTRIB);
	glVertexAttribPointer(INDEX_NORMAL_ATTRIB, 3, GL_FLOAT, GL_FALSE, 0, (void*)(sizeVert)); // normal data
	glEnableVertexAttribArray(INDEX_NORMAL_ATTRIB);
	glVertexAttribPointer(INDEX_TEXTURE_ATTRIB, 2, GL_FLOAT, GL_FALSE, 0, (void*)(sizeVert + sizeNormals)); // texture data
	glEnableVertexAttribArray(INDEX_TEXTURE_ATTRIB);

	// unbind vertex array buffer
	glBindVertexArray(0);
	return vaoID;
}


/************************************************************************************************
*
*	CLASS TextureSlotManager
*	 
************************************************************************************************/
ruya::Renderer::TextureSlotManager::TextureSlotManager()
{
	// init slot maps and priority list
	int fragShaderMaxSlots = Texture::get_num_texture_slots_fragment_shader();
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
GLuint ruya::Renderer::TextureSlotManager::bind_texture(const ruya::Texture& texture)
{
	// Check whether the texture is already bound
	if (mTextureSlotMap[texture.ID()] == 0)
	{
		// free new slot and set map values to new texture id
		GLuint newSlot = free_slot();
		mSlotTextureMap[newSlot] = texture.ID();
		mTextureSlotMap[texture.ID()] = newSlot;

		// make new slot top priority
		set_top_priority(mSlotPriorityRefMap[newSlot]);

		// bind texture to new slot
		glActiveTexture(newSlot);
		glBindTexture(GL_TEXTURE_2D, texture.ID());
	}
	else
	{
		// texture is already bound to a slot, increment slot priority
		GLuint textureSlot = mTextureSlotMap[texture.ID()];
		increment_priority(mSlotPriorityRefMap[textureSlot]);
	}

	// return slot number the texture has been (or was already) bound to
	return mTextureSlotMap[texture.ID()];
}

/*
* Frees a texture slot to be used by a new Texture.
*	Is responsible for internal state changes of TextureSlotManager() when freeing a slot
* @returns slot number that has been freed.
*/
GLuint ruya::Renderer::TextureSlotManager::free_slot()
{
	// get number of least priority slot
	GLuint slot = mSlotPriority.back();

	// unregister texture residing in that slot
	GLuint oldTextureId = mSlotTextureMap[slot];
	mTextureSlotMap[oldTextureId] = 0; // tex id might be 0 (invalid) but does no harm

	// mark texture slot as free by setting its mapped texture value to 0
	mSlotTextureMap[slot] = 0;

	// return slot num
	return slot;
}

/*
* Moves given slot up one position in the priority list.
* (this is done when an already-binded texture is being rendered again)
*/
void ruya::Renderer::TextureSlotManager::increment_priority(list<GLuint>::iterator& slotIt)
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
void ruya::Renderer::TextureSlotManager::set_top_priority(list<GLuint>::iterator& slotIt)
{
	// save slot number and remove slot from priority list
	GLuint slot = *slotIt;
	mSlotPriority.erase(slotIt);

	// reinsert slot to the front
	mSlotPriority.push_front(slot);

	// update slotIt iterator since it was invalidated with the erase operation
	slotIt = mSlotPriority.begin();
}





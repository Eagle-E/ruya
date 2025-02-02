#ifndef RENDERER_H
#define RENDERER_H

#include <unordered_map>
#include <list>
#include <memory>
#include <glm/gtc/matrix_transform.hpp>
#include <glad/glad.h>

#include "engine/scene/scene.h"
#include "engine/scene/mesh.h"
#include "engine/render/shader.h"
#include "engine/core/window.h"
#include "engine/scene/camera.h"

using std::unordered_map;
using std::list;
using std::shared_ptr;
using glm::mat4;
using ruya::Shader;
using ruya::Window;
using ruya::Camera;

class ruya::Texture;


namespace ruya
{
	/*
	* A basic temporary renderer which will render a given scene
	*/
	class Renderer
	{
	private:
		/*
		* Manages which texture slots to use next for object textures.
		*
		* This is done with a priority list where the slots at the front have high priority
		* and the ones at the back low priority. The goal is to put slots that are frequently
		* and/or recently used at the front and the ones that haven't been used for a while at
		* the back by moving up the position of a slot each time it is used.
		*
		* When a texture has to be rendered that is not yet placed in a slot, it will get the
		* slot at the back of the priority list. That slot will be placed at the front of the
		* priority list making it highest priority. This is necessary because of a "low-priority
		* lock" see explanation comment lock below.
		*/
		class TextureSlotManager
		{
		public:
			TextureSlotManager();
			GLuint bind_texture(const ruya::Texture & texture);

		private:
			GLuint free_slot();
			void increment_priority(list<GLuint>::iterator& slotIt);
			void set_top_priority(list<GLuint>::iterator& slotIt);

			unordered_map<GLuint, GLuint> mTextureSlotMap; // mapping texture id to texture slot, not loaded if slot = 0
			unordered_map<GLuint, GLuint> mSlotTextureMap; // mapping slots to textures
			list<GLuint> mSlotPriority; // last item in map is the next one to use (and free up for new textures if necessary)
			unordered_map<GLuint, list<GLuint>::iterator> mSlotPriorityRefMap; // for each slot, contains iterator pointing to its location in the priority list.
		};

	public:
		enum class ShadingMode { SMOOTH, FLAT };

		Renderer(Shader* shaderObjects, Shader* shaderLights, Window* window, Camera* camera);
		void render_scene(Scene& scene);
		void render_object(Object& obj);
		void set_flat_shader(Shader* flatShader) { mFlatShaderObjects = flatShader; }
		void set_shading_mode(ShadingMode mode) { mShadingMode = mode; }
		ShadingMode shading_mode() const { return mShadingMode; }

	private:
		static void GLAPIENTRY debug_mesage_callback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, 
														const GLchar* message, const void* userParam);

		void render_object(Object& obj, const mat4& viewProjectTransform, const LightSource& light, Shader * activeShader);
		void render_light_source(LightSource& light, const mat4& viewProjectTransform);
		void draw_mesh(const shared_ptr<Mesh>& mesh);

		GLuint buffer_mesh(const Mesh& mesh);
		Shader* mSmoothShaderObjects;
		Shader* mShaderLights;
		Shader* mFlatShaderObjects;
		Window* mWindow;
		Camera* mCamera;
		ShadingMode mShadingMode;

		unordered_map<shared_ptr<Mesh>, GLuint> mMeshVaoMap;
		const GLuint INDEX_VERTEX_ATTRIB; // indexes of the attributes used in the vertex shader
		const GLuint INDEX_NORMAL_ATTRIB;
		const GLuint INDEX_TEXTURE_ATTRIB;
		TextureSlotManager mSlotManager;

		bool test = true;
	};
}

#endif


// LOW_PRIORITY LOCK PROBLEM DESCRIPTION
/*
*		Suppose you want to render a million pairs of objects. You render the first obj
*		of the pair then the second. The texture of the first one will get the last slot
*		then that slot will be pushed up by 1. Then, the second texture will get the current
*		last slot and will be pushed up 1 position, above the first one. If the two textures
*		get used again and again in the same order they will keep pushing each other down,
*		neither of the two every reaching the top of the priority list as they should. This
*		can be visualized as follows, 'X': texture 1, 'O': texture 2, '.': other textures.
*
*		.  .  .  .  .  .  .  .  .  .  .
*		.  .  .  .  .  .  .  .  .  .  .   ...
*		.  O  X  O  X  O  X  O  X  O  X
*		.  .  O  X  O  X  O  X  O  X  O
*
*		Now suppose now and then some random object gets rendered while the million pair-
*		objects are being rendered. The texture of the random object will get the slot at
*		the end of the priority queue so one of the textures of the pair objects that were
*		racing each other will be removed from its slot, and the random texture will be
*		placed in it, also moving its position up by one. Then when rendering the pairs
*		again, the texture that was kicked out will replace the texture of the other pair,
*		and move one slot up, above the random texture. The random texture will be kicked
*		out by the texture that was just kicked out and move one slot up, above the pair
*		texture that had just moved up, and they will race each other again.
*		This beats the purpose of the priority queue and makes it worse than just selecting
*		random texture slots in the first place.
*
*		One might think that moving a used slot up 2 positions will solve the problem, which
*		is true if there are only 2 textures being rendered one after the other. When there
*		are 3 textures, then moving up 2 positions brings the same problem as before. And if
*		we were to move up 1 position with 3 textures, then each texture would push out the
*		texture that will be rendered right after itself, making the shader keep loading in
*		and out texture data from texture slots. 'T': texture 3
* 		OXT
*
*		.  .  .  .  .  .  .  .  .  .  .
*		.  .  .  .  .  .  .  .  .  .  .    ...
*		.  O  X  T  O  X  T  O  X  T  O
*		.  .  O  X  T  O  X  T  O  X  T
*		-------------------------------
*			     O  X  T  O  X  T  O  X    pushed out
*
*		Very long problem description for a simple solution that I came up while starting to
*		write the last paragraph: when a new texture is moved to the lowest priority slot,
*		move that slot to the beginning of the list........
*		.....and making sure that there isn't a giant sequence of 33 textures that will be
*		rendered back-to-back a million times. (33 because the max # of slots for my gpu is 32)
*/
#ifndef LIGHTSOURCE_H
#define LIGHTSOURCE_H

#include "engine/scene/object.h"
#include "utils/uuid.h"
#include <glm/glm.hpp>

using glm::vec3;

namespace ruya
{
	
	class LightSource
	{
	public:
		LightSource() : mColor(1.0f), mAmbient(1), mDiffuse(1), mSpecular(1) {}
		LightSource(const vec3& color) : mColor(color), mAmbient(1), mDiffuse(1), mSpecular(1) {}

		// GETTERS
		vec3 position() const	{ return mModel.position(); }
		vec3 color() const		{ return mColor; }
		vec3 ambient() const	{ return mAmbient; }
		vec3 diffuse() const	{ return mDiffuse; }
		vec3 specular() const	{ return mSpecular; }
		Object& model()			{ return mModel; }


		// SETTERS
		void set_color(const vec3& color)		{ mColor = color; }
		void set_position(const vec3& position) { mModel.set_position(position); }
		void set_ambient(const vec3& ambient)   { mAmbient = ambient; }
		void set_diffuse(const vec3& diffuse)   { mDiffuse = diffuse; }
		void set_specular(const vec3& specular) { mSpecular = specular; }

	private:
		vec3 mColor;
		vec3 mAmbient;
		vec3 mDiffuse;
		vec3 mSpecular;
		Object mModel;
		UUID mUUID;
	};


}


#endif
#ifndef _PRIMITIVE_H_
#define _PRIMITIVE_H_

#include "glmath.h"
#include "Color.h"

namespace Cronos {

	enum PrimitiveTypes
	{
		Null,
		Primitive_Plane
	};

	class Primitive
	{
	public:

		Primitive();

		virtual void	Render() const;
		virtual void	InnerRender() const;
		void			SetPos(float x, float y, float z);
		void			SetRotation(float angle, const vec3 &u);
		void			Scale(float x, float y, float z);
		PrimitiveTypes	GetType() const;

	public:

		Color color;
		mat4x4 transform;
		bool axis, wire;

	protected:
		PrimitiveTypes type;
	};

	// ============================================
	class Plane : public Primitive
	{
	public:
		Plane();
		Plane(float x, float y, float z, float d);
		void InnerRender() const;
	public:
		vec3 normal;
		float constant;
	};

}

#endif
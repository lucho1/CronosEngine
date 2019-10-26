#ifndef _PRIMITIVECOMPONENT_H_
#define _PRIMITIVECOMPONENT_H_

#include "GameObject/GameObject.h"
#include <glm/glm.hpp>

#define DEFAULT_RADIUS 0.6f
#define DEFAULT_SLICE_AND_STACK 32
#define DEFAULT_SUBDIVISIONS 3

namespace Cronos {

	typedef struct par_shapes_mesh_s;
	enum class PrimitiveType {
								NONE = -1, CUBE, TETRAHEDRON, OCTAHEDRON, DODECAHEDRON, ICOSAHEDRON,			//Very Simple primitives (for par shapes)
								EMPTY_CYLINDER, CLOSED_CYLINDER, EMPTY_CONE, CLOSED_CONE, SPHERE, PLANE,		//Simple primitives for par shapes (slices, stacks)
								SEMI_SPHERE, TORUS, TREFOIL_KNOT, KLEIN_BOTTLE,
								DISK, ROCK, SUBDIVIDED_SPHERE, EMPTY											//More complex primitives (with seeds and divisions)
	};


	class PrimitiveGameObject : public GameObject
	{
	public:

		PrimitiveGameObject(PrimitiveType primitve_type, glm::vec3 size = { 2, 2, 2 }, float radius = DEFAULT_RADIUS,
			int figure_slices = DEFAULT_SLICE_AND_STACK, int figure_stacks = DEFAULT_SLICE_AND_STACK);

		~PrimitiveGameObject() {}

		void CreateRock(glm::vec3 size = { 2, 2, 2 }, int seed = 1, uint nSubdivisions = DEFAULT_SUBDIVISIONS); //To create a Rock, create an Empty primitive and then call this function
		void CreateSubdividedSphere(glm::vec3 size = { 2, 2, 2 }, uint nSubdivisions = DEFAULT_SUBDIVISIONS); //To create a Subd. Sphere, create an Empty primitive and then call this function
		void CreateDisk(glm::vec3 center = { 0, 0, 0 }, glm::vec3 size = { 2, 2, 2 }, float radius = DEFAULT_RADIUS, int figure_slices = DEFAULT_SLICE_AND_STACK); //To create a Disk, create an Empty primitive and then call this function

	private:

		void ParShapeToPrimitive(glm::vec3 size);
		void CreateCylinder(glm::vec3 size, int figure_slices, int figure_stacks);
		void CreateCone(glm::vec3 size, int figure_slices, int figure_stacks);

		PrimitiveType m_PrimitiveType = PrimitiveType::NONE;
		const inline PrimitiveType GetPrimitiveType() const { return m_PrimitiveType; }
	};
}

#endif
#ifndef _CRONOSPRIMITIVE_H_
#define _CRONOSPRIMITIVE_H_

#include "Model.h"

#define DEFAULT_RADIUS 0.5f
#define DEFAULT_SLICE_AND_STACK 32

namespace Cronos {

	typedef struct par_shapes_mesh_s;
	enum class PrimitiveType	{	NONE = -1, CUBE, TETRAHEDRON, OCTAHEDRON, DODECAHEDRON, ICOSAHEDRON,	//Very Simple primitives (for par shapes)
									CYLINDER, CONE, SPHERE, SEMI_SPHERE, PLANE, KLEIN_BOTTLE				//Simple primitives for par shapes (slices, stacks)
								};				

	class CronosPrimitiveMesh : public CronosMesh
	{
		friend class CronosPrimitive;
	public:

		virtual void Draw() override;
		const std::vector<unsigned short> GetIndexVector() const { return m_PARSHAPES_IndicesVector; }

	private:

		CronosPrimitiveMesh(std::vector<CronosVertex>vertices, std::vector<uint>indices, std::vector<CronosTexture>textures, std::vector<unsigned short>usIndices);
		virtual void SetupMesh() override;

		std::vector<unsigned short> m_PARSHAPES_IndicesVector;
	};


	class CronosPrimitive : public CronosModel
	{
		friend class CronosModel;
	public:

		CronosPrimitive(PrimitiveType primitve_type, glm::vec3 size = { 2, 2, 2 },
			int figure_slices = DEFAULT_SLICE_AND_STACK, int figure_stacks = DEFAULT_SLICE_AND_STACK);

		~CronosPrimitive();

	private:

		void ParShapeToPrimitive(PrimitiveType primitve_type, glm::vec3 size, int figure_slices, int figure_stacks);
		CronosPrimitiveMesh* Primitive_Mesh;
	};



}
#endif
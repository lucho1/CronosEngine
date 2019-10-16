#ifndef _CRONOSPRIMITIVE_H_
#define _CRONOSPRIMITIVE_H_

#include "Model.h"

namespace Cronos {

	typedef struct par_shapes_mesh_s;
	enum class PrimitiveType { NONE = -1, CUBE = 0 };

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

		CronosPrimitive(PrimitiveType primitve_type);
		~CronosPrimitive();

	private:

		void ParShapeToPrimitive(PrimitiveType primitve_type);
		CronosPrimitiveMesh* Primitive_Mesh;
	};



}
#endif
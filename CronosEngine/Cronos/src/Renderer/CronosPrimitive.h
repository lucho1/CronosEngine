#ifndef _CRONOSPRIMITIVE_H_
#define _CRONOSPRIMITIVE_H_

#include "Model.h"
#include "par_shapes/par_shapes.h"

namespace Cronos {

	class CronosPrimitive : public CronosModel
	{
	public:

		CronosPrimitive(par_shapes_mesh_s ParshapeMesh) : CronosModel(this), Parshape_Mesh(ParshapeMesh) {}
		~CronosPrimitive() {}

	private:

		void ParShapeToPrimitive();

		par_shapes_mesh_s Parshape_Mesh;

	};



}
#endif
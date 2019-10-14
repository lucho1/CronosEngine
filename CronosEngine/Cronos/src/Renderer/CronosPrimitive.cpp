#include "Providers/cnpch.h"
#include "CronosPrimitive.h"

namespace Cronos
{
	void CronosPrimitive::ParShapeToPrimitive()
	{
		
		std::vector<glm::vec3> positionVector;


		float* vertices = Parshape_Mesh.points;
		float* normals = Parshape_Mesh.normals;
		float* text_coords = Parshape_Mesh.tcoords;

		uint j = 0;
		for (uint i = 0; i < Parshape_Mesh.npoints; i++)
		{
			float x = Parshape_Mesh.points[j];
			float y = Parshape_Mesh.points[++j];
			float z = Parshape_Mesh.points[++j];

		}





		unsigned short* PrSh_Indices = Parshape_Mesh.triangles;
		const uint IndicesSize = sizeof(PrSh_Indices) / sizeof(PrSh_Indices[0]);

		uint Indices_to_CronosIndices[IndicesSize];
		for (uint i = 0; i < IndicesSize; i++)
			Indices_to_CronosIndices[i] = PrSh_Indices[i];

		std::vector<uint> tmpIndicesVector;
		for (uint i = 0; i < IndicesSize; i++)
			tmpIndicesVector.push_back(Indices_to_CronosIndices[i]);




		


		
		CronosMesh tmpMesh(vertex, tmpIndicesVector, textures);
	}
}



/*
	//ushort array --> uint array

	//Setting ushort array
	unsigned short ushortArray[5] = { 0, 1, 2, 3, 4 };
	const uint size = sizeof(ushortArray) / sizeof(ushortArray[0]);

	//Setting uint array
	uint uintArray[size];
	for (uint i = 0; i < size; i++)
		uintArray[i] = ushortArray[i];
*/
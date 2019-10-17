#include "Providers/cnpch.h"
#include "CronosPrimitive.h"

#include "par_shapes/par_shapes.h"


namespace Cronos
{
	// --------------------------------- PRIMITIVE MODEL ---------------------------------
	CronosPrimitive::CronosPrimitive(PrimitiveType primitve_type, glm::vec3 size, int figure_slices, int figure_stacks) : CronosModel(this)
	{
		if (primitve_type == PrimitiveType::NONE)
		{
			LOG("Invalid Primitive Type!! Couldn't be created!");
			return;
		}
		else
		{
			ParShapeToPrimitive(primitve_type, size, figure_slices, figure_stacks);
			this->CalculateModelAxis();
		}
	}

	CronosPrimitive::~CronosPrimitive()
	{
		std::vector<CronosMesh*>::iterator item = m_ModelMeshesVector.begin();
		for (; item != m_ModelMeshesVector.end(); item++)
		{
			(*item)->~CronosMesh();
			m_ModelMeshesVector.erase(item);
		}

		m_ModelMeshesVector.clear();

		if (m_PrimitiveMesh != nullptr)
		{
			delete m_PrimitiveMesh;
			m_PrimitiveMesh = nullptr;
		}
	}
	
	void CronosPrimitive::ParShapeToPrimitive(PrimitiveType primitve_type, glm::vec3 size, int figure_slices, int figure_stacks)
	{
		par_shapes_mesh ParshapeMesh;
		switch (primitve_type)
		{
		//Very Simple primitives (for par shapes)
			case PrimitiveType::CUBE:
					ParshapeMesh = *par_shapes_create_cube();
					break;
			case PrimitiveType::TETRAHEDRON:
				ParshapeMesh = *par_shapes_create_tetrahedron();
				break;
			case PrimitiveType::OCTAHEDRON:
				ParshapeMesh = *par_shapes_create_octahedron();
				break;
			case PrimitiveType::DODECAHEDRON:
				ParshapeMesh = *par_shapes_create_dodecahedron();
				break;
			case PrimitiveType::ICOSAHEDRON:
				ParshapeMesh = *par_shapes_create_icosahedron();
				break;
		//Simple primitives for par shapes (slices, stacks)
			case PrimitiveType::CYLINDER:
				ParshapeMesh = *par_shapes_create_cylinder(figure_slices, figure_stacks);
				break;
			case PrimitiveType::CONE:
				ParshapeMesh = *par_shapes_create_cone(figure_slices, figure_stacks);
				break;
			case PrimitiveType::SPHERE:
				ParshapeMesh = *par_shapes_create_parametric_sphere(figure_slices, figure_stacks);
				break;
			case PrimitiveType::SEMI_SPHERE:
				ParshapeMesh = *par_shapes_create_hemisphere(figure_slices, figure_stacks);
				break;
			case PrimitiveType::PLANE:
				ParshapeMesh = *par_shapes_create_plane(figure_slices, figure_stacks);
				break;
			case PrimitiveType::KLEIN_BOTTLE:
				ParshapeMesh = *par_shapes_create_klein_bottle(figure_slices, figure_stacks);
				break;
			case PrimitiveType::NONE:
				LOG("Invalid Primitive Type!!"); par_shapes_free_mesh(&ParshapeMesh); return;
			default:
				LOG("Invalid Primitive Type!!"); par_shapes_free_mesh(&ParshapeMesh); return;
		}

		par_shapes_translate(&ParshapeMesh, 0, 0, 0);
		par_shapes_scale(&ParshapeMesh, size.x, size.y, size.z);
		std::vector<CronosVertex>tmpVertexVector;

		uint j = 0;
		for (uint i = 0; i < ParshapeMesh.npoints; i++)
		{
			CronosVertex tmpVertex;

			float x = ParshapeMesh.points[j];
			float y = ParshapeMesh.points[j + 1];
			float z = ParshapeMesh.points[j + 2];

			tmpVertex.Position = glm::vec3(x, y, z);
			
			if (ParshapeMesh.normals != nullptr) {

				x = ParshapeMesh.normals[j];
				y = ParshapeMesh.normals[j + 1];
				z = ParshapeMesh.normals[j + 2];

				tmpVertex.Normal = glm::vec3(x, y, z);
			}

			//if (ParshapeMesh.tcoords != nullptr)
			//{
			//	x = ParshapeMesh.tcoords[j];
			//	y = ParshapeMesh.tcoords[j + 1];
			//
			//	tmpVertex.TexCoords = glm::vec2(x, y);
			//}

			tmpVertexVector.push_back(tmpVertex);
			j += 3;
		}

		std::vector<uint> tmpIndicesVector;
		tmpIndicesVector.push_back(0);

		std::vector<unsigned short> tmpUSIndicesVector;
		for (uint i = 0; i < (ParshapeMesh.ntriangles * 3); i++)
			tmpUSIndicesVector.push_back(ParshapeMesh.triangles[i]);

		std::vector<uint> uintVec(tmpUSIndicesVector.size(), 0);
		for (uint i = 0; i < tmpUSIndicesVector.size(); i++)
			uintVec[i] = tmpUSIndicesVector[i];

		std::vector<CronosTexture>tmp_TextureVector;
		CronosTexture defT = { 0, "TEXTURENONE" };
		tmp_TextureVector.push_back(defT);

		m_PrimitiveMesh = new CronosMesh(tmpVertexVector, uintVec, tmp_TextureVector);
		this->m_ModelMeshesVector.push_back(m_PrimitiveMesh);
	}
}
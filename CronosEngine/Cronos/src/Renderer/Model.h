#ifndef _MODEL_H_
#define _MODEL_H_

#include "glm/glm.hpp"
#include "VertexArray.h"

namespace Cronos {


	struct Vertex
	{
		glm::vec3 Position;
		glm::vec3 Normal;
		glm::vec2 TexCoords;
	};


	class Mesh
	{

	public:
		
		Mesh(std::vector<Vertex> vertices, std::vector<uint>indices);
		Mesh();
		~Mesh();

		void Draw();

	private:

		void SetupMesh();

		std::vector<Vertex> m_VertexVector;
		std::vector<uint> m_IndicesVector;

		VertexArray* m_MeshVAO;
		VertexBuffer* m_MeshVBO;
		IndexBuffer* m_MeshIBO;

	};
}
#endif
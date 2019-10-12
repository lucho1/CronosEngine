#ifndef _MODEL_H_
#define _MODEL_H_

#include "glm/glm.hpp"
#include "VertexArray.h"

#include <Assimp/include/cimport.h>
#include <Assimp/include/cfileio.h>
#include <Assimp/include/Importer.hpp>
#include <Assimp/include/scene.h>
#include <Assimp/include/postprocess.h>

namespace Cronos {

	struct Vertex
	{
		glm::vec3 Position;
		glm::vec3 Normal;
		glm::vec2 TexCoords;
	};

	struct Texture
	{
		uint m_TextureID;
		std::string m_TextureType;
	};

	class Mesh
	{
	public:
		
		Mesh(std::vector<Vertex> vertices, std::vector<uint>indices, std::vector<Texture>textures);
		Mesh();
		~Mesh();

		void Draw();

	private:

		void SetupMesh();

		std::vector<Texture> m_TexturesVector;
		std::vector<Vertex> m_VertexVector;
		std::vector<uint> m_IndicesVector;

		VertexArray* m_MeshVAO;
		VertexBuffer* m_MeshVBO;
		IndexBuffer* m_MeshIBO;
	};


	class Model
	{
	public:
	
		Model(const std::string& filepath);
		~Model();
		void Draw();
	
	private:
	
		std::vector<Mesh*> m_ModelMeshesVector;
		std::string m_ModelDirectoryPath;
	
		void LoadCronosModel(const std::string& filepath);
		void ProcessNode(aiNode* node, const aiScene* scene);
		Mesh* ProcessCronosMesh(aiMesh* mesh, const aiScene* scene);
	
	};
}
#endif
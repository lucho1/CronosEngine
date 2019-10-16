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

	class CronosPrimitive;
	class CronosPrimitiveMesh;

	struct CronosVertex
	{
		glm::vec3 Position;
		glm::vec3 Normal;
		glm::vec2 TexCoords;
	};

	struct CronosTexture
	{
		uint m_TextureID;
		std::string m_TextureType;
	};

	class CronosMesh
	{
		friend class CronosPrimitiveMesh;

	public:
		
		CronosMesh(std::vector<CronosVertex>vertices, std::vector<uint>indices, std::vector<CronosTexture>textures);
		CronosMesh(CronosPrimitiveMesh* primitive) {}
		~CronosMesh();

		virtual void Draw();
		void DrawVerticesNormals();
		void DrawPlanesNormals();

		void ScaleMesh(glm::vec3 Unitary_scaleAxis, float scaleMagnitude);
		void MoveMesh(glm::vec3 Unitary_moveAxis, float moveMagnitude);

		const std::vector<CronosTexture> GetTexturesVector() const { return m_TexturesVector; }
		const std::vector<CronosVertex> GetVertexVector() const { return m_VertexVector; }
		const std::vector<uint> GetIndexVector() const { return m_IndicesVector; }

	private:

		virtual void SetupMesh();

		std::vector<CronosTexture> m_TexturesVector;
		std::vector<CronosVertex> m_VertexVector;
		std::vector<uint> m_IndicesVector;

		VertexArray* m_MeshVAO;
		VertexBuffer* m_MeshVBO;
		IndexBuffer* m_MeshIBO;
	};


	class CronosModel
	{
		friend class AssimpCronosTranslator;
		friend class CronosPrimitive;

	public:
		
		CronosModel(const std::string& filepath);
		CronosModel(CronosPrimitive* primitive) {}
		~CronosModel();

		void Draw();
		void DrawVerticesNormals();
		void DrawPlanesNormals();

		void ScaleModel(glm::vec3 Unitary_scaleAxis, float scaleMagnitude);
		void MoveModel(glm::vec3 Unitary_moveAxis, float moveMagnitude);
		
		const glm::vec3 GetModelAxis() const { return m_ModelAxis; }

	private:
		
		void CalculateModelAxis();

		std::vector<CronosMesh*> m_ModelMeshesVector;
		std::string m_ModelDirectoryPath;
		glm::vec3 m_ModelAxis;
	};


	class AssimpCronosTranslator
	{
	private:
		friend class CronosModel;

		AssimpCronosTranslator(CronosModel* Cr_Model);
		
		void LoadModel(const std::string& filepath);
		void ProcessAssimpNode(aiNode* as_node, const aiScene* as_scene);
		CronosMesh* ProcessCronosMesh(aiMesh* as_mesh);

	private:
		CronosModel* m_CronosModel;
	};
}
#endif
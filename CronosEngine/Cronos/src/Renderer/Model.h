#ifndef _MODEL_H_
#define _MODEL_H_

#include "glm/glm.hpp"
#include "glm/gtc/quaternion.hpp"
#include "VertexArray.h"

#include <Assimp/include/cimport.h>
#include <Assimp/include/cfileio.h>
#include <Assimp/include/Importer.hpp>
#include <Assimp/include/scene.h>
#include <Assimp/include/postprocess.h>


namespace Cronos {

	class CronosPrimitive;

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
	public:
		
		CronosMesh(std::vector<CronosVertex>vertices, std::vector<uint>indices, std::vector<CronosTexture>textures);
		~CronosMesh();

		void Draw();
		void DrawVerticesNormals();
		void DrawPlanesNormals();

		void ScaleMesh(glm::vec3 ScaleMagnitude);
		void MoveMesh(glm::vec3 MoveAxis, float moveMagnitude);
		void RotateMesh(float RotDegrees, glm::vec3 RotAxis, glm::vec3 OwnAxis);

		const std::vector<CronosTexture> GetTexturesVector() const { return m_TexturesVector; }
		const std::vector<CronosVertex> GetVertexVector() const { return m_VertexVector; }
		const std::vector<uint> GetIndexVector() const { return m_IndicesVector; }
		
		const glm::mat4 GetTransformation() const { return m_Transformation; }

	private:

		void SetupMesh();
		void DecomposeTransformation();

		std::vector<CronosTexture> m_TexturesVector;
		std::vector<CronosVertex> m_VertexVector;
		std::vector<uint> m_IndicesVector;

		VertexArray* m_MeshVAO;
		VertexBuffer* m_MeshVBO;
		IndexBuffer* m_MeshIBO;


		glm::mat4 m_Transformation = glm::mat4(1.0f); // your transformation matrix.
		glm::vec3 m_Scale;
		glm::quat m_Rotation;
		glm::vec3 m_Translation;
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

		void ScaleModel(glm::vec3 ScaleMagnitude);
		void MoveModel(glm::vec3 MoveAxis, float moveMagnitude);
		void RotateModel(float RotDegrees, glm::vec3 RotAxis);
		
		const glm::vec3 GetModelAxis() const { return m_ModelAxis; }
		const glm::mat4 GetTransformation() const { return m_Transformation; }

	private:
		
		void CalculateModelAxis();

		std::vector<CronosMesh*> m_ModelMeshesVector;
		std::string m_ModelDirectoryPath;
		glm::vec3 m_ModelAxis;


		glm::mat4 m_Transformation = glm::mat4(1.0f);
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
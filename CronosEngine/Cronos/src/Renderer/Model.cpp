#include "Providers/cnpch.h"

#include "Model.h"
#include "Application.h"
#include "Modules/TextureManager.h"

#include "glm/gtx/transform.hpp"
#include <glm/gtx/matrix_decompose.hpp>

#include "mmgr/mmgr.h"

namespace Cronos {

	// ---------------------------------- CRONOS MESHES ----------------------------------
	CronosMesh::CronosMesh(std::vector<CronosVertex>vertices, std::vector<uint>indices, std::vector<Texture*>& textures)
	{
		m_VertexVector = vertices;
		m_IndicesVector = indices;
		m_TexturesVector = textures;

		SetupMesh();
	}

	CronosMesh::~CronosMesh()
	{
		if (m_MeshVAO != nullptr)
			m_MeshVAO->~VertexArray();
		if (m_MeshIBO != nullptr)
			m_MeshIBO->~IndexBuffer();
		if (m_MeshVBO != nullptr)
			m_MeshVBO->~VertexBuffer();
	}

	void CronosMesh::Draw(Shader* shader, bool bindShader)
	{
		m_MeshVAO->Bind();
		
		if (bindShader)
		{
			shader->Bind();
			shader->SetUniformMat4f("u_Proj", App->engineCamera->GetProjectionMatrixMAT4());
			shader->SetUniformMat4f("u_View", App->engineCamera->GetViewMatrixMAT4());
			shader->SetUniformMat4f("u_Model", GetTransformation());
		}

		std::vector< Texture*>::iterator item = m_TexturesVector.begin();
		for (; item != m_TexturesVector.end() && (*item) != NULL; item++)
		{
			(*item)->Bind((*item)->GetTextureID());

			if((*item)->GetTextureType() == TextureType::DIFFUSE)
				shader->SetUniform1i("u_DiffuseTexture", (*item)->GetTextureID());
			if ((*item)->GetTextureType() == TextureType::SPECULAR)
				shader->SetUniform1i("u_SpecularTexture", (*item)->GetTextureID());
			//else
			//	App->scene->BasicTestShader->SetUniform1i("u_Texture", (*item)->GetTextureID());
		}


		glDrawElements(GL_TRIANGLES, m_MeshVAO->GetIndexBuffer()->GetCount(), GL_UNSIGNED_INT, 0);
		for (; item != m_TexturesVector.end() && (*item) != NULL; item++)
			(*item)->Unbind();

		shader->Unbind();
	}

	void CronosMesh::DrawVerticesNormals()
	{
		float linelength = 0.000002f;
		glLineWidth(2.0f);
		glColor4f(0.1f, 0.5f, 0.8f, 1.0f);
		std::vector<CronosVertex>::iterator item = m_VertexVector.begin();
		for (; item != m_VertexVector.end(); item++)
		{

			//glm::vec3 norm = (*item).Normal + (*item).Position;
			glm::vec3 pos = (*item).Position;
			glm::vec3 norm = (*item).Position + (*item).Normal;

			glBegin(GL_LINES);
				glVertex3f(pos.x, pos.y, pos.z);
				glVertex3f(norm.x + linelength, norm.y, norm.z);
				glVertex3f(pos.x, pos.y, pos.z);
				glVertex3f(norm.x, norm.y + linelength, norm.z);
				glVertex3f(pos.x, pos.y, pos.z);
				glVertex3f(norm.x, norm.y, norm.z + linelength);
			glEnd();
		}
	}

	void CronosMesh::DrawPlanesNormals()
	{
		float linelength = 0.2f;
		glColor4f(0.6f, 0.4f, 0.2f, 1.0f);
		for (uint i = 0; i < m_IndicesVector.size(); i += 3)
		{
			glm::vec3 p1 = m_VertexVector[m_IndicesVector[i]].Position;
			glm::vec3 p2 = m_VertexVector[m_IndicesVector[i + 1]].Position;
			glm::vec3 p3 = m_VertexVector[m_IndicesVector[i + 2]].Position;

			glm::vec3 PlaneNormal = glm::cross(p2 - p1, p3 - p1);
			glm::normalize(PlaneNormal);
			PlaneNormal *= linelength;

			glm::vec3 TriCenter = { 0, 0, 0 };
			TriCenter.x = (p1.x + p2.x + p3.x) / 3;
			TriCenter.y = (p1.y + p2.y + p3.y) / 3;
			TriCenter.z = (p1.z + p2.z + p3.z) / 3;

			glBegin(GL_LINES);
				glColor4f(0.0f, 0.2f, 1.0f, 0.5f);
				glVertex3f(TriCenter.x, TriCenter.y, TriCenter.z);
				glVertex3f(TriCenter.x + PlaneNormal.x, TriCenter.y + PlaneNormal.y, TriCenter.z + PlaneNormal.z);
			glEnd();
		}
	}

	void CronosMesh::SetupMesh()
	{
		m_MeshVAO = new VertexArray();
		m_MeshVBO = new VertexBuffer(&m_VertexVector[0], m_VertexVector.size() * sizeof(CronosVertex));

		m_MeshVBO->SetLayout({
			{Cronos::VertexDataType::VEC3F, "a_Position"},
			{Cronos::VertexDataType::VEC3F, "a_Normal"},
			{Cronos::VertexDataType::VEC2F, "a_TexCoords"}
		});
		m_MeshVAO->AddVertexBuffer(*m_MeshVBO);

		m_MeshIBO = new IndexBuffer(&m_IndicesVector[0], m_IndicesVector.size());
		m_MeshVAO->AddIndexBuffer(*m_MeshIBO);
	}

	void CronosMesh::ScaleMesh(glm::vec3 ScaleMagnitude)
	{
		//glm::mat4 scale = glm::mat4(1.0f);
		//scale = glm::scale(ScaleMagnitude);
		//m_Transformation *= scale;
		//DecomposeTransformation();

		m_Transformation = glm::scale(m_Transformation, ScaleMagnitude);
		DecomposeTransformation();

		//ModelMatrix = glm::scale(ModelMatrix, Scale);
		/*glm::mat4 translation = glm::mat4(1.0f);
		translation = glm::scale(translation, ScaleMagnitude);

		std::vector<CronosVertex>::iterator item = m_VertexVector.begin();
		for (; item != m_VertexVector.end(); item++)
		{
			glm::vec4 pos = glm::vec4((*item).Position, 1.0f);
			(*item).Position = translation * pos;
		}*/
	}

	void CronosMesh::MoveMesh(glm::vec3 MoveAxis, float moveMagnitude)
	{
		//glm::mat4 translation = glm::mat4(1.0f);
		//translation = glm::translate(translation, MoveAxis);

		m_Transformation = glm::translate(m_Transformation, MoveAxis);

		//m_Transformation += translation;
		DecomposeTransformation();

		//MoveAxis *= moveMagnitude;
		//glm::mat4 translation = glm::mat4(1.0f);
		//translation = glm::translate(translation, MoveAxis);
		//
		//std::vector<CronosVertex>::iterator item = m_VertexVector.begin();
		//for (; item != m_VertexVector.end(); item++)
		//{
		//	glm::vec4 pos = glm::vec4((*item).Position, 1.0f);
		//	(*item).Position = translation * pos;
		//}

	}

	void CronosMesh::RotateMesh(float RotDegrees, glm::vec3 RotAxis, glm::vec3 OwnAxis)
	{
		glm::mat4 rot = glm::mat4(1.0f);
		rot = glm::rotate(rot, glm::radians(RotDegrees), RotAxis);
		m_Transformation *= rot;
		DecomposeTransformation();
	}

	void CronosMesh::DecomposeTransformation()
	{
		glm::vec3 skew;
		glm::vec4 perspective;
		glm::decompose(m_Transformation, m_Scale, m_Rotation, m_Translation, skew, perspective);
		m_Rotation = glm::conjugate(m_Rotation);
	}

	void CronosMesh::SetTextures(std::vector<Texture*>& newTexture, TextureType textureType)
	{
		std::vector<Texture*>::iterator item = m_TexturesVector.begin();
		for (; item != m_TexturesVector.end() && (*item) != NULL; item++)
		{
			if ((*item)->GetTextureType() == textureType)
			{
				(*item)->~Texture();
				delete (*item);
				m_TexturesVector.erase(item);
			}
		}

		m_TexturesVector.insert(m_TexturesVector.end(), newTexture.begin(), newTexture.end());
	}


	// ---------------------------------- CRONOS MODELS ----------------------------------
	CronosModel::CronosModel(const std::string& filepath)
	{
		AssimpCronosTranslator m_ACT(this);
		m_ACT.LoadModel(filepath);
		CalculateModelAxis();
	}

	CronosModel::~CronosModel()
	{
		std::vector<CronosMesh*>::iterator item = m_ModelMeshesVector.begin();
		for (; item != m_ModelMeshesVector.end(); item++)
		{
			(*item)->~CronosMesh();
			delete(*item);
			m_ModelMeshesVector.erase(item);
		}

		m_ModelMeshesVector.clear();
	}

	void CronosModel::Draw(Shader* shader, bool bindShader)
	{
		//Just iterate all model's meshes and draw them
		for (uint i = 0; i < m_ModelMeshesVector.size(); i++)
			m_ModelMeshesVector[i]->Draw(shader, bindShader);
	}

	void CronosModel::DrawVerticesNormals()
	{
		//Just iterate all model's meshes and draw them
		for (uint i = 0; i < m_ModelMeshesVector.size(); i++)
			m_ModelMeshesVector[i]->DrawVerticesNormals();
	}

	void CronosModel::DrawPlanesNormals()
	{
		//Just iterate all model's meshes and draw them
		for (uint i = 0; i < m_ModelMeshesVector.size(); i++)
			m_ModelMeshesVector[i]->DrawPlanesNormals();
	}


	//TODO: Change this, is not optimal!
	void CronosModel::ScaleModel(glm::vec3 ScaleMagnitude)
	{
		//Scale all Meshes
		for (uint i = 0; i < m_ModelMeshesVector.size(); i++)
		{
			m_ModelMeshesVector[i]->ScaleMesh(ScaleMagnitude);
			m_Transformation *= m_ModelMeshesVector[i]->GetTransformation();
		}

		CalculateModelAxis();
	}

	//TODO: Change this, is not optimal!
	void CronosModel::MoveModel(glm::vec3 MoveAxis, float moveMagnitude)
	{
		//Move all Meshes
		for (uint i = 0; i < m_ModelMeshesVector.size(); i++)
		{
			m_ModelMeshesVector[i]->MoveMesh(MoveAxis, moveMagnitude);
			m_Transformation *= m_ModelMeshesVector[i]->GetTransformation();
		}

		CalculateModelAxis();
	}

	void CronosModel::RotateModel(float RotDegrees, glm::vec3 RotAxis)
	{
		//Rotate all Meshes
		for (uint i = 0; i < m_ModelMeshesVector.size(); i++)
		{
			m_ModelMeshesVector[i]->RotateMesh(RotDegrees, RotAxis, glm::vec3(1.0f));
			m_Transformation *= m_ModelMeshesVector[i]->GetTransformation();
		}
		
		CalculateModelAxis();
	}

	//TODO: Change this, is not optimal!
	void CronosModel::CalculateModelAxis()
	{
		//glm::vec3 new_axis = (m_ModelMaxVertexPos - m_ModelMinVertexPos) / 2.0f;
		//m_ModelAxis = new_axis;
	}

	void CronosModel::DrawModelAxis()
	{
		glm::vec3 axis = GetModelAxis();
		float linelength = 1.0f;
		glLineWidth(2.0f);
		glBegin(GL_LINES);
			glColor4f(1.0f, 0.0f, 0.0f, 1.0f);
			glVertex3f(axis.x, axis.y, axis.z);
			glVertex3f(axis.x + linelength, axis.y, axis.z);
			glColor4f(0.0f, 1.0f, 0.0f, 1.0f);
			glVertex3f(axis.x, axis.y, axis.z);
			glVertex3f(axis.x, axis.y + linelength, axis.z);
			glColor4f(0.0f, 0.0f, 1.0f, 1.0f);
			glVertex3f(axis.x, axis.y, axis.z);
			glVertex3f(axis.x, axis.y, axis.z + linelength);
		glEnd();
	}


	// ---------------------------------- ASSIMP-CRONOS MODEL TRANSLATOR ----------------------------------
	AssimpCronosTranslator::AssimpCronosTranslator(CronosModel* Cr_Model)
	{
		// Stream log messages to Debug window
		struct aiLogStream stream;
		stream = aiGetPredefinedLogStream(aiDefaultLogStream_DEBUGGER, nullptr);
		aiAttachLogStream(&stream);

		m_CronosModel = Cr_Model;
	}

	void AssimpCronosTranslator::LoadModel(const std::string& filepath)
	{
		//Generate an Assimp Importer & Call ReadFile() to actually read the model file
		//aiProcess_Triangulate makes all the model's primitive shapes to be triangles if they aren't
		//aiProcess_FlipUVs flips the texture UV's Y axis (necessary)
		//Other useful option: _GenNormals (generates normals for vertices if they don't have),
		//_OptimizeMeshes (joins several meshes into 1 to reduce drawcalls)
		LOG("LOADING MODEL");
		Assimp::Importer importer;
		const aiScene* scene = importer.ReadFile(filepath, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcessPreset_TargetRealtime_MaxQuality);

		//Report an error if the model data is incomplete
		if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
		{
			std::cout << "Error in Assimp, " << importer.GetErrorString() << std::endl;
			return;
		}

		//This thing with directory is to get the model directory and not its path, so
		//the substr() gets, in this case, all the characters until the last '/' char.
		//So, if filepath is "AA/BB/model.fbx", this will be "AA/BB"
		m_CronosModel->m_ModelDirectoryPath = filepath.substr(0, filepath.find_last_of('/'));
		//If all is correct, we process all the nodes passing the first one (root)
		ProcessAssimpNode(scene->mRootNode, scene);

		// detach log stream
		aiDetachAllLogStreams();
	}

	void AssimpCronosTranslator::ProcessAssimpNode(aiNode* as_node, const aiScene* as_scene)
	{
		LOG("	Processing Model Mesh");
		//Process node's meshes if there are
		for (uint i = 0; i < as_node->mNumMeshes; i++)
		{
			//Get the mesh from the meshes list of the node in scene's meshes list
			aiMesh* as_mesh = as_scene->mMeshes[as_node->mMeshes[i]];
			m_CronosModel->m_ModelMeshesVector.push_back(ProcessCronosMesh(as_mesh, as_scene));
		}

		//Process all node's childrens
		for (uint i = 0; i < as_node->mNumChildren; i++)
			ProcessAssimpNode(as_node->mChildren[i], as_scene);
	}

	CronosMesh* AssimpCronosTranslator::ProcessCronosMesh(aiMesh* as_mesh, const aiScene* as_scene)
	{
		LOG("	Processing Model Mesh");
		std::vector<CronosVertex>tmp_VertexVector;
		std::vector<uint>tmp_IndicesVector;
		std::vector<Texture*>tmp_TextureVector;

		static float minX = as_mesh->mVertices[0].x, minY = as_mesh->mVertices[0].y, minZ = as_mesh->mVertices[0].x;
		static float maxX = minX, maxY = minY, maxZ = minZ;

		static glm::vec3 minPos = glm::vec3(as_mesh->mVertices[0].x, as_mesh->mVertices[0].y, as_mesh->mVertices[0].z);
		static glm::vec3 maxPos = minPos;

		//Process mesh vertices
		for (uint i = 0; i < as_mesh->mNumVertices; i++)
		{
			//First, position & normals
			CronosVertex tmpVertex;
			tmpVertex.Position = glm::vec3(as_mesh->mVertices[i].x, as_mesh->mVertices[i].y, as_mesh->mVertices[i].z);
			tmpVertex.Normal = glm::vec3(as_mesh->mNormals[i].x, as_mesh->mNormals[i].y, as_mesh->mNormals[i].z);

			//minPos = glm::vec3(MIN(minPos.x, tmpVertex.Position.x), MIN(minPos.y, tmpVertex.Position.y), MIN(minPos.z, tmpVertex.Position.z));
			//maxPos = glm::vec3(MAX(maxPos.x, tmpVertex.Position.x), MAX(maxPos.y, tmpVertex.Position.y), MAX(maxPos.z, tmpVertex.Position.z));

			minX = MIN(minX, tmpVertex.Position.x); minY = MIN(minY, tmpVertex.Position.y); minZ = MIN(minZ, tmpVertex.Position.z);
			maxX = MAX(maxX, tmpVertex.Position.x); maxY = MAX(maxY, tmpVertex.Position.y); maxZ = MAX(maxZ, tmpVertex.Position.z);

			//Then we see if there are text. coords in the first set [0] (OGL has until 8)
			if (as_mesh->mTextureCoords[0])
				tmpVertex.TexCoords = glm::vec2(as_mesh->mTextureCoords[0][i].x, as_mesh->mTextureCoords[0][i].y);
			else
				tmpVertex.TexCoords = glm::vec2(0.0f, 0.0f);

			tmp_VertexVector.push_back(tmpVertex);
		}

		//Process mesh's indices
		for (uint i = 0; i < as_mesh->mNumFaces; i++)
		{
			aiFace face = as_mesh->mFaces[i];
			for (uint j = 0; j < face.mNumIndices; j++)
				tmp_IndicesVector.push_back(face.mIndices[j]);
		}

		if (as_mesh->mMaterialIndex >= 0)
		{
			aiMaterial *material = as_scene->mMaterials[as_mesh->mMaterialIndex];

			std::vector<Texture*> diffuseMaps = LoadTextures(material, aiTextureType_DIFFUSE, TextureType::DIFFUSE);
			tmp_TextureVector.insert(tmp_TextureVector.end(), diffuseMaps.begin(), diffuseMaps.end());

			std::vector<Texture*> specularMaps = LoadTextures(material, aiTextureType_SPECULAR, TextureType::SPECULAR);
			tmp_TextureVector.insert(tmp_TextureVector.end(), specularMaps.begin(), specularMaps.end());
		}

		//m_CronosModel->m_ModelMaxVertexPos = maxPos;
		//m_CronosModel->m_ModelMinVertexPos = minPos;
		m_CronosModel->m_ModelAxis = glm::vec3((minX + maxX)/2, (minY+maxY)/2, (minZ+maxZ)/2);

		return new CronosMesh(tmp_VertexVector, tmp_IndicesVector, tmp_TextureVector);
	}

	std::vector<Texture*> AssimpCronosTranslator::LoadTextures(aiMaterial *material, aiTextureType Texturetype, TextureType TexType)
	{
		std::vector<Texture*> ret;
		for (unsigned int i = 0; i < material->GetTextureCount(Texturetype); i++)
		{
			aiString str;
			material->GetTexture(Texturetype, i, &str);

			//std::string path = m_CronosModel->m_ModelDirectoryPath.c_str();
			//const char* charsss = m_CronosModel->m_ModelDirectoryPath.c_str();
			//path += charsss;

			//if (path.size() != 0)
			//	path.append("\\");
			//path.append(str.C_Str());
			std::string path = m_CronosModel->m_ModelDirectoryPath + '/' + str.C_Str();

			Texture* tex = App->textureManager->CreateTexture(path.c_str(), TexType);
			ret.push_back(tex);
		}

		return ret;
	}

}

#include "Providers/cnpch.h"

#include "Model.h"

#include "mmgr/mmgr.h"


namespace Cronos {

	// ---------------------------------- CRONOS MESHES ----------------------------------
	CronosMesh::CronosMesh()
	{
	}

	CronosMesh::CronosMesh(std::vector<CronosVertex>vertices, std::vector<uint>indices, std::vector<CronosTexture>textures)
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

	void CronosMesh::Draw()
	{
		m_MeshVAO->Bind();
		glDrawElements(GL_TRIANGLES, m_MeshVAO->GetIndexBuffer()->GetCount(), GL_UNSIGNED_INT, nullptr);
	}

	void CronosMesh::SetupMesh()
	{
		m_MeshVAO = new VertexArray();

		CronosVertex* VBasArray = &m_VertexVector[0];
		m_MeshVBO = new VertexBuffer(VBasArray, m_VertexVector.size() * sizeof(CronosVertex));

		m_MeshVBO->SetLayout({
			{Cronos::VertexDataType::VEC3F, "a_Position", true},
			{Cronos::VertexDataType::VEC3F, "a_Normal", true},
			{Cronos::VertexDataType::VEC2F, "a_TexCoords", true}
		});

		m_MeshVAO->AddVertexBuffer(*m_MeshVBO);

		m_MeshIBO = new IndexBuffer(&m_IndicesVector[0], m_IndicesVector.size());
		m_MeshVAO->AddIndexBuffer(*m_MeshIBO);
	}

	void CronosMesh::ScaleMesh(glm::vec3 Unitary_scaleAxis, float scaleMagnitude)
	{
		if (Unitary_scaleAxis.x == 1.0f)
		{
			std::vector<CronosVertex>::iterator item = m_VertexVector.begin();
			for (; item != m_VertexVector.end(); item++)
				(*item).Position.x *= scaleMagnitude;
		}
		if (Unitary_scaleAxis.y == 1.0f)
		{
			std::vector<CronosVertex>::iterator item = m_VertexVector.begin();
			for (; item != m_VertexVector.end(); item++)
				(*item).Position.y *= scaleMagnitude;
		}
		if (Unitary_scaleAxis.z == 1.0f)
		{
			std::vector<CronosVertex>::iterator item = m_VertexVector.begin();
			for (; item != m_VertexVector.end(); item++)
				(*item).Position.z *= scaleMagnitude;
		}
	}

	void CronosMesh::MoveMesh(glm::vec3 Unitary_moveAxis, float moveMagnitude)
	{
		if (Unitary_moveAxis.x == 1.0f)
		{
			std::vector<CronosVertex>::iterator item = m_VertexVector.begin();
			for (; item != m_VertexVector.end(); item++)
				(*item).Position.x += moveMagnitude;
		}
		if (Unitary_moveAxis.y == 1.0f)
		{
			std::vector<CronosVertex>::iterator item = m_VertexVector.begin();
			for (; item != m_VertexVector.end(); item++)
				(*item).Position.y += moveMagnitude;
		}
		if (Unitary_moveAxis.z == 1.0f)
		{
			std::vector<CronosVertex>::iterator item = m_VertexVector.begin();
			for (; item != m_VertexVector.end(); item++)
				(*item).Position.z += moveMagnitude;
		}
	}


	// ---------------------------------- CRONOS MODELS ----------------------------------
	CronosModel::CronosModel(const std::string& filepath)
	{
		// Stream log messages to Debug window
		struct aiLogStream stream;
		stream = aiGetPredefinedLogStream(aiDefaultLogStream_DEBUGGER, nullptr);
		aiAttachLogStream(&stream);

		//LoadCronosModel(filepath);
		AssimpCronosTranslator m_ACT(this);
		m_ACT.LoadModel(filepath);
	}

	CronosModel::~CronosModel()
	{
		// detach log stream
		aiDetachAllLogStreams();

		std::vector<CronosMesh*>::iterator item = m_ModelMeshesVector.begin();
		for (; item != m_ModelMeshesVector.end(); item++)
		{
			(*item)->~CronosMesh();
			m_ModelMeshesVector.erase(item);
		}

		m_ModelMeshesVector.clear();
	}

	void CronosModel::Draw()
	{
		//Just iterate all model's meshes and draw them
		for (uint i = 0; i < m_ModelMeshesVector.size(); i++)
			m_ModelMeshesVector[i]->Draw();
	}


	void CronosModel::ScaleModel(glm::vec3 Unitary_scaleAxis, float scaleMagnitude)
	{
		if ((Unitary_scaleAxis.x == 0.0f || Unitary_scaleAxis.x == 1.0f) &&
			(Unitary_scaleAxis.y == 0.0f || Unitary_scaleAxis.y == 1.0f) &&
			(Unitary_scaleAxis.z == 0.0f || Unitary_scaleAxis.z == 1.0f) &&
			scaleMagnitude >= 0.0f)
		{

			std::vector<CronosMesh*>::iterator item = m_ModelMeshesVector.begin();
			for (; item != m_ModelMeshesVector.end(); item++)
			{
				(*item)->ScaleMesh(Unitary_scaleAxis, scaleMagnitude);
				CronosMesh* tmpMesh = new CronosMesh((*item)->GetVertexVector(), (*item)->GetIndexVector(), (*item)->GetTexturesVector());;
				(*item)->~CronosMesh();
				(*item) = nullptr;
				*item = tmpMesh;
			}
		}
		else
			LOG("Couldn't Scale Model. Axis must only contain 0s or 1s and scaleMagnitude must be bigger or equal than 0!");
	}

	void CronosModel::MoveModel(glm::vec3 Unitary_moveAxis, float moveMagnitude)
	{
		if ((Unitary_moveAxis.x == 0.0f || Unitary_moveAxis.x == 1.0f) &&
			(Unitary_moveAxis.y == 0.0f || Unitary_moveAxis.y == 1.0f) &&
			(Unitary_moveAxis.z == 0.0f || Unitary_moveAxis.z == 1.0f))
		{

			std::vector<CronosMesh*>::iterator item = m_ModelMeshesVector.begin();
			for (; item != m_ModelMeshesVector.end(); item++)
			{
				(*item)->MoveMesh(Unitary_moveAxis, moveMagnitude);
				CronosMesh* tmpMesh = new CronosMesh((*item)->GetVertexVector(), (*item)->GetIndexVector(), (*item)->GetTexturesVector());;
				(*item)->~CronosMesh();
				(*item) = nullptr;
				*item = tmpMesh;
			}
		}
		else
			LOG("Couldn't Move Model. Axis must only contain 0s or 1s!");
	}


	// ---------------------------------- ASSIMP-CRONOS MODEL TRANSLATOR ----------------------------------
	void AssimpCronosTranslator::LoadModel(const std::string& filepath)
	{
		//Generate an Assimp Importer & Call ReadFile() to actually read the model file
		//aiProcess_Triangulate makes all the model's primitive shapes to be triangles if they aren't
		//aiProcess_FlipUVs flips the texture UV's Y axis (necessary)
		//Other useful option: _GenNormals (generates normals for vertices if they don't have),
		//_OptimizeMeshes (joins several meshes into 1 to reduce drawcalls)
		Assimp::Importer importer;
		const aiScene* scene = importer.ReadFile(filepath, aiProcess_Triangulate | aiProcess_FlipUVs);

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
	}

	void AssimpCronosTranslator::ProcessAssimpNode(aiNode* as_node, const aiScene* as_scene)
	{
		//Process node's meshes if there are
		for (uint i = 0; i < as_node->mNumMeshes; i++)
		{
			aiMesh* mesh = as_scene->mMeshes[as_node->mMeshes[i]];
			m_CronosModel->m_ModelMeshesVector.push_back(ProcessCronosMesh(mesh));
		}

		//Process all the meshes of the node's childrens
		for (uint i = 0; i < as_node->mNumChildren; i++)
			ProcessAssimpNode(as_node->mChildren[i], as_scene);
	}

	CronosMesh* AssimpCronosTranslator::ProcessCronosMesh(aiMesh* as_mesh)
	{
		std::vector<CronosVertex>tmp_VertexVector;
		std::vector<uint>tmp_IndicesVector;
		std::vector<CronosTexture>tmp_TextureVector;

		//Process mesh's vertex positions
		for (uint i = 0; i < as_mesh->mNumVertices; i++)
		{
			CronosVertex tmpVertex;
			tmpVertex.Position = glm::vec3(as_mesh->mVertices[i].x, as_mesh->mVertices[i].y, as_mesh->mVertices[i].z);
			tmpVertex.Normal = glm::vec3(as_mesh->mNormals[i].x, as_mesh->mNormals[i].y, as_mesh->mNormals[i].z);

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

		CronosTexture defT = { 0, "TEXTURENONE" };
		tmp_TextureVector.push_back(defT);

		return new CronosMesh(tmp_VertexVector, tmp_IndicesVector, tmp_TextureVector);
	}
}
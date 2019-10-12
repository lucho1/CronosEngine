#include "Providers/cnpch.h"

#include "Model.h"

#include "mmgr/mmgr.h"


namespace Cronos {

	// ------------ CRONOS MESHES ------------
	Mesh::Mesh()
	{
	}

	Mesh::Mesh(std::vector<Vertex>vertices, std::vector<uint>indices, std::vector<Texture>textures)
	{
		m_VertexVector = vertices;
		m_IndicesVector = indices;
		m_TexturesVector = textures;

		SetupMesh();
	}

	Mesh::~Mesh()
	{
		m_MeshVAO->~VertexArray();

		//if (m_MeshVAO != nullptr)
		//	delete m_MeshVAO;
		//if (m_MeshIBO != nullptr)
		//	delete m_MeshIBO;
		//if (m_MeshVBO != nullptr)
		//	delete m_MeshVBO;
	}

	void Mesh::Draw()
	{
		m_MeshVAO->Bind();
		glDrawElements(GL_TRIANGLES, m_MeshVAO->GetIndexBuffer()->GetCount(), GL_UNSIGNED_INT, nullptr);
	}

	void Mesh::SetupMesh()
	{
		m_MeshVAO = new VertexArray();

		Vertex* VBasArray = &m_VertexVector[0];
		m_MeshVBO = new VertexBuffer(VBasArray, m_VertexVector.size() * sizeof(Vertex));

		m_MeshVBO->SetLayout({
			{Cronos::VertexDataType::VEC3F, "a_Position"},
			{Cronos::VertexDataType::VEC3F, "a_Normal"},
			{Cronos::VertexDataType::VEC2F, "a_TexCoords"}
		});

		m_MeshVAO->AddVertexBuffer(*m_MeshVBO);

		m_MeshIBO = new IndexBuffer(&m_IndicesVector[0], m_IndicesVector.size());
		m_MeshVAO->AddIndexBuffer(*m_MeshIBO);
	}


	// ------------ CRONOS MODELS ------------
	Model::Model(const std::string& filepath)
	{
		// Stream log messages to Debug window
		struct aiLogStream stream;
		stream = aiGetPredefinedLogStream(aiDefaultLogStream_DEBUGGER, nullptr);
		aiAttachLogStream(&stream);

		LoadCronosModel(filepath);
	}

	Model::~Model()
	{
		// detach log stream
		aiDetachAllLogStreams();

		std::vector<Mesh*>::iterator item = m_ModelMeshesVector.begin();
		for (; item != m_ModelMeshesVector.end(); item++)
		{
			delete *item;
			m_ModelMeshesVector.erase(item);
		}

		m_ModelMeshesVector.clear();
	}

	void Model::Draw()
	{
		//Just iterate all model's meshes and draw them
		for (uint i = 0; i < m_ModelMeshesVector.size(); i++)
			m_ModelMeshesVector[i]->Draw();
	}

	void Model::LoadCronosModel(const std::string& filepath)
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

		//If all is correct, we process all the nodes passing the first one (root)
		m_ModelDirectoryPath = filepath.substr(0, filepath.find_last_of('/'));
		ProcessNode(scene->mRootNode, scene);
	}

	void Model::ProcessNode(aiNode* node, const aiScene* scene)
	{
		//Process node's meshes if there are
		for (uint i = 0; i < node->mNumMeshes; i++)
		{
			aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
			m_ModelMeshesVector.push_back(ProcessCronosMesh(mesh, scene));
		}

		//Process all the meshes of the node's childrens
		for (uint i = 0; i < node->mNumChildren; i++)
			ProcessNode(node->mChildren[i], scene);
	}

	Mesh* Model::ProcessCronosMesh(aiMesh* mesh, const aiScene* scene)
	{
		std::vector<Vertex>tmp_VertexVector;
		std::vector<uint>tmp_IndicesVector;
		std::vector<Texture>tmp_TextureVector;

		//Process mesh's vertex positions
		for (uint i = 0; i < mesh->mNumVertices; i++)
		{
			Vertex tmpVertex;
			tmpVertex.Position = glm::vec3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);
			tmpVertex.Normal = glm::vec3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z);
			
			if (mesh->mTextureCoords[0])
				tmpVertex.TexCoords = glm::vec2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y);
			else
				tmpVertex.TexCoords = glm::vec2(0.0f, 0.0f);

			tmp_VertexVector.push_back(tmpVertex);
		}

		//Process mesh's indices
		for (uint i = 0; i < mesh->mNumFaces; i++)
		{
			aiFace face = mesh->mFaces[i];
			for (uint j = 0; j < face.mNumIndices; j++)
				tmp_IndicesVector.push_back(face.mIndices[j]);
		}

		Texture defT = { 0, "TEXTURENONE" };
		tmp_TextureVector.push_back(defT);

		return new Mesh(tmp_VertexVector, tmp_IndicesVector, tmp_TextureVector);
	}

}
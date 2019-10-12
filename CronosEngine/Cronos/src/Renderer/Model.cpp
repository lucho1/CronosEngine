#include "Providers/cnpch.h"

#include "Model.h"

#include "mmgr/mmgr.h"


namespace Cronos {

	// ---------------------------------- CRONOS MESHES ----------------------------------
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
		float linelength = 1.0f;
		glColor4f(0.6f, 0.4f, 0.2f, 1.0f);
		std::vector<uint>::iterator item = m_IndicesVector.begin();

		for (uint i = 0; i < m_IndicesVector.size();)
		{
			glm::vec3 planeVecNorm = glm::vec3(0, 0, 0);
			glm::vec3 auxvec = glm::vec3(0, 0, 0);
			glm::vec3 planepos = glm::vec3(0, 0, 0);

			for (uint j = 0; j < 3; j++)
			{
				if (i < m_VertexVector.size()) {
					auxvec += m_VertexVector[i].Normal;
					planepos += m_VertexVector[i].Position;
				}
				i++;
			}

			float auxvecMagnitude = glm::sqrt((glm::pow(auxvec.x, 2)) + (glm::pow(auxvec.y, 2)) + (glm::pow(auxvec.z, 2)));
			auxvec /= auxvecMagnitude;
			planepos /= 3;
			planeVecNorm = planepos + auxvec;

			glBegin(GL_LINES);
				glVertex3f(planepos.x, planepos.y, planepos.z);
				glVertex3f(planeVecNorm.x + linelength, planeVecNorm.y + linelength, planeVecNorm.z + linelength);
			glEnd();
		}
	}

	void CronosMesh::SetupMesh()
	{
		m_MeshVAO = new VertexArray();

		CronosVertex* VBasArray = &m_VertexVector[0];
		m_MeshVBO = new VertexBuffer(VBasArray, m_VertexVector.size() * sizeof(CronosVertex));

		m_MeshVBO->SetLayout({
			{Cronos::VertexDataType::VEC3F, "a_Position"},
			{Cronos::VertexDataType::VEC3F, "a_Normal"},
			{Cronos::VertexDataType::VEC2F, "a_TexCoords"}
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

			CalculateModelAxis();
		}
		else
			LOG("Couldn't Scale Model. Axis must only contain 0s or 1s and scaleMagnitude must be bigger or equal than 0!");
	}

	//TODO: Change this, is not optimal!
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

			CalculateModelAxis();
		}
		else
			LOG("Couldn't Move Model. Axis must only contain 0s or 1s!");
	}

	void CronosModel::CalculateModelAxis()
	{
		glm::vec3 new_axis = glm::vec3(0, 0, 0);
		float numVertices = 0.0f;

		std::vector<CronosMesh*>::iterator item = m_ModelMeshesVector.begin();
		for (; item != m_ModelMeshesVector.end(); item++)
		{
			std::vector<CronosVertex>vVec = (*item)->GetVertexVector();
			std::vector<CronosVertex>::iterator vertex_item = vVec.begin();

			for (; vertex_item != vVec.end(); vertex_item++)
			{
				new_axis += (*vertex_item).Position;
				numVertices++;
			}
		}

		new_axis /= numVertices;
		m_ModelAxis = new_axis;
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
			m_CronosModel->m_ModelMeshesVector.push_back(ProcessCronosMesh(as_mesh));
		}

		//Process all node's childrens
		for (uint i = 0; i < as_node->mNumChildren; i++)
			ProcessAssimpNode(as_node->mChildren[i], as_scene);
	}

	CronosMesh* AssimpCronosTranslator::ProcessCronosMesh(aiMesh* as_mesh)
	{
		LOG("	Processing Model Mesh");
		std::vector<CronosVertex>tmp_VertexVector;
		std::vector<uint>tmp_IndicesVector;
		std::vector<CronosTexture>tmp_TextureVector;

		//Process mesh vertices
		for (uint i = 0; i < as_mesh->mNumVertices; i++)
		{
			//First, position & normals
			CronosVertex tmpVertex;
			tmpVertex.Position = glm::vec3(as_mesh->mVertices[i].x, as_mesh->mVertices[i].y, as_mesh->mVertices[i].z);
			tmpVertex.Normal = glm::vec3(as_mesh->mNormals[i].x, as_mesh->mNormals[i].y, as_mesh->mNormals[i].z);

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

		CronosTexture defT = { 0, "TEXTURENONE" };
		tmp_TextureVector.push_back(defT);

		return new CronosMesh(tmp_VertexVector, tmp_IndicesVector, tmp_TextureVector);
	}
}
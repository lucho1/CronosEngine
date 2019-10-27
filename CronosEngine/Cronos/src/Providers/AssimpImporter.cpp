#include "Providers/cnpch.h"

#include "AssimpImporter.h"
#include "Application.h"

#include "GameObject/Components/TransformComponent.h"

#include "mmgr/mmgr.h"

namespace Cronos {

	// ---------------------------------- CRONOS MESHES ----------------------------------
	//void CronosMesh::ScaleMesh(glm::vec3 ScaleMagnitude)
	//{
	//	//glm::mat4 scale = glm::mat4(1.0f);
	//	//scale = glm::scale(ScaleMagnitude);
	//	//m_Transformation *= scale;
	//	//DecomposeTransformation();

	//	m_Transformation = glm::scale(m_Transformation, ScaleMagnitude);
	//	DecomposeTransformation();

	//	//ModelMatrix = glm::scale(ModelMatrix, Scale);
	//	/*glm::mat4 translation = glm::mat4(1.0f);
	//	translation = glm::scale(translation, ScaleMagnitude);

	//	std::vector<CronosVertex>::iterator item = m_VertexVector.begin();
	//	for (; item != m_VertexVector.end(); item++)
	//	{
	//		glm::vec4 pos = glm::vec4((*item).Position, 1.0f);
	//		(*item).Position = translation * pos;
	//	}*/
	//}

	//void CronosMesh::MoveMesh(glm::vec3 MoveAxis, float moveMagnitude)
	//{
	//	//glm::mat4 translation = glm::mat4(1.0f);
	//	//translation = glm::translate(translation, MoveAxis);

	//	m_Transformation = glm::translate(m_Transformation, MoveAxis);

	//	//m_Transformation += translation;
	//	DecomposeTransformation();

	//	//MoveAxis *= moveMagnitude;
	//	//glm::mat4 translation = glm::mat4(1.0f);
	//	//translation = glm::translate(translation, MoveAxis);
	//	//
	//	//std::vector<CronosVertex>::iterator item = m_VertexVector.begin();
	//	//for (; item != m_VertexVector.end(); item++)
	//	//{
	//	//	glm::vec4 pos = glm::vec4((*item).Position, 1.0f);
	//	//	(*item).Position = translation * pos;
	//	//}

	//}

	//void CronosMesh::RotateMesh(float RotDegrees, glm::vec3 RotAxis, glm::vec3 OwnAxis)
	//{
	//	glm::mat4 rot = glm::mat4(1.0f);
	//	rot = glm::rotate(rot, glm::radians(RotDegrees), RotAxis);
	//	m_Transformation *= rot;
	//	DecomposeTransformation();
	//}

	//void CronosMesh::DecomposeTransformation()
	//{
	//	glm::vec3 skew;
	//	glm::vec4 perspective;
	//	glm::decompose(m_Transformation, m_Scale, m_Rotation, m_Translation, skew, perspective);
	//	m_Rotation = glm::conjugate(m_Rotation);
	//}


	// ---------------------------------- ASSIMP-CRONOS MODEL TRANSLATOR ----------------------------------
	AssimpCronosImporter::AssimpCronosImporter()
	{
		// Stream log messages to Debug window
		struct aiLogStream stream;
		stream = aiGetPredefinedLogStream(aiDefaultLogStream_DEBUGGER, nullptr);
		aiAttachLogStream(&stream);
	}

	GameObject* AssimpCronosImporter::LoadModel(const std::string& filepath)
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
			return nullptr;
		}

		//This thing with directory is to get the model directory and not its path, so
		//the substr() gets, in this case, all the characters until the last '/' char.
		//So, if filepath is "AA/BB/model.fbx", this will be "AA/BB"
		//m_CronosModel->m_ModelDirectoryPath = filepath.substr(0, filepath.find_last_of('/'));
		std::string GOName = filepath.substr(filepath.find_last_of('/') + 1, filepath.find_last_of('.') - 4);
		GameObject* mother_GO = new GameObject(GOName, App->m_RandomNumGenerator.GetIntRN(), filepath.substr(0, filepath.find_last_of('/')));


		//If all is correct, we process all the nodes passing the first one (root)
		ProcessAssimpNode(scene->mRootNode, scene, mother_GO);

		//For the parent AABB, I'll just get the first child's AABB
		auto comp = (*mother_GO->m_Childs.begin())->GetComponent<TransformComponent>();
		mother_GO->SetAABB(comp->GetAABB().getMin(), comp->GetAABB().getMax());

		// detach log stream
		aiDetachAllLogStreams();

		return mother_GO;
	}

	void AssimpCronosImporter::ProcessAssimpNode(aiNode* as_node, const aiScene* as_scene, GameObject* motherGameObj)
	{
		LOG("	Processing Assimp Node");
		//Process node's meshes if there are
		for (uint i = 0; i < as_node->mNumMeshes; i++)
		{
			//Get the mesh from the meshes list of the node in scene's meshes list
			aiMesh* as_mesh = as_scene->mMeshes[as_node->mMeshes[i]];
			ProcessCronosMesh(as_mesh, as_scene, motherGameObj);
		}
		
		//Process all node's childrens
		for (uint i = 0; i < as_node->mNumChildren; i++)
			ProcessAssimpNode(as_node->mChildren[i], as_scene, motherGameObj);
	}

	void AssimpCronosImporter::ProcessCronosMesh(aiMesh* as_mesh, const aiScene* as_scene, GameObject* motherGameObj)
	{
		LOG("	Processing Model Mesh");
		std::vector<CronosVertex>tmp_VertexVector;
		std::vector<uint>tmp_IndicesVector;
		std::vector<Texture*>tmp_TextureVector;
		uint facesNumber = 0;

		float minX = as_mesh->mVertices[0].x, minY = as_mesh->mVertices[0].y, minZ = as_mesh->mVertices[0].z;
		float maxX = minX, maxY = minY, maxZ = minZ;

		//Process mesh vertices
		for (uint i = 0; i < as_mesh->mNumVertices; i++)
		{
			//First, position & normals
			CronosVertex tmpVertex;
			tmpVertex.Position = glm::vec3(as_mesh->mVertices[i].x, as_mesh->mVertices[i].y, as_mesh->mVertices[i].z);
			tmpVertex.Normal = glm::vec3(as_mesh->mNormals[i].x, as_mesh->mNormals[i].y, as_mesh->mNormals[i].z);

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

			static uint PolyNum = 0;
			if (PolyNum == 2)
			{
				facesNumber++;
				PolyNum = 0;
			}
			PolyNum++;			
		}

		//Now create a Game Object and a mesh component to load the textures
		GameObject* GO = new GameObject(as_mesh->mName.C_Str(), App->m_RandomNumGenerator.GetIntRN(), motherGameObj->GetPath());

		//Set the AABB Cube
		m_AABB_MinVec = glm::vec3(minX, minY, minZ);
		m_AABB_MaxVec = glm::vec3(maxX, maxY, maxZ);
		GO->SetAABB(m_AABB_MinVec, m_AABB_MaxVec);

		

		//Process Mesh's textures
		if (as_mesh->mMaterialIndex >= 0)
		{
			aiMaterial *material = as_scene->mMaterials[as_mesh->mMaterialIndex];

			std::vector<Texture*> ambientMaps = LoadTextures(material, aiTextureType_AMBIENT, TextureType::AMBIENT, GO);
			tmp_TextureVector.insert(tmp_TextureVector.end(), ambientMaps.begin(), ambientMaps.end());

			std::vector<Texture*> diffuseMaps = LoadTextures(material, aiTextureType_DIFFUSE, TextureType::DIFFUSE, GO);
			tmp_TextureVector.insert(tmp_TextureVector.end(), diffuseMaps.begin(), diffuseMaps.end());

			std::vector<Texture*> specularMaps = LoadTextures(material, aiTextureType_SPECULAR, TextureType::SPECULAR, GO);
			tmp_TextureVector.insert(tmp_TextureVector.end(), specularMaps.begin(), specularMaps.end());

			std::vector<Texture*> normalMaps = LoadTextures(material, aiTextureType_NORMALS, TextureType::NORMALMAP, GO);
			tmp_TextureVector.insert(tmp_TextureVector.end(), normalMaps.begin(), normalMaps.end());	

			std::vector<Texture*> heightMaps = LoadTextures(material, aiTextureType_HEIGHT, TextureType::HEIGHTMAP, GO);
			tmp_TextureVector.insert(tmp_TextureVector.end(), heightMaps.begin(), heightMaps.end());

			std::vector<Texture*> lightMaps = LoadTextures(material, aiTextureType_HEIGHT, TextureType::LIGHTMAP, GO);
			tmp_TextureVector.insert(tmp_TextureVector.end(), lightMaps.begin(), lightMaps.end());
		}
		
		//Setup the component mesh and put GO into the mother's childs list
		MeshComponent* meshComp = ((MeshComponent*)(GO->CreateComponent(ComponentType::MESH)));
		meshComp->SetupMesh(tmp_VertexVector, tmp_IndicesVector, tmp_TextureVector);
		GO->m_Components.push_back(meshComp);
		motherGameObj->m_Childs.push_back(GO);

		LOG("Processed Mesh with %i Vertices %i Indices and %i Polygons (Triangles) and %i Textures", tmp_VertexVector.size(), tmp_IndicesVector.size(), facesNumber, tmp_TextureVector.size());
	}

	std::vector<Texture*> AssimpCronosImporter::LoadTextures(aiMaterial *material, aiTextureType Texturetype, TextureType TexType, GameObject* motherGameObj)
	{
		std::vector<Texture*> ret;
		for (unsigned int i = 0; i < material->GetTextureCount(Texturetype); i++)
		{
			aiString str;
			material->GetTexture(Texturetype, i, &str);

			bool skip = false;
			for (unsigned int j = 0; j < m_TexturesLoaded.size(); j++)
			{
				if (std::strcmp(m_TexturesLoaded[j]->GetTexturePath().data(), str.C_Str()) == 0)
				{
					ret.push_back(m_TexturesLoaded[j]);
					skip = true;
					break;
				}
			}

			if (!skip)
			{
				std::string path = motherGameObj->GetPath() + '/' + str.C_Str();
				Texture* tex = App->textureManager->CreateTexture(path.c_str(), TexType);
				ret.push_back(tex);
				m_TexturesLoaded.push_back(tex);
			}
		}

		return ret;
	}

}

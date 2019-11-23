#include "Providers/cnpch.h"

#include "AssimpImporter.h"
#include "Application.h"

#include "GameObject/Components/TransformComponent.h"
#include "GameObject/Components/MeshComponent.h"
#include "GameObject/Components/MaterialComponent.h"

#include "mmgr/mmgr.h"

namespace Cronos {

	const aiTextureType ConvertToAssimpTextureType(TextureType CN_textureType)
	{
		switch (CN_textureType)
		{
			case TextureType::AMBIENT:		return aiTextureType_AMBIENT;
			case TextureType::DIFFUSE:		return aiTextureType_DIFFUSE;
			case TextureType::SPECULAR:		return aiTextureType_SPECULAR;
			case TextureType::NORMALMAP:	return aiTextureType_NORMALS;
			case TextureType::HEIGHTMAP:	return aiTextureType_HEIGHT;
			case TextureType::LIGHTMAP:		return aiTextureType_LIGHTMAP;
		}

		CRONOS_ASSERT(0, "COULDN'T CONVERT TO ASSIMP TEXTURE TYPE!");
		return aiTextureType_NONE;
	}

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
		//_OptimizeMeshes (joins several meshes into 1 to reduce draw calls)
		LOG("LOADING MODEL");
		Assimp::Importer importer;
		const aiScene* scene = importer.ReadFile(filepath, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcessPreset_TargetRealtime_MaxQuality);

		//Report an error if the model data is incomplete
		if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
		{
			LOG("Error in Assimp, %s", importer.GetErrorString());
			return nullptr;
		}

		//This thing with directory is to get the model directory and not its path, so
		//the substr() gets, in this case, all the characters until the last '/' char.
		//So, if filepath is "AA/BB/model.fbx", this will be "AA/BB"
		//m_CronosModel->m_ModelDirectoryPath = filepath.substr(0, filepath.find_last_of('/'));
		std::string GOName = filepath.substr(filepath.find_last_of('/') + 1, filepath.find_last_of('.') - filepath.find_last_of('/') - 1);
		GameObject* mother_GO = new GameObject(GOName, App->m_RandomNumGenerator.GetIntRN(), filepath.substr(0, filepath.find_last_of('/')));

		//If all is correct, we process all the nodes passing the first one (root)
		ProcessAssimpNode(scene->mRootNode, scene, mother_GO);

		//For the parent AABB, I'll just get the first child's AABB
		auto comp = (*mother_GO->m_Childs.begin())->GetComponent<TransformComponent>();
		mother_GO->SetAABB(comp->GetAABB().getMin(), comp->GetAABB().getMax());


		//aiReleaseImport(scene);
		// detach log stream
		aiDetachAllLogStreams();

		LOG("FINISHED LOADING MODEL -- Model With %i Meshes", MeshNum);
		MeshNum = 0;
		return mother_GO;
	}

	void AssimpCronosImporter::ProcessAssimpNode(aiNode* as_node, const aiScene* as_scene, GameObject* motherGameObj)
	{
		LOG("	Processing Assimp Node");
		//aiVector3D translation, scaling, EulerRotation;
		//aiQuaternion rotation;
		//as_node->mTransformation.Decompose(scaling, rotation, translation);
		//EulerRotation = rotation.GetEuler();
		//
		//if (scaling.x > 50.0f || scaling.y > 50.0f || scaling.z > 50.0f)
		//	scaling = aiVector3D(1.0f);
		//
		//////scaling = scaling / 10.0f;
		//motherGameObj->GetComponent<TransformComponent>()->SetScale(glm::vec3(1.0f/*scaling.x, scaling.y, scaling.z*/));
		//motherGameObj->GetComponent<TransformComponent>()->SetPosition(glm::vec3(translation.x, translation.y, translation.z));
		//motherGameObj->GetComponent<TransformComponent>()->SetOrientation(glm::degrees(glm::vec3(EulerRotation.x, EulerRotation.y, EulerRotation.z)));

		//Process node's meshes if there are
		for (uint i = 0; i < as_node->mNumMeshes; i++)
		{
			//Get the mesh from the meshes list of the node in scene's meshes list
			aiMesh* as_mesh = as_scene->mMeshes[as_node->mMeshes[i]];
			ProcessCronosMesh(as_mesh, as_scene, motherGameObj, as_node);
		}

		//Process all node's children
		for (uint i = 0; i < as_node->mNumChildren; i++)
			ProcessAssimpNode(as_node->mChildren[i], as_scene, motherGameObj);
	}

	void AssimpCronosImporter::ProcessCronosMesh(aiMesh* as_mesh, const aiScene* as_scene, GameObject* motherGameObj, aiNode* as_node)
	{
		LOG("	Processing Model Mesh");
		MeshNum++;
		std::vector<CronosVertex>tmp_VertexVector;
		std::vector<uint>tmp_IndicesVector;
		uint facesNumber = 0;

		float minX = as_mesh->mVertices[0].x, minY = as_mesh->mVertices[0].y, minZ = as_mesh->mVertices[0].z;
		float maxX = minX, maxY = minY, maxZ = minZ;

		//Process mesh vertices
		for (uint i = 0; i < as_mesh->mNumVertices; i++)
		{
			//First, position & normals
			CronosVertex tmpVertex;
			tmpVertex.Position = glm::vec3(as_mesh->mVertices[i].x, as_mesh->mVertices[i].y, as_mesh->mVertices[i].z);

			if(as_mesh->HasNormals())
				tmpVertex.Normal = glm::vec3(as_mesh->mNormals[i].x, as_mesh->mNormals[i].y, as_mesh->mNormals[i].z);

			//Then we see if there are text. coordinates in the first set [0] (OGL has until 8)
			if (as_mesh->mTextureCoords[0])
				tmpVertex.TexCoords = glm::vec2(as_mesh->mTextureCoords[0][i].x, as_mesh->mTextureCoords[0][i].y);
			else
				tmpVertex.TexCoords = glm::vec2(0.0f, 0.0f);


			minX = MIN(minX, tmpVertex.Position.x); minY = MIN(minY, tmpVertex.Position.y); minZ = MIN(minZ, tmpVertex.Position.z);
			maxX = MAX(maxX, tmpVertex.Position.x); maxY = MAX(maxY, tmpVertex.Position.y); maxZ = MAX(maxZ, tmpVertex.Position.z);
			tmp_VertexVector.push_back(tmpVertex);
		}

		//Set the AABB Cube
		m_AABB_MinVec = glm::vec3(minX, minY, minZ);
		m_AABB_MaxVec = glm::vec3(maxX, maxY, maxZ);

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


		//Now set up the new Game Object
		std::string GOName;
		if (as_node->mName.length > 0)
			GOName = as_node->mName.C_Str();
		else
			GOName = "Game Object";

		//Create a Game Object
		GameObject* GO = new GameObject(GOName.substr(0, GOName.find_last_of('.')), App->m_RandomNumGenerator.GetIntRN(), motherGameObj->GetPath());

		aiVector3D translation, scaling, EulerRotation;
		aiQuaternion rotation;
		as_node->mTransformation.Decompose(scaling, rotation, translation);
		EulerRotation = rotation.GetEuler();

		if (scaling.x > 50.0f || scaling.y > 50.0f || scaling.z > 50.0f)
			scaling = aiVector3D(1.0f);

		//motherGameObj->GetComponent<TransformComponent>()->SetScale(glm::vec3(1.0f/*scaling.x, scaling.y, scaling.z*/));
		GO->GetComponent<TransformComponent>()->SetPosition(glm::vec3(translation.x, translation.y, translation.z));
		//motherGameObj->GetComponent<TransformComponent>()->SetOrientation(glm::degrees(glm::vec3(EulerRotation.x, EulerRotation.y, EulerRotation.z)));


		//Setup the component mesh and put GO into the mother's childs list
		MeshComponent* meshComp = ((MeshComponent*)(GO->CreateComponent(ComponentType::MESH)));
		
		meshComp->SetupMesh(tmp_VertexVector, tmp_IndicesVector);
		GO->SetParent(motherGameObj);
		GO->m_Components.push_back(meshComp);

		//Process Mesh's textures/material
		if (as_mesh->mMaterialIndex >= 0)
		{
			aiMaterial* AssimpMaterial = as_scene->mMaterials[as_mesh->mMaterialIndex];
			MaterialComponent* matComp = (MaterialComponent*)(GO->CreateComponent(ComponentType::MATERIAL));

			for (uint i = 1; i < (uint)TextureType::MAX_TEXTURES; i++)
				matComp->SetTexture(LoadTextures(AssimpMaterial, TextureType(i), GO->GetPath()), TextureType(i));

			matComp->SetShader(App->scene->BasicTestShader);
			GO->m_Components.push_back(matComp);
		}

		//Set the GO AABB and finally push it to the mother's child list
		GO->SetAABB(m_AABB_MinVec, m_AABB_MaxVec);
		motherGameObj->m_Childs.push_back(GO);

		LOG("	Processed Mesh with %i Vertices %i Indices and %i Polygons (Triangles)", tmp_VertexVector.size(), tmp_IndicesVector.size(), facesNumber);
	}


	Texture* AssimpCronosImporter::LoadTextures(aiMaterial *material, TextureType TexType, const std::string& GOPath)
	{
		/*aiColor3D color = aiColor3D(0, 0, 0);
		material->Get(AI_MATKEY_COLOR_AMBIENT, color);*/ //Maybe also shininess?
		//AMBIENT (color?), DIFFUSE, SPECULAR, NORMALMAP, HEIGHTMAP, LIGHTMAP

		//Get the texture wanted from Assimp
		aiTextureType Ass_TextureType = ConvertToAssimpTextureType(TexType);

		if(material->GetTextureCount(Ass_TextureType) > 0)
		{
			aiString str;
			material->GetTexture(Ass_TextureType, 0, &str);
			std::string path = GOPath + '/' + str.C_Str();


			//If it has been already loaded, then return it without creating a new one
			std::list<Texture*>::iterator it = App->scene->m_TexturesLoaded.begin();
			for (; it != App->scene->m_TexturesLoaded.end() && (*it) != nullptr; it++)
				if (std::strcmp((*it)->GetTexturePath().data(), path.c_str()) == 0)
					return (*it);


			//If it has been already loaded, then return it without creating a new one
			/*for (unsigned int j = 0; j < m_TexturesLoaded.size(); j++)
				if (std::strcmp(m_TexturesLoaded[j]->GetTexturePath().data(), path.c_str()) == 0)
					return m_TexturesLoaded[j];*/

			//Else, create it and put it in the loaded textures vector
			Texture* tex = App->textureManager->CreateTexture(path.c_str(), TexType);

			//m_TexturesLoaded.push_back(tex);
			return tex;
		}

		return nullptr;
	}

}
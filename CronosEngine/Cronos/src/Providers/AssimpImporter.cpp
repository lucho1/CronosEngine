#include "Providers/cnpch.h"

#include "AssimpImporter.h"
#include "Application.h"
#include "Modules/Filesystem.h"

#include "GameObject/Components/TransformComponent.h"
#include "GameObject/Components/MeshComponent.h"
#include "GameObject/Components/MaterialComponent.h"

#include "mmgr/mmgr.h"

namespace Cronos {

	const aiTextureType ConvertToAssimpTextureType(TextureType CN_textureType)
	{
		switch (CN_textureType)
		{
			case TextureType::DIFFUSE:		return aiTextureType_DIFFUSE;
			case TextureType::SPECULAR:		return aiTextureType_SPECULAR;
			//case TextureType::NORMALMAP:	return aiTextureType_NORMALS;
			//case TextureType::HEIGHTMAP:	return aiTextureType_HEIGHT;
			//case TextureType::LIGHTMAP:		return aiTextureType_LIGHTMAP;
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

		//Mother's AABB
		math::AABB mother_aabb;
		mother_aabb.SetNegativeInfinity();

		std::list<GameObject*>::iterator it = mother_GO->m_Childs.begin();
		for (; it != mother_GO->m_Childs.end(); it++)
			mother_aabb.Enclose((*it)->GetAABB());

		mother_GO->SetAABB(mother_aabb);
		mother_GO->SetOOBB(mother_aabb);
		mother_GO->SetInitialAABB(mother_aabb);

		//Abd save in own format
		App->filesystem->SaveOwnFormat(mother_GO);


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

		//Let's first look for the name to set the new game object (or a default one)
		std::string GOName;
		if (as_node->mName.length > 0)
			GOName = as_node->mName.C_Str();
		else
			GOName = "Game Object";

		//Create a Game Object
		GameObject* GO = new GameObject(GOName.substr(0, GOName.find_last_of('.')), App->m_RandomNumGenerator.GetIntRN(), motherGameObj->GetPath());
		GO->HasVertices = true;

		//Setup the component mesh and put GO into the mother's childs list
		MeshComponent* meshComp = ((MeshComponent*)(GO->CreateComponent(ComponentType::MESH)));

		//Creating the resource with unique ID
		meshComp->r_mesh = new ResourceMesh(App->m_RandomNumGenerator.GetIntRN());
		ResourceMesh* rMesh = meshComp->r_mesh;

		//Setup of the Buffers Size
		rMesh->m_BufferSize[0] = as_mesh->mNumVertices;

		//Loading Vertex Positions
		rMesh->Position = new float[rMesh->m_BufferSize[0] * 3];
		memcpy(rMesh->Position, as_mesh->mVertices, sizeof(float)*rMesh->m_BufferSize[0] * 3);

		//Loading Indexes
		if (as_mesh->HasFaces())
		{
			rMesh->m_BufferSize[3] = as_mesh->mNumFaces;
			rMesh->Index = new uint[rMesh->m_BufferSize[3] * 3];

			for (uint i = 0; i < as_mesh->mNumFaces; i++)
			{
				aiFace face = as_mesh->mFaces[i];
				memcpy(&rMesh->Index[i*3], face.mIndices ,sizeof(uint) * 3);
			}
		}
		else
			rMesh->m_BufferSize[3] = 0;

		//Loading Vertex Normals
		if (as_mesh->HasNormals())
		{
			rMesh->m_BufferSize[1] = as_mesh->mNumVertices;
			rMesh->Normal = new float[rMesh->m_BufferSize[1] * 3];
			memcpy(rMesh->Normal, as_mesh->mNormals, sizeof(float)*rMesh->m_BufferSize[0] * 3);

		}
		else
			rMesh->m_BufferSize[1] = 0;

		//Loading Vertex Texture Coordinates
		if (as_mesh->HasTextureCoords(0))
		{
			rMesh->m_BufferSize[2] = as_mesh->mNumVertices;
			rMesh->TextureV = new float[rMesh->m_BufferSize[2] * 2];

			for (uint i = 0; i < as_mesh->mNumVertices; i++)
			{
				rMesh->TextureV[i * 2] = as_mesh->mTextureCoords[0][i].x;
				rMesh->TextureV[i * 2 + 1] = as_mesh->mTextureCoords[0][i].y;
			}
		}
		else
			rMesh->m_BufferSize[2] = 0;

		//Transform information into cronos vertex data
		rMesh->toCronosVertexVector();
		meshComp->SetupMesh(rMesh->getVector(),rMesh->getIndex());

		//Getting & Setting the transformation
		aiVector3D translation, scaling;
		aiQuaternion rotation;
		as_node->mTransformation.Decompose(scaling, rotation, translation);

		aiVector3D translation2, scaling2;
		aiQuaternion rotation2;
		as_scene->mRootNode;
		as_scene->mRootNode->mTransformation.Decompose(scaling2, rotation2, translation2);

		glm::vec3 EulerAnglesRotation = glm::eulerAngles(glm::quat(rotation.w, rotation.x, rotation.y, rotation.z));

		if (scaling.x > 95.0f || scaling.y > 95.0f || scaling.z > 95.0f)
			scaling = aiVector3D(95.0f);

		//motherGameObj->GetComponent<TransformComponent>()->SetScale(glm::vec3(scaling.x, scaling.y, scaling.z));
		GO->GetComponent<TransformComponent>()->SetPosition(glm::vec3(translation.x, translation.y, translation.z));
		//motherGameObj->GetComponent<TransformComponent>()->SetOrientation(glm::degrees(EulerAnglesRotation));

		GO->SetParent(motherGameObj);
		GO->m_Components.push_back(meshComp);

		//Process Mesh's textures/material
		if (as_mesh->mMaterialIndex >= 0)
		{
			aiMaterial* AssimpMaterial = as_scene->mMaterials[as_mesh->mMaterialIndex];
			MaterialComponent* matComp = (MaterialComponent*)(GO->CreateComponent(ComponentType::MATERIAL));

			for (uint i = 1; i < (uint)TextureType::MAX_TEXTURES; i++)
				matComp->SetTexture(LoadTextures(AssimpMaterial, TextureType(i), GO->GetPath()), TextureType(i));

			aiColor3D col = aiColor3D(0, 0, 0);
			AssimpMaterial->Get(AI_MATKEY_COLOR_AMBIENT, col);
			if(col.r >= 0.01f || col.g >= 0.01f || col.b >= 0.01f)
				matComp->SetColor(glm::vec4(col.r, col.g, col.b, 1.0f));

			GO->m_Components.push_back(matComp);
		}

		//Set the Game Object's AABB, OOBB and Initial AABB
		float size = rMesh->getVector().size();
		math::float3* verts = new math::float3[size];
		for (uint i = 0; i < size; i++)
		{
			glm::vec3 vec = rMesh->getVector()[i].Position;
			verts[i] = math::float3(vec.x, vec.y, vec.z);
		}
		
		math::AABB aabb;
		math::OBB oobb;

		math::float4x4 mat = math::float4x4::identity;
		mat.Set(glm::value_ptr(GO->GetComponent<TransformComponent>()->GetGlobalTranformationMatrix()));

		aabb.SetNegativeInfinity();
		aabb.SetFrom(verts, size);		
		oobb.SetFrom(aabb);
		oobb.Transform(mat);
		
		GO->SetInitialAABB(aabb);
		GO->SetOOBB(oobb);
		GO->SetAABB(aabb);
		delete[] verts;

		//Push the child game object into the mother
		motherGameObj->m_Childs.push_back(GO);
		LOG("	Processed Mesh with %i Vertices and %i Indices", rMesh->m_BufferSize[0], rMesh->m_BufferSize[1]);
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

			//Else, create it and put it in the loaded textures vector
			Texture* tex = App->textureManager->CreateTexture(path.c_str(), TexType);
			return tex;
		}

		return nullptr;
	}

}

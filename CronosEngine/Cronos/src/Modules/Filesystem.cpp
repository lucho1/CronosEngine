#include "Providers/cnpch.h"

#include "Filesystem.h"
#include "Application.h"
#include "TextureManager.h"
#include "PhysFS/include/physfs.h"

#include "GameObject/Components/TransformComponent.h"

#include "Resources/ResourceMesh.h"

#include "Modules/ResourceManager.h"
#include "Modules/Scene.h"

#include "Renderer/GLRenderer3D.h"

#include "ImGui/ImGuiLayer.h"

#include <fstream>

namespace Cronos {

	Filesystem::Filesystem(Application* app, bool start_enabled) : Module(app, "Module Filesystem", start_enabled)
	{
		for (uint i = 0; i < (uint)ItemType::MAX_ITEMS; i++)
			ArrayIconTextures[i] = nullptr;
	}

	bool Filesystem::OnStart() {

		LOG("LOADING FILESYSTEM ICONS");
		//loader Icons -> this needs to be loaded with JSON
		ArrayIconTextures[(int)ItemType::ITEM_FBX] = App->textureManager->CreateTexture("res/Icons/Fbx_Icon.png", TextureType::ICON);
		ArrayIconTextures[(int)ItemType::ITEM_FOLDER] = App->textureManager->CreateTexture("res/Icons/Folder_Icon.png", TextureType::ICON);
		ArrayIconTextures[(int)ItemType::ITEM_MATERIAL] = App->textureManager->CreateTexture("res/Icons/Material_Icon.png", TextureType::ICON);
		ArrayIconTextures[(int)ItemType::ITEM_OBJ] = App->textureManager->CreateTexture("res/Icons/Obj_Icon.png", TextureType::ICON);
		ArrayIconTextures[(int)ItemType::ITEM_SCRIPT] = App->textureManager->CreateTexture("res/Icons/Script_Icon.png", TextureType::ICON);
		ArrayIconTextures[(int)ItemType::ITEM_SHADER] = App->textureManager->CreateTexture("res/Icons/Shader_Icon.png", TextureType::ICON);

		m_LibraryPath = "lib/";
		m_HiddenMeshLibPath = m_LibraryPath + "Meshes/";
		m_HiddenMaterialLibPath = m_LibraryPath + "Materials/";
		m_ScenePath = "res/Scenes/";

		std::wstring Temp_lib = std::wstring(m_LibraryPath.begin(),m_LibraryPath.end());

		SetFileAttributes(Temp_lib.c_str(),FILE_ATTRIBUTE_HIDDEN);

		if (!std::filesystem::exists(m_LibraryPath))
			std::filesystem::create_directory(m_LibraryPath);

		if (!std::filesystem::exists(m_HiddenMeshLibPath))
			std::filesystem::create_directory(m_HiddenMeshLibPath);

		if (!std::filesystem::exists(m_HiddenMaterialLibPath))
			std::filesystem::create_directory(m_HiddenMaterialLibPath);

		m_RootDirectory = std::filesystem::current_path();
		m_LabelRootDirectory = m_RootDirectory.filename().string();
		m_AssetRoot = LoadCurrentDirectories(m_RootDirectory);


		char* base_path = new char[GetRootPath().length() + 1];
		strcpy(base_path, GetRootPath().c_str());
		PHYSFS_init(base_path);
		SDL_free(base_path);

		return true;
	}

	bool Filesystem::OnCleanUp()
	{
		for (uint i = 0; i < (uint)ItemType::MAX_ITEMS; i++)
			if (ArrayIconTextures[i] != nullptr)
				RELEASE(ArrayIconTextures[i]);

		return true;

	}

	bool saveData(MeshComponent* mesh,const char* filePath) {

		if (filePath == nullptr) {
			CRONOS_WARN(filePath != nullptr, ("Unable to find Path to save: " + std::string(filePath)));
			return false;
		}

		ResourceMesh* rMesh = mesh->r_mesh;

		//std::vector<CronosVertex>Vertex = mesh->GetVertexVector();
		//std::vector<uint>Index = mesh->GetIndexVector();

		uint totalSize=0;
		uint bytes = 0;
		int a  = sizeof(CronosVertex);

		uint range[4] = {rMesh->m_BufferSize[0],rMesh->m_BufferSize[1],rMesh->m_BufferSize[2],rMesh->m_BufferSize[3]};

		totalSize = sizeof(range) + (sizeof(float) * 3 * range[0]) + (sizeof(float) * 3 * range[1]) + (sizeof(float) * 2 * range[2]) + (sizeof(uint) * range[3]*3);

		char*Data = new char[totalSize];
		char*cursor = Data;

		//Store Total of Vertices and Indexs
		bytes = sizeof(range);
		memcpy(cursor, range, bytes);

		//Store Position
		cursor += bytes;
		bytes = sizeof(float) * 3 * range[0];
		memcpy(cursor, rMesh->Position, bytes);
		cursor += bytes;

		//Store Normal
		if (range[1] > 0) {
			bytes = sizeof(float) * 3 * range[1];
			memcpy(cursor, rMesh->Normal, bytes);
			cursor += bytes;
		}

		//Store Texture Vector
		if (range[2] > 0) {
			bytes = sizeof(float) * 2 * range[2];
			memcpy(cursor, rMesh->TextureV, bytes);
			cursor += bytes;
		}
		//Store Index
		if (range[3] > 0) {
			bytes = sizeof(uint) * range[3] * 3;
			memcpy(cursor, rMesh->Index, bytes);
			cursor += bytes;
		}

		std::ofstream MeshData{filePath, std::ios::binary };
		MeshData.write(Data, totalSize);
		MeshData.close();


		SDL_free(Data);

	}

	bool Filesystem::SaveMaterial(Material* material,const char* path)
	{
		bool ret = true;
		//std::string Data = m_HiddenMaterialLibPath + std::to_string(material->GetMaterialID()) + ".material";
		std::string Data = path;
		Data+="/" + material->GetMatName()+".material";
		const char* filePath = Data.c_str();
		if (filePath == nullptr) {
			CRONOS_WARN(filePath != nullptr, ("Unable to find Path to save: " + std::string(filePath)));
			ret = false;
		}

		json aux_JSONFile;

		aux_JSONFile["Name"] = material->GetMatName().c_str();
		aux_JSONFile["ID"] = material->GetMaterialID();
		
		aux_JSONFile["Shininess"] = material->GetMaterialShininess();
		
		aux_JSONFile["AlbedoColor"][0] = material->GetMaterialColor().r;
		aux_JSONFile["AlbedoColor"][1] = material->GetMaterialColor().g;
		aux_JSONFile["AlbedoColor"][2] = material->GetMaterialColor().b;
		aux_JSONFile["AlbedoColor"][3] = material->GetMaterialColor().a;

		if (material->GetTextureType(TextureType::DIFFUSE) != nullptr) {
			aux_JSONFile["DiffuseTexturePath"] = material->GetTextureType(TextureType::DIFFUSE)->GetTexturePath();
			aux_JSONFile["DiffuseTextureID"] = material->GetTextureType(TextureType::DIFFUSE)->GetTextureID();
		}
		if (material->GetTextureType(TextureType::SPECULAR) != nullptr) {

			aux_JSONFile["SpecularTexturePath"] = material->GetTextureType(TextureType::SPECULAR)->GetTexturePath();
			aux_JSONFile["SpecularTextureID"] = material->GetTextureType(TextureType::SPECULAR)->GetTextureID();
		}

		std::ofstream OutputFile_Stream{ filePath,std::ofstream::out };
		OutputFile_Stream << std::setw(2) << aux_JSONFile;
		OutputFile_Stream.close();
		return ret;
	}

	bool Save(GameObject* ToConvert)
	{
		//Open file (or return if couuldn't)
		std::string Data= ToConvert->GetMetaPath();
		const char* filePath = Data.c_str();
		if (filePath == nullptr) {
			CRONOS_WARN(filePath != nullptr, ("Unable to find Path to save: " + std::string(filePath)));
			return false;
		}

		//RootInfo
		//std::ofstream ToSave{ RootGameObject->GetMetaPath().c_str() };
		//ToSave << Data << std::endl;

		json aux_JSONFile;

		//Game Object's common data
		aux_JSONFile["Name"] = ToConvert->GetName().c_str();
		aux_JSONFile["Path"] = ToConvert->GetPath().c_str();
		aux_JSONFile["ID"] = ToConvert->GetGOID();
		aux_JSONFile["Active"] = ToConvert->isActive();

		//Game Object's AABB
		aux_JSONFile["ComponentTransform"]["AABBMinVec"][0] = ToConvert->GetAABB().minPoint.x;
		aux_JSONFile["ComponentTransform"]["AABBMinVec"][1] = ToConvert->GetAABB().MinY();
		aux_JSONFile["ComponentTransform"]["AABBMinVec"][2] = ToConvert->GetAABB().MinZ();
		aux_JSONFile["ComponentTransform"]["AABBMaxVec"][0] = ToConvert->GetAABB().MaxX();
		aux_JSONFile["ComponentTransform"]["AABBMaxVec"][1] = ToConvert->GetAABB().MaxY();
		aux_JSONFile["ComponentTransform"]["AABBMaxVec"][2] = ToConvert->GetAABB().MaxZ();

		//Game Object's Initial AABB
		aux_JSONFile["ComponentTransform"]["InitialAABBMinVec"][0] = ToConvert->GetInitialAABB().MinX();
		aux_JSONFile["ComponentTransform"]["InitialAABBMinVec"][1] = ToConvert->GetInitialAABB().MinY();
		aux_JSONFile["ComponentTransform"]["InitialAABBMinVec"][2] = ToConvert->GetInitialAABB().MinZ();
		aux_JSONFile["ComponentTransform"]["InitialAABBMaxVec"][0] = ToConvert->GetInitialAABB().MaxX();
		aux_JSONFile["ComponentTransform"]["InitialAABBMaxVec"][1] = ToConvert->GetInitialAABB().MaxY();
		aux_JSONFile["ComponentTransform"]["InitialAABBMaxVec"][2] = ToConvert->GetInitialAABB().MaxZ();

		//Game Object's Transform
		TransformComponent* Transform = ToConvert->GetComponent<TransformComponent>();
		aux_JSONFile["ComponentTransform"]["Position"][0] = Transform->GetTranslation().x;
		aux_JSONFile["ComponentTransform"]["Position"][1] = Transform->GetTranslation().y;
		aux_JSONFile["ComponentTransform"]["Position"][2] = Transform->GetTranslation().z;
		aux_JSONFile["ComponentTransform"]["Rotation"][0] = Transform->GetOrientation().x;
		aux_JSONFile["ComponentTransform"]["Rotation"][1] = Transform->GetOrientation().y;
		aux_JSONFile["ComponentTransform"]["Rotation"][2] = Transform->GetOrientation().z;
		aux_JSONFile["ComponentTransform"]["Scale"][0] = Transform->GetScale().x;
		aux_JSONFile["ComponentTransform"]["Scale"][1] = Transform->GetScale().y;
		aux_JSONFile["ComponentTransform"]["Scale"][2] = Transform->GetScale().z;

		//Game Object's Material
		if (ToConvert->GetComponent<MaterialComponent>())
		{
			aux_JSONFile["ComponentMaterial"]["MaterialID"] = ToConvert->GetComponent<MaterialComponent>()->GetMaterial()->GetMaterialID();

			//Color
			//aux_JSONFile["ComponentMaterial"]["Color"]["R"] = ToConvert->GetComponent<MaterialComponent>()->GetColor().r;
			//aux_JSONFile["ComponentMaterial"]["Color"]["G"] = ToConvert->GetComponent<MaterialComponent>()->GetColor().g;
			//aux_JSONFile["ComponentMaterial"]["Color"]["B"] = ToConvert->GetComponent<MaterialComponent>()->GetColor().b;
			//aux_JSONFile["ComponentMaterial"]["Color"]["A"] = ToConvert->GetComponent<MaterialComponent>()->GetColor().a;
			//
			////Textures
			//std::unordered_map<TextureType, Texture*>m_TexturesContainer= ToConvert->GetComponent<MaterialComponent>()->GetTextures();
			//if(m_TexturesContainer[TextureType::DIFFUSE]!=nullptr)
			//	aux_JSONFile["ComponentMaterial"]["Albedo"] = m_TexturesContainer[TextureType::DIFFUSE]->GetTexturePath();
			//if(m_TexturesContainer[TextureType::SPECULAR]!=nullptr)
			//	aux_JSONFile["ComponentMaterial"]["Specular"] = m_TexturesContainer[TextureType::SPECULAR]->GetTexturePath();

		}

		//Game Object's Mesh
		if (ToConvert->GetComponent<MeshComponent>())
		{
			MeshComponent* mesh = ToConvert->GetComponent<MeshComponent>();
			App->resourceManager->AddResource(mesh->r_mesh);

			aux_JSONFile["ComponentMesh"]["VertexSize"] = mesh->GetVertexVector().size();
			aux_JSONFile["ComponentMesh"]["IndexSize"] = mesh->GetIndexVector().size();

			std::string Mesh_Path = App->filesystem->GetMeshLib();
			Mesh_Path += std::to_string(mesh->r_mesh->GetResID()) + ".mesh";
			aux_JSONFile["ComponentMesh"]["MeshPath"] = Mesh_Path.c_str();
			aux_JSONFile["ComponentMesh"]["MeshID"] = mesh->r_mesh->GetResID();

			saveData(mesh, Mesh_Path.c_str());
		}

		//Game Object's Parenting
		if (ToConvert->GetParentGameObject() != nullptr)
			aux_JSONFile["ParentID"] = ToConvert->GetParentGameObject()->GetGOID();

		//Game Obect's Childs
		if (ToConvert->GetCountChilds() > 0)
		{
			aux_JSONFile["Childs"]["NumberChilds"] = ToConvert->GetCountChilds();
			int index = 0;
			for (auto&childs :ToConvert->m_Childs)
			{
				aux_JSONFile["Childs"]["IDChild"][index] = childs->GetGOID();
				aux_JSONFile["Childs"]["MetaPath"][index] = childs->GetMetaPath();
				++index;
			}
		}

		//Close and return
		std::ofstream OutputFile_Stream{ filePath,std::ofstream::out };
		OutputFile_Stream << std::setw(2) << aux_JSONFile;
		OutputFile_Stream.close();
		return true;
	}

	//bool 

	bool Filesystem::SaveOwnFormat(GameObject* RootGameObject)
	{
		Save(RootGameObject);
		for (auto& childs : RootGameObject->m_Childs)
			SaveOwnFormat(childs);

		return true;
	}

	bool Filesystem::LoadMesh(const char* filepath, MeshComponent& mesh,uint ResID) {

		bool ret = false;
		ResourceMesh* rMesh = new ResourceMesh(ResID);
		mesh.r_mesh = rMesh;

		char*Data = nullptr;
		std::ifstream file{ filepath, std::ios::binary };

		char* cursor = nullptr;
		if (file.is_open()) {

			file.seekg(0, file.end);
			uint size = file.tellg();
			//Cursor to beggining;
			file.seekg(0);

			if (size > 0) {
				Data = new char[size];
				file.read(Data, size);

			}

			cursor = (char*)Data;

			//Load Total of Vertices and Indices
			uint range[4];
			uint bytes = sizeof(range);
			memcpy(rMesh->m_BufferSize, cursor, bytes);
			//rMesh->m_BufferSize[0] = range[0];
			//rMesh->m_BufferSize[1] = range[1];

			//Load Vector Position
			cursor += bytes;
			bytes = sizeof(float) * 3 * rMesh->m_BufferSize[0];
			rMesh->Position = new float[rMesh->m_BufferSize[0] * 3];
			memcpy(rMesh->Position, cursor, bytes);
			cursor+=bytes;

			//Load Normals
			if (range[1] > 0) {
				bytes = sizeof(float) * 3 * rMesh->m_BufferSize[0];
				rMesh->Normal = new float[rMesh->m_BufferSize[0] * 3];
				memcpy(rMesh->Normal, cursor, bytes);
				cursor += bytes;
			}

			//Load TextureCoordinates
			if (range[2] > 0) {
				bytes = sizeof(float) * 2 * rMesh->m_BufferSize[0];
				rMesh->TextureV = new float[rMesh->m_BufferSize[0] * 2];
				memcpy(rMesh->TextureV, cursor, bytes);
				cursor += bytes;
			}
			//Load Indices
			if (range[3] > 0) {
				bytes = sizeof(uint) * rMesh->m_BufferSize[1] * 3;
				rMesh->Index = new uint[rMesh->m_BufferSize[1] * 3];
				memcpy(rMesh->Index, cursor, bytes);
				cursor += bytes;
			}
			rMesh->toCronosVertexVector();

			//Set the GO AABB and finally push it to the mother's child list
			math::AABB aabb;
			aabb.SetNegativeInfinity();

			//rMesh->Position = new float[rMesh->m_BufferSize[0] * 3];

			float VectorSize = rMesh->getVector().size();
			math::float3* verts = new math::float3[VectorSize];
			for (uint i = 0; i < VectorSize; i++)
			{
				glm::vec3 vec = rMesh->getVector()[i].Position;
				verts[i] = math::float3(vec.x, vec.y, vec.z);
			}

			aabb.Enclose(verts, VectorSize);
			delete[] verts;

			// Generate global OBB
			mesh.GetParent()->SetAABB(aabb);
			mesh.GetParent()->SetOOBB(aabb);

			GameObject* obj = mesh.GetParent();
			int xx = 0;
			//Ret->SetAABB(aabb);
			//Ret->SetOOBB(aabb);
		}
		else
		{
			LOG("FileSystem error loading file %s");
			ret = false;
		}

		SDL_free(Data);
		return ret;
	}

	ResourceMaterial* Filesystem::LoadMaterial(const char* filepath) {

		Material* ret = new Material();
		//std::string filename = m_LibraryPath + m_HiddenMaterialLibPath + std::to_string(MatID) + ".material";
		bool exists = std::filesystem::exists(filepath);
		if (!exists) {
			CRONOS_WARN(!exists, ("Filepath %s doesn't exist", filepath));
			return false;
		}

		std::ifstream file{ filepath };
		if (file.is_open())
		{
			//Once file is opened, operate in it
			json configFile = json::parse(file);
			ret->SetName(configFile["Name"].get < std::string>());
			ret->SetID(configFile["ID"].get<uint>());
			
			glm::vec4 color;
			color.r = configFile["AlbedoColor"][0].get<float>();
			color.g = configFile["AlbedoColor"][1].get<float>();
			color.b = configFile["AlbedoColor"][2].get<float>();
			color.a = configFile["AlbedoColor"][3].get<float>();
			float shininess = configFile["Shininess"].get<float>();

			ret->SetColor(color);
			ret->SetShininess(shininess);

			if (configFile.contains("DiffuseTexturePath"))
			{
				std::string TextName = configFile["DiffuseTexturePath"].get <std::string>();
				std::list<Texture*>::iterator it = App->scene->m_TexturesLoaded.begin();
				
				bool textureLoaded = false;
				for (; it != App->scene->m_TexturesLoaded.end() && (*it) != nullptr; it++)
				{
					if (std::strcmp((*it)->GetTexturePath().data(), TextName.c_str()) == 0)
					{
						textureLoaded = true;
						ret->SetTexture((*it), TextureType::DIFFUSE);
						break;
					}					
				}

				if (!textureLoaded)
				{
					Texture* tex = App->textureManager->CreateTexture(TextName.c_str(), TextureType::DIFFUSE);
					ret->SetTexture(tex, TextureType::DIFFUSE);
				}

			}
			if (configFile.contains("SpecularTexturePath"))
			{
				std::string TextName = configFile["SpecularTexturePath"].get <std::string>();
				std::list<Texture*>::iterator it = App->scene->m_TexturesLoaded.begin();
				
				bool textureLoaded = false;
				for (; it != App->scene->m_TexturesLoaded.end() && (*it) != nullptr; it++)
				{
					if (std::strcmp((*it)->GetTexturePath().data(), TextName.c_str()) == 0)
					{
						textureLoaded = true;
						ret->SetTexture((*it), TextureType::SPECULAR);
						break;
					}
				}

				if (!textureLoaded)
				{
					Texture* tex = App->textureManager->CreateTexture(TextName.c_str(), TextureType::SPECULAR);
					ret->SetTexture(tex, TextureType::SPECULAR);
				}
			}

			if (!App->resourceManager->isMaterialLoaded(ret->GetMaterialID()))
			{
				ResourceMaterial* res = new ResourceMaterial(ret->GetMaterialID(), ret);
				App->resourceManager->AddResource(res);
				return res;
			}
			else
				return App->resourceManager->getMaterialResource(ret->GetMaterialID());

		}
		return nullptr;
	}

	GameObject* Filesystem::Load(int GOID)
	{
		GameObject* ret = nullptr;
		std::string filename = m_LibraryPath + std::to_string(GOID) + ".model";
		bool exists = std::filesystem::exists(filename);

		if (!exists)
		{
			CRONOS_WARN(!exists, ("Filepath %s doesn't exist", filename.c_str()));
			return nullptr;
		}

		std::ifstream file{ filename.c_str() };
		if (file.is_open())
		{
			//Once file is opened, operate in it
			json configFile = json::parse(file);

			//Game Object's common data
			int id = GOID;
			bool Active = configFile["Active"].get<bool>();

			std::string name = configFile["Name"].get<std::string>();
			std::string Path = configFile["Path"].get<std::string>();

			if (configFile.contains("ParentID"))
				int ParentID = configFile["ParentID"].get<int>();

			//Game Object's Transform
			glm::vec3 Position = glm::vec3(configFile["ComponentTransform"]["Position"][0],
				configFile["ComponentTransform"]["Position"][1],
				configFile["ComponentTransform"]["Position"][2]);

			glm::vec3 Rotation = glm::vec3(configFile["ComponentTransform"]["Rotation"][0],
				configFile["ComponentTransform"]["Rotation"][1],
				configFile["ComponentTransform"]["Rotation"][2]);

			glm::vec3 Scale = glm::vec3(configFile["ComponentTransform"]["Scale"][0],
				configFile["ComponentTransform"]["Scale"][1],
				configFile["ComponentTransform"]["Scale"][2]);

			//Create a Game Object (to be returned) with the previous data
			ret = new GameObject(name, id, Path, Active, Position, Rotation, Scale);

			//Set Game Object's AABB, OOBB & InitAABB
			glm::vec3 AABBMinVec = glm::vec3(configFile["ComponentTransform"]["AABBMinVec"][0],
				configFile["ComponentTransform"]["AABBMinVec"][1],
				configFile["ComponentTransform"]["AABBMinVec"][2]);

			glm::vec3 AABBMaxVec = glm::vec3(configFile["ComponentTransform"]["AABBMaxVec"][0],
				configFile["ComponentTransform"]["AABBMaxVec"][1],
				configFile["ComponentTransform"]["AABBMaxVec"][2]);

			glm::vec3 InitialAABBMinVec = glm::vec3(configFile["ComponentTransform"]["InitialAABBMinVec"][0],
				configFile["ComponentTransform"]["InitialAABBMinVec"][1],
				configFile["ComponentTransform"]["InitialAABBMinVec"][2]);

			glm::vec3 InitialAABBMaxVec = glm::vec3(configFile["ComponentTransform"]["InitialAABBMaxVec"][0],
				configFile["ComponentTransform"]["InitialAABBMaxVec"][1],
				configFile["ComponentTransform"]["InitialAABBMaxVec"][2]);

			TransformComponent* transf = ret->GetComponent<TransformComponent>();
			math::AABB aabb = math::AABB(math::float3(AABBMinVec.x, AABBMinVec.y, AABBMinVec.z), math::float3(AABBMaxVec.x, AABBMaxVec.y, AABBMaxVec.z));
			math::AABB initAABB = math::AABB(math::float3(InitialAABBMinVec.x, InitialAABBMinVec.y, InitialAABBMinVec.z), math::float3(InitialAABBMaxVec.x, InitialAABBMaxVec.y, InitialAABBMaxVec.z));
			math::OBB oobb;

			math::float4x4 mat = math::float4x4::identity;
			mat.Set(glm::value_ptr(transf->GetGlobalTranformationMatrix()));

			oobb.SetFrom(aabb);
			oobb.Transform(mat);

			ret->SetInitialAABB(initAABB);
			ret->SetOOBB(oobb);
			ret->SetAABB(aabb);

			transf->SetPosition(Position);
			transf->SetOrientation(Rotation);
			transf->SetScale(Scale);

			//If Game Object has mesh, load it
			if (configFile.contains("ComponentMesh"))
			{
				MeshComponent* MeshComp = ((MeshComponent*)(ret->CreateComponent(ComponentType::MESH)));
				uint resID = configFile["ComponentMesh"]["MeshID"].get<uint>();

				if (App->resourceManager->isMeshLoaded(resID))
					MeshComp->r_mesh = App->resourceManager->getMeshResource(resID);
				else
				{
					std::string MeshPath = configFile["ComponentMesh"]["MeshPath"].get < std::string >();
					LoadMesh(MeshPath.c_str(), *MeshComp, resID);
					App->resourceManager->AddResource(MeshComp->r_mesh);
				}

				MeshComp->SetupMesh(MeshComp->r_mesh->getVector(), MeshComp->r_mesh->getIndex());
				ret->m_Components.push_back(MeshComp);

				//If Game Object has mesh & Material, load the material (no point on loading material with no mesh)
				if (configFile.contains("ComponentMaterial"))
				{
					//Create the component from the file's data
					json CompMat = configFile["ComponentMaterial"];
					MaterialComponent* MatComp = ((MaterialComponent*)(ret->CreateComponent(ComponentType::MATERIAL)));
					ResourceMaterial* Remat = App->resourceManager->getMaterialResource(configFile["ComponentMaterial"]["MaterialID"].get<uint>());
					
					//uint matIndex = configFile["ComponentMaterial"]["MaterialIndex"].get<uint>();
					//if (matIndex < 0 || matIndex >= App->renderer3D->GetMaterialsList().size())
					//	matIndex = 0;
					//
					//MatComp->m_MaterialIndex = matIndex;
					MatComp->SetMaterial(*Remat->m_Material);
									

					//Set the color
					//glm::vec4 color;
					//color.r = configFile["ComponentMaterial"]["Color"]["R"].get<float>();
					//color.g = configFile["ComponentMaterial"]["Color"]["G"].get<float>();
					//color.b = configFile["ComponentMaterial"]["Color"]["B"].get<float>();
					//color.a = configFile["ComponentMaterial"]["Color"]["A"].get<float>();
					//MatComp->SetColor(color);
					//
					////Set the Albedo texture (if any)
					//if (CompMat.contains("Albedo"))
					//{
					//	std::string AlbedoPath = configFile["ComponentMaterial"]["Albedo"].get<std::string>();
					//	Texture* textTemp = App->textureManager->CreateTexture(AlbedoPath.c_str(), TextureType::DIFFUSE);
					//	MatComp->SetTexture(textTemp, textTemp->GetTextureType());
					//}

					//Put the Material Component into the Game Object
					ret->m_Components.push_back(MatComp);
				}
			}

			//If any child, load it and set it as child
			if (configFile.contains("Childs"))
			{
				int numOfChilds = configFile["Childs"]["NumberChilds"].get<int>();
				for (int i = 0; i < numOfChilds; ++i)
				{
					int ChildID = configFile["Childs"]["IDChild"][i].get<int>();
					GameObject* Child = Load(ChildID);
					Child->SetParent(ret);
					ret->m_Childs.push_back(Child);
				}
			}
		}
		else
		{
			CRONOS_WARN(file.is_open(), ("Unable to Open file to load! Filename: %s", filename.c_str()));
			file.close();
		}

		return ret;
	}

	bool AssetItems::HasMeta() const
	{

		std::string APath = m_Path;
		APath.erase(APath.find(m_Extension));
		APath += ".asset";

		if (std::filesystem::exists(APath))
			return true;

		return false;
	}

	bool saveAssetDetails(GameObject* root, json& config) {

		config[root->GetPath()] = root->GetGOID();

		for(auto& childs : root->m_Childs)
		{
			saveAssetDetails(childs,config);
		}
		return true;
	}

	uint loadAsset(const char* Path) {
		uint ret = 0;
		json AssetFile;

		std::ifstream file{ Path };
		if (file.is_open()) {
			json configFile = json::parse(file);
			ret = configFile[Path].get<uint>();
		}
		return ret;
	}

	bool SaveAsset(GameObject*Root,const char* Path) {

		json AssetFile;
		//saveAssetDetails(Root,AssetFile); Needs to be fixed in the future
		AssetFile[Path] = Root->GetGOID();

		std::ofstream OutputFile_Stream{ Path,std::ofstream::out };
		OutputFile_Stream << std::setw(2) << AssetFile;
		OutputFile_Stream.close();

		return true;
	}

	AssetItems::AssetItems(std::filesystem::path m_path,Directories* parentfolder,ItemType mtype, bool beloadedLater): folderDirectory(parentfolder),type(mtype),m_Path(m_path.root_path().string()) {
		
		m_path.make_preferred();
		m_Path = m_path.parent_path().generic_string();
		m_AbsolutePath = m_path.generic_string();
		std::string temp = m_path.generic_string();
		temp.erase(0, App->filesystem->GetRootPath().size()+1);
		m_Path = temp;
		//temp-=App->filesystem->GetLabelAssetRoot();

		m_AssetFullName = m_path.filename().string();
		sprintf_s(labelID,"%s", m_AssetFullName.c_str());
		m_AssetNameNoExtension = m_AssetShortName = m_AssetFullName;

		m_AssetID = App->m_RandomNumGenerator.GetIntRN();



		if (m_AssetFullName.length() > 10) {
			m_AssetShortName.erase(10);
			m_AssetShortName += "...";
		}
		if (m_path.has_extension()) {
			m_Extension = m_path.extension().string();
			m_AssetNameNoExtension.erase(m_AssetNameNoExtension.find(m_Extension));
		}

		if (beloadedLater) {
			type = mtype;
			return;
		}

		if (m_Extension == ".obj") {
			type = ItemType::ITEM_OBJ;
			m_IconTex = App->filesystem->GetIcon(type);
			std::string path = m_Path;
			path.erase(path.find(m_Extension));
			path += ".asset";
			if (!HasMeta())
			{
				GameObject*temp = App->filesystem->m_CNAssimp_Importer.LoadModel(m_Path);
				if (temp != nullptr) {
					SaveAsset(temp, path.c_str());
					m_GameObjecID = temp->GetGOID();
					temp->CleanUp();
				}
				else
					LOG("Failed To Load %s ", m_Path.c_str());
			}
			else
			{
				m_GameObjecID = loadAsset(path.c_str());
			}
		}
		else if (m_Extension == ".fbx"||m_Extension == ".FBX") {
			type = ItemType::ITEM_FBX;
			m_IconTex = App->filesystem->GetIcon(type);
			std::string path = m_Path;
			path.erase(path.find(m_Extension));
			path += ".asset";
			if(!HasMeta())
			{
				GameObject*temp = App->filesystem->m_CNAssimp_Importer.LoadModel(m_Path);
				if (temp != nullptr) {
					SaveAsset(temp, path.c_str());
					m_GameObjecID = temp->GetGOID();
					temp->CleanUp();
				}
				else
					LOG("Failed To Load %s ", m_Path.c_str());
			}
			else
			{
				m_GameObjecID = loadAsset(path.c_str());
			}
		}
		else if (m_Extension == ".glsl") {
			type = ItemType::ITEM_SHADER;
			m_Shader = new Shader(m_Path.c_str());
			m_IconTex = App->filesystem->GetIcon(type);
		}
		else if (m_Extension == ".cpp" || m_Extension == ".h") {
			type = ItemType::ITEM_SCRIPT;
			m_IconTex = App->filesystem->GetIcon(type);
		}
		else if (m_Extension == ".png" ) {

			type = ItemType::ITEM_TEXTURE_PNG;
			m_AssetTexture = App->textureManager->CreateTexture(m_Path.c_str(), TextureType::ICON);

			m_Resolution = ImVec2(m_AssetTexture->GetWidth(), m_AssetTexture->GetHeight());
			m_IconTex = m_AssetTexture->GetTextureID();
			m_Details += std::to_string((int)m_AssetTexture->GetWidth());
			m_Details += "x";
			m_Details += std::to_string((int)m_AssetTexture->GetHeight());
			m_Details += " ";
			m_Details += m_AssetFullName;

		}
		else if (m_Extension == ".dds") {
			type = ItemType::ITEM_TEXTURE_DDS;
			m_AssetTexture = App->textureManager->CreateTexture(m_Path.c_str(), TextureType::ICON);


			m_Resolution = ImVec2(m_AssetTexture->GetWidth(), m_AssetTexture->GetHeight());
			m_IconTex = m_AssetTexture->GetTextureID();
			m_Details += std::to_string((int)m_AssetTexture->GetWidth());
			m_Details += "x";
			m_Details += std::to_string((int)m_AssetTexture->GetHeight());
			m_Details += " ";
			m_Details += m_AssetFullName;
		}
		else if (m_Extension == ".jpeg"||m_Extension==".jpg") {
			type = ItemType::ITEM_TEXTURE_JPEG;
			m_AssetTexture = App->textureManager->CreateTexture(m_Path.c_str(), TextureType::ICON);

			m_Resolution = ImVec2(m_AssetTexture->GetWidth(), m_AssetTexture->GetHeight());
			m_IconTex = m_AssetTexture->GetTextureID();
			m_Details += std::to_string((int)m_AssetTexture->GetWidth());
			m_Details += "x";
			m_Details += std::to_string((int)m_AssetTexture->GetHeight());
			m_Details += " ";
			m_Details += m_AssetFullName;
		}
		else if (m_Extension == ".tga") {
			type = ItemType::ITEM_TEXTURE_TGA;
			m_AssetTexture = App->textureManager->CreateTexture(m_Path.c_str(), TextureType::ICON);

			m_Resolution = ImVec2(m_AssetTexture->GetWidth(), m_AssetTexture->GetHeight());
			m_IconTex = m_AssetTexture->GetTextureID();
			m_Details += std::to_string((int)m_AssetTexture->GetWidth());
			m_Details += "x";
			m_Details += std::to_string((int)m_AssetTexture->GetHeight());
			m_Details += " ";
			m_Details += m_AssetFullName;

		}
		else if (m_Extension == ".material") {
			type = ItemType::ITEM_MATERIAL;
			m_resMaterial = App->filesystem->LoadMaterial(m_Path.c_str());
			m_IconTex = App->filesystem->GetIcon(type);
		}
		if (type == ItemType::ITEM_FOLDER) {
			m_IconTex = App->filesystem->GetIcon(type);
		}
	};
	void AssetItems::Clear() {
		//delete folderDirectory;
	}

	void AssetItems::DrawIcons()
	{

		ImGui::BeginGroup();

		ImGui::ImageButton((void*)(m_IconTex - 1), ImVec2(50, 50), ImVec2(0, 1), ImVec2(1, 0), 2);

		hovered = ImGui::IsItemHovered(); //ASK MARC WHY IS NOT HOVERING ALL TIME
		static double refresh_time = 0.0;
		if (hovered) {

			if (refresh_time == 0.0)
				refresh_time = ImGui::GetTime();
			//static float Time = ImGui::GetTime();
			if (ImGui::GetTime() >= refresh_time ) {
				ImGui::BeginTooltip();
				ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
				ImGui::TextUnformatted(m_AssetFullName.c_str());
				ImGui::PopTextWrapPos();
				ImGui::EndTooltip();
			}
		}
		else
			refresh_time = 0.0f;
		if (ImGui::IsItemClicked())
		{
			App->EditorGUI->modifingShader = false;
			App->EditorGUI->m_CurrentAssetClicked = this;
			if (ImGui::IsMouseDoubleClicked(0))
			{
				App->EditorGUI->m_CurrentAssetSelected = this;
				App->EditorGUI->CancelGameObject();
			}
		}

		if (ImGui::IsItemClicked(1)) {
			ImGui::OpenPopup(labelID);
		}
		if (ImGui::BeginPopup(labelID)) {

			if (ImGui::BeginMenu("Rename File")) {

				static char buf1[64];
				sprintf_s(buf1, "%s", m_AssetNameNoExtension.c_str());
				ImGui::SetNextItemWidth(ImGui::CalcTextSize(buf1).x+25);
				if (ImGui::InputText("###", buf1, 64, ImGuiInputTextFlags_CharsNoBlank)) {
					App->filesystem->RenameFile(this,buf1);
				}

				ImGui::EndMenu();
			}
			if (ImGui::MenuItem("Delete"))
			{
			/*	folderDirectory->m_Container.;
				folderDirectory->m_Container.pop_back();
				std::list<AssetItems*>::iterator it;
				for (it = folderDirectory->m_Container.begin(); it != folderDirectory->m_Container.end(); ++it) {
					if (*(it) == this) {

						break;
					}
				}
				std::wstring TempstringToDelete = std::wstring(m_AbsolutePath.begin(), m_AbsolutePath.begin());*/
				//DeleteFile(TempstringToDelete.c_str());
			}
			ImGui::EndPopup();
		}


		ImGui::Text(m_AssetShortName.c_str());
		m_ElementSize = ImGui::GetItemRectSize().x;

		ImGui::EndGroup();
	}

	int AssetItems::GetElementSize() {

		return m_ElementSize;
	}



	Directories::Directories(std::filesystem::path m_Path)
	{
		m_Path.make_preferred();
		m_Directories = m_Path.generic_string();
		m_LabelDirectories = m_Directories.string();
	}

	void Directories::Clear() 
	{
		m_Container;
	}

	void Filesystem::AddAssetFile(const char* filepath,const char* name,ItemType type) 
	{
		Directories* TempDir = GetDirectories(filepath);
		for (auto&currDir : TempDir->m_Container) {
			if (currDir->GetAssetPath() == name)
				return;
		}
		AssetItems* newAsset = new AssetItems(App->filesystem->GetRootPath()+"/"+name, TempDir);
		TempDir->m_Container.push_front(newAsset);
	}

	Directories* Filesystem::GetDirectories(const char* dirPath) 
	{
		for (auto&dir : DirectoriesArray) {
			if (dir->m_LabelDirectories.find(dirPath)!=std::string::npos)
				return dir;
		}
		return nullptr;
	}

	void Filesystem::RenameFile(AssetItems* Asset, const char* newName) 
	{

		std::string tempDirName = Asset->GetAssetPath();
		tempDirName.erase(tempDirName.find(Asset->m_AssetFullName));
		tempDirName += newName+Asset->GetExtension();
		Asset->m_AssetNameNoExtension = newName;
		std::filesystem::rename(Asset->GetAssetPath(),tempDirName);
		Asset->m_AssetFullName = newName+Asset->GetExtension();
		Asset->m_AssetShortName = Asset->m_AssetFullName;
		if (Asset->m_AssetFullName.length() > 10) {
			Asset->m_AssetShortName.erase(10);
			Asset->m_AssetShortName += "...";
		}
		Asset->SetAssetPath(tempDirName);
	}

	void Filesystem::SearchFile(Directories* tempDir, const char* name) {
		bool isInside = false;
		for (auto& a : AssetArray) {
			std::string::size_type n;
			n = a->m_AssetFullName.find(name);
			if (n != std::string::npos) {
				for (auto&b : tempDir->m_Container) {
					if (a != b)
						isInside = false;
					else {
						isInside = true;
						break;
					}

				}
				if (!isInside) {
					tempDir->m_Container.push_back(a);
				}
			}
			else
				for (auto&b : tempDir->m_Container)
					if (a == b) {
						tempDir->m_Container.remove(a);
						break;
					}

		}
	}
	//bool RecursiveSerchForExistingDir(Directories* currentDir,const char* newName) {

	//	for (auto& container : currentDir->m_Container) {
	//		RecursiveSerchForExistingDir(container,)
	//	}
	//}

	void Filesystem::CreateNewDirectory(Directories* currentDir,const char* newName) {

		std::string tempDirName = currentDir->m_LabelDirectories +"/"+ newName;
		//if (RecursiveSerchForExistingDir(currentDir, newName)) {

		//}
		for (auto& container : currentDir->m_Container) {

			if (container->GetAssetPath()==newName) {
				std::string temp = newName;
				if (temp.find_last_of("0123456789") != std::string::npos) {
					int a = temp.at(1);
				}
				else
					tempDirName += std::to_string(1);
			}
		}
		std::filesystem::create_directory(tempDirName);
		Directories* TempDir = new Directories(tempDirName);
		for (auto& a : DirectoriesArray) {
			if (a==currentDir) {
				AssetItems* t = new AssetItems(TempDir->m_Directories, TempDir, ItemType::ITEM_FOLDER);
				currentDir->m_Container.push_front(t);
				a->childs.push_back(TempDir);				
				TempDir->SetParentDirectory(a);
				break;
			}
		}
	}
	
	void Filesystem::DeleteDirectory(const char* path) {

		std::filesystem::remove(path);
	}

	void Filesystem::UpdateDirectories() {
		for (auto& a : DirectoriesArray) {
			a->Clear();
		}
	}

	Directories* Filesystem::LoadCurrentDirectories(std::filesystem::path filepath) {

		static int LastDepth = 0;
		static int ID = 0;
		static int currentDepth = 0;
		filepath.make_preferred();
		Directories* SolutionDirTemp = new Directories(filepath);
		SolutionDirTemp->m_ID = ID;
		DirectoriesArray.push_back(SolutionDirTemp);
		Directories* currentDir = SolutionDirTemp;
		for (auto& p = std::filesystem::recursive_directory_iterator(SolutionDirTemp->m_Directories); p != std::filesystem::recursive_directory_iterator(); ++p) {

			for (auto&path : p) {
				if (path.is_directory()) {
					
					ID++;
					Directories* newPath = new Directories(path.path());
					newPath->m_ID = ID;
					newPath->m_DepthID = p.depth();
					int test = p.depth();
					if (p.depth() <= LastDepth)
					{
						int	NearID = 0;
						for (auto&co : DirectoriesArray) {

							if (p.depth() > 0 && co->m_DepthID == p.depth() - 1) {
								if (co->m_ID > NearID) {
									NearID = co->m_ID;
									currentDir = co;
								}
							}
							else if (p.depth() == 0) {

								currentDir = SolutionDirTemp;
								break;
							}
						}
						currentDir->childs.push_back(newPath);

						AssetItems* t = new AssetItems(path.path().string().c_str(),newPath,ItemType::ITEM_FOLDER);
						currentDir->m_Container.push_front(t);
						newPath->SetParentDirectory(currentDir);
						currentDir = newPath;
					}
					else if (p.depth() > LastDepth) {
						currentDir->childs.push_back(newPath);

						AssetItems* t = new AssetItems(path.path().string().c_str(),newPath,ItemType::ITEM_FOLDER);

						currentDir->m_Container.push_front(t);
						newPath->SetParentDirectory(currentDir);
						currentDir = newPath;

					}

					LastDepth = p.depth();
					DirectoriesArray.push_back(newPath);

				}
				else {
					AssetItems* t = new AssetItems(path.path().string().c_str(),currentDir);
					currentDir->m_Container.push_back(t);
					AssetArray.push_back(t);
				}
			}
		}
		return SolutionDirTemp;
	}


}

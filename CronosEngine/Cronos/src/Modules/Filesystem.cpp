#include "Providers/cnpch.h"

#include "Filesystem.h"
#include "Application.h"
#include "TextureManager.h"
#include "PhysFS/include/physfs.h"
#include "GameObject/Components/TransformComponent.h"
#include "Resources/ResourceMesh.h"




#include <fstream>

namespace Cronos {



	Filesystem::Filesystem(Application* app, bool start_enabled) : Module(app, "Module Filesystem", start_enabled)
	{
		for (uint i = 0; i < (uint)ItemType::MAX_ITEMS; i++)
			ArrayIconTextures[i] = nullptr;

		//char* base_path = SDL_GetBasePath();
	
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

		ResourceMesh* rMesh = mesh->mesh;

		//std::vector<CronosVertex>Vertex = mesh->GetVertexVector();
		//std::vector<uint>Index = mesh->GetIndexVector();

		uint totalSize=0;
		uint bytes = 0;
		int a  = sizeof(CronosVertex);
	//change to 4
		uint range[2];
		//uint result  = rMesh->GetBufferSize(range);
		
		totalSize = sizeof(range) + (sizeof(float) * 3 * range[0]) + (sizeof(float) * 3 * range[0]) + (sizeof(float) * 2 * range[0]) + (sizeof(uint) * range[1]);

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
		bytes = sizeof(float) * 3 * range[0];
		memcpy(cursor, rMesh->Normal, bytes);
		cursor += bytes;

		//Store Texture Vector
		bytes = sizeof(float) * 2 * range[0];
		memcpy(cursor, rMesh->Position, bytes);
		cursor += bytes;

		//Store Index
		bytes = sizeof(uint) * range[1];
		memcpy(cursor,rMesh->Index, bytes);
		cursor += bytes;


		////Store Vector of Indexes
		//cursor += bytes;
		//bytes = sizeof(std::vector<uint>)*Index.size();
		//memcpy(cursor, &Index, bytes);

		////Store Vector of struct with Vertices/Normals/TextCords
		
	

		//memcpy(cursor, Vertex.data(), bytes);
		
	/*	for (auto t = Vertex.begin(); t != Vertex.end();++t) {
			cursor += bytes;
			bytes = sizeof(CronosVertex);
			CronosVertex ToData = *t;
			CronosVertex* pt = new CronosVertex(*t);
			CronosVertex* Now = new CronosVertex();
			memcpy(cursor, pt, bytes);
			memcpy(Now, cursor, bytes);
			bool a = true;
		}*/
		//for (auto t = Index.begin(); t != Index.end(); ++t) {
		//	cursor += bytes;
		//	bytes = sizeof(uint);
		//	uint* ToData = new uint(*t);
		//	memcpy(cursor, ToData, bytes);
		//	uint*temp = malloc(sizeof(uint));
		//}
		//uint ranges2[2];
		//uint bytes2 = sizeof(ranges2);
		//memcpy(ranges2, cursor, bytes2);

		//std::vector<CronosVertex>Vertex2(ranges2[0]);
		//cursor += bytes2;
		//bytes2 = sizeof(CronosVertex)*ranges2[0];
		//memcpy(&Vertex2[0], cursor, bytes2);

		//uint *Index2 = &Index[0];
		//cursor += bytes;
		//bytes = sizeof(uint)*range[1];
		////memcpy(cursor, Index2, bytes);
		//std::copy(Index.begin(), Index.end(), cursor);
		//std::vector<uint>vec(cursor, cursor + range[1]);


		//cursor += bytes;
		//bytes = sizeof(CronosVertex)*range[0];
		//memcpy(cursor,&Vertex, bytes);



		//std::copy(Vertex.begin(), Vertex.end(), cursor);
		//std::vector<CronosVertex>vec2(cursor, cursor + range[0]);

		//std::vector<CronosVertex>vec2(cursor, cursor + range[0]);
		//CronosVertex*VertexTest = &Vertex[0];
		////std::vector<CronosVertex>*VertexTest = new std::vector<CronosVertex>(Vertex);
		//cursor += bytes;
		//bytes = sizeof(CronosVertex)*range[0];
		//memcpy(cursor, VertexTest, bytes);


		std::ofstream MeshData{filePath, std::ios::binary };
		MeshData.write(Data, totalSize);
		MeshData.close();


		SDL_free(Data);
		//SDL_free(cursor);
		//cursor = nullptr;

		
		//delete Index2;
		//delete VertexTest;
		//Data = nullptr;
		//cursor = nullptr;
		//tempVertex=nullptr;
		//tempIndex = nullptr;
	
		//char*Data2
	}

	bool Save(GameObject* ToConvert)
	{
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
		aux_JSONFile["Name"] = ToConvert->GetName().c_str();
		aux_JSONFile["Path"] = ToConvert->GetPath().c_str();
		aux_JSONFile["ID"] = ToConvert->GetGOID();
		aux_JSONFile["Active"] = ToConvert->isActive();
			
		TransformComponent* Transform = ToConvert->GetComponent<TransformComponent>();
		aux_JSONFile["ComponentTransform"]["Position"][0] = Transform->GetPosition().x;
		aux_JSONFile["ComponentTransform"]["Position"][1] = Transform->GetPosition().y;
		aux_JSONFile["ComponentTransform"]["Position"][2] = Transform->GetPosition().z;
		aux_JSONFile["ComponentTransform"]["Rotation"][0] = Transform->GetRotation().x;
		aux_JSONFile["ComponentTransform"]["Rotation"][1] = Transform->GetRotation().y;
		aux_JSONFile["ComponentTransform"]["Rotation"][2] = Transform->GetRotation().z;
		aux_JSONFile["ComponentTransform"]["Scale"][0] = Transform->GetScale().x;
		aux_JSONFile["ComponentTransform"]["Scale"][1] = Transform->GetScale().y;
		aux_JSONFile["ComponentTransform"]["Scale"][2] = Transform->GetScale().z;

		if (ToConvert->GetComponent<MaterialComponent>()) {
			std::unordered_map<TextureType, Texture*>m_TexturesContainer= ToConvert->GetComponent<MaterialComponent>()->GetTextures();
			aux_JSONFile["ComponentMaterial"]["Albedo"] = m_TexturesContainer[TextureType::DIFFUSE]->GetTexturePath();
			if(m_TexturesContainer[TextureType::SPECULAR]!=nullptr)
				aux_JSONFile["ComponentMaterial"]["Specular"] = m_TexturesContainer[TextureType::SPECULAR]->GetTexturePath();		
		}
		if (ToConvert->GetComponent<MeshComponent>()) {
			MeshComponent* mesh = ToConvert->GetComponent<MeshComponent>();
			aux_JSONFile["ComponentMesh"]["VertexSize"] = mesh->GetVertexVector().size();
			aux_JSONFile["ComponentMesh"]["IndexSize"] = mesh->GetIndexVector().size();
			std::string Mesh_Path = App->filesystem->GetMeshLib();
			Mesh_Path += std::to_string(ToConvert->GetGOID())+".mesh";
			aux_JSONFile["ComponentMesh"]["MeshPath"] = Mesh_Path.c_str();
			saveData(mesh,Mesh_Path.c_str());

		}
		if (ToConvert->GetParentGameObject() != nullptr) {
			aux_JSONFile["ParentID"] = ToConvert->GetParentGameObject()->GetGOID();
		}
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
		std::ofstream OutputFile_Stream{ filePath,std::ofstream::out };

		OutputFile_Stream << std::setw(2) << aux_JSONFile;

		OutputFile_Stream.close();

		return true;
	}

	bool Filesystem::SaveOwnFormat(GameObject* RootGameObject) 
	{
		Save(RootGameObject);

		for (auto& childs : RootGameObject->m_Childs) {
			SaveOwnFormat(childs);
		}
		return true;
	}

	
	std::vector<CronosVertex>GetVertexVector(std::ifstream& a,int size)
	{
		std::vector<CronosVertex> testing(size);

		std::string Vertex;
		std::vector<float>Values;
		int position = a.tellg();
		int index=0;
		int offset;
		bool start = false;
		while (!a.eof()) {
			getline(a, Vertex);
			if (!(offset = Vertex.find("EndVertex;", 0) != std::string::npos)) {

				if ((offset = Vertex.find("Vertices:", 0) != std::string::npos)) {
					start = true;
					a.seekg(1, std::ios::cur);
					getline(a, Vertex);
				}
				if (start) {
					float tempvalue;
					std::stringstream ss(Vertex);
					while (ss >> tempvalue) {
						Values.push_back(tempvalue);
					}
					if (index == 158) {
						bool awdadsz = true;
					}
					CronosVertex temp(Values);
					testing[index] = temp;
					++index;			
					Values.clear();
					ss.clear();
				}
			}
			else
				return testing;
		}
		return testing;
	}

	bool Filesystem::LoadMesh(const char* filepath, MeshComponent& mesh) {
		
		bool ret = false;

		char*data2 = nullptr;
		std::ifstream file{ filepath, std::ios::binary };

		char* cursor2 = nullptr;
		if (file.is_open()) {

			file.seekg(0, file.end);
			uint size = file.tellg();
			//Cursor to beggining;
			file.seekg(0);

			if (size > 0) {
				data2 = new char[size];
				file.read(data2, size);

			}
			cursor2 = (char*)data2;

			uint ranges[1];

			uint bytes2 = sizeof(ranges);
			memcpy(ranges, cursor2, bytes2);
			mesh.BufferSize[0] = ranges[0];
			cursor2 += bytes2;
			
			bytes2 = sizeof(float) * 3 * ranges[0];
			mesh.Position = new float[ranges[0] * 3];
			memcpy(mesh.Position, cursor2, bytes2);
			cursor2+=bytes2;

			bool a = true;
		}
		else
			LOG("FileSystem error loading file %s");
		
		//Vertices = std::vector<CronosVertex>(*tempVertex);
		
		//std::vector<CronosVertex>asdasef(*tempVertex);
		SDL_free(data2);
		cursor2 = nullptr;
		return ret;
	}

	GameObject* Filesystem::Load(int GOID) {

		std::string filename = m_LibraryPath + std::to_string(GOID) + ".model";
		bool exists = std::filesystem::exists(filename);
		
		if (exists) {
			std::ifstream file{ filename.c_str() };
			
			if (file.is_open()) {

				json configFile = json::parse(file);
				int id = GOID;
				std::string name = configFile["Name"].get<std::string>();
				std::string Path = configFile["Path"].get<std::string>();
				if(configFile.contains("ParentID"))
					int ParentID = configFile["ParentID"].get<int>();
				bool Active = configFile["Active"].get<bool>();
				glm::vec3 position = glm::vec3(configFile["ComponentTransform"]["Position"][0],
											   configFile["ComponentTransform"]["Position"][1],
											   configFile["ComponentTransform"]["Position"][2]);
				glm::vec3 Rotation = glm::vec3(configFile["ComponentTransform"]["Rotation"][0],
											   configFile["ComponentTransform"]["Rotation"][1],
											   configFile["ComponentTransform"]["Rotation"][2]);
				glm::vec3 Scale = glm::vec3(configFile["ComponentTransform"]["Scale"][0],
										    configFile["ComponentTransform"]["Scale"][1],
										    configFile["ComponentTransform"]["Scale"][2]);

				GameObject* Ret = new GameObject(name, id, Path, Active, position, Rotation, Scale);
				if (configFile.contains("ComponentMesh")) {
					if (configFile.contains("ComponentMesh"))
					{
						MeshComponent* MeshComp = ((MeshComponent*)(Ret->CreateComponent(ComponentType::MESH)));
						std::string MeshPath = configFile["ComponentMesh"]["MeshPath"].get < std::string >();
						std::vector<CronosVertex>VertexVector;
						std::vector<uint>IndexVector;
						LoadMesh(MeshPath.c_str(), *MeshComp);
						MeshComp->SetupMesh(VertexVector, IndexVector);
						Ret->m_Components.push_back(MeshComp);
					}
					if (configFile.contains("ComponentMaterial"))
					{

						//MaterialComponent* MatComp = (MaterialComponent*)(Ret->CreateComponent(ComponentType::MATERIAL));
						//Texture* texTemp = 
						//for (uint i = 1; i < (uint)TextureType::MAX_TEXTURES; i++)
						//	MatComp->SetTexture(LoadTextures(AssimpMaterial, TextureType(i), GO->GetPath()), TextureType(i));

						//matComp->SetShader(App->scene->BasicTestShader);
						//GO->m_Components.push_back(matComp);
					}
				}
				if (configFile.contains("Childs"))
				{
					int numOfChilds = configFile["Childs"]["NumberChilds"].get<int>();
					for (int i = 0; i < numOfChilds; ++i) {
						int ChildID = configFile["Childs"]["IDChild"][i].get<int>();					
						Ret->m_Childs.push_back(Load(ChildID));
					}
				}
				
				return Ret;
			}
			else {
				CRONOS_WARN(file.is_open(), "Unable to Open file to load!");
				file.close();
				return nullptr;
			}
		}
		else {
			CRONOS_WARN(!exists, ("Filepath %s doesn`t exist", filename.c_str()));
		}
		return nullptr;
	}

	std::vector<uint>GetIndexVector(std::ifstream&a, int size) {

		std::vector<uint> ret(size);
		std::vector<float>Values;
		std::string indexes;
		int position = a.tellg();
		int index = 0;
		int offset;
		bool start = false;
		while (!a.eof()) {
			getline(a, indexes);
			if (!(offset = indexes.find("EndNode;", 0) != std::string::npos)) {

				if ((offset = indexes.find("Indexes:", 0) != std::string::npos)) {
					start = true;
					a.seekg(1, std::ios::cur);
					getline(a, indexes);
				}
				if (start) {
					float tempvalue;
					std::stringstream ss(indexes);
					while (ss >> tempvalue) {
						ret[index] = tempvalue;
						++index;
					}
					Values.clear();
					ss.clear();
				}
			}
			else
				return ret;
		}
		return ret;
	}

	void LoadGameObjects(GameObject* Parent,std::ifstream& a ) {
		
		GameObject* Child;
		int offset;
		int VertexSize=0;
		int IndexSize=0;
		int childs=0;
		bool SetupMesh = false;
		bool SetupMaterial = false;
		std::string atest;
		int position = a.tellg();
		//while (std::getline(a, line)) {
		//	atest += line;
		//}
		if (a.is_open()) {
			std::string name;
			std::string path;
			int id;
			while (!a.eof())
			{
				getline(a, atest);
				if (!(offset = atest.find("EndNode;", 0) != std::string::npos)) {

					if ((offset = atest.find("Childs:", 0)) != std::string::npos) {

						int offset1 = atest.find_first_of(":") + 1;
						int offset2 = atest.find(";") - offset1;
						childs = std::stoi(atest.substr(offset1, offset2).c_str());
					}

					if ((offset = atest.find("VertexSize:", 0)) != std::string::npos) {

						int offset1 = atest.find_first_of(":") + 1;
						int offset2 = atest.find(";") - offset1;

						std::string testing = atest.substr(offset1, offset2);
						VertexSize= std::stoi(atest.substr(offset1, offset2).c_str());
					}
					if ((offset = atest.find("IndexSize:", 0)) != std::string::npos) {

						int offset1 = atest.find_first_of(":") + 1;
						int offset2 = atest.find(";") - offset1;

						std::string testing = atest.substr(offset1, offset2);
						IndexSize = std::stoi(atest.substr(offset1, offset2).c_str());
					}
					if ((offset = atest.find("Name:", 0)) != std::string::npos) {

						int offset1 = atest.find_first_of(":") + 1;
						int offset2 = atest.find(";") - offset1;
						name = atest.substr(offset1, offset2).c_str();
					}
					if ((offset = atest.find("Path:", 0)) != std::string::npos) {

						int offset1 = atest.find_first_of(":") + 1;
						int offset2 = atest.find(";") - offset1;
						path = atest.substr(offset1, offset2).c_str();
					}
					if ((offset = atest.find("ID:", 0)) != std::string::npos) {

						int offset1 = atest.find_first_of(":") + 1;
						int offset2 = atest.find(";") - offset1;
						id = std::stoi(atest.substr(offset1, offset2).c_str());
					}
					if ((offset = atest.find("MeshComponent", 0)) != std::string::npos) {
						SetupMesh = true;
					}
				}
				else {
					Child = new GameObject(name, id, path);
					Child->SetParent(Parent);
					if (SetupMesh) {
						MeshComponent* meshComp = ((MeshComponent*)(Child->CreateComponent(ComponentType::MESH)));
						std::vector<uint> per;
						a.seekg(position, std::ios::beg);		
						std::vector<CronosVertex>VertexVector = GetVertexVector(a, VertexSize);
						std::vector<uint>IndexVector = GetIndexVector(a, IndexSize);
						meshComp->SetupMesh(VertexVector, IndexVector);
						Child->m_Components.push_back(meshComp);
					}
					break;
				}
			}
			for (int i = 0; i < childs; ++i)
				LoadGameObjects(Child, a);
		}
		Parent->m_Childs.push_back(Child);
	}

	GameObject* Load(std::string MetaPath)
	{
		//std::vector<CronosVertex>&vertices
		GameObject* Root;

		std::ifstream a;
		a.open(MetaPath);
		
		int offset;
		int VertexSize=0,IndexSize=0;
		int childs=0;
		bool SetupMesh=false;
		std::ofstream test;
		std::string atest,line;
		//while (std::getline(a, line)) {
		//	atest += line;
		//}
		if (a.is_open()) {
			std::string name;
			std::string path;
			int id;
			while (!a.eof())
			{			
				getline(a, atest);
				if (!(offset = atest.find("EndNode;", 0) != std::string::npos)) {
			
					if ((offset = atest.find("Childs:", 0)) != std::string::npos) {
						
						int offset1 = atest.find_first_of(":") + 1;
						int offset2 = atest.find(";") - offset1;
						childs = std::stoi(atest.substr(offset1, offset2).c_str());
					}

					if ((offset = atest.find("VertexSize:", 0)) != std::string::npos) {

						int offset1 = atest.find_first_of(":") + 1;
						int offset2 = atest.find(";") - offset1;

						std::string testing = atest.substr(offset1, offset2);
						VertexSize = std::stoi(atest.substr(offset1, offset2).c_str());
					}
					if ((offset = atest.find("IndexSize:", 0)) != std::string::npos) {

						int offset1 = atest.find_first_of(":") + 1;
						int offset2 = atest.find(";") - offset1;

						std::string testing = atest.substr(offset1, offset2);
						IndexSize = std::stoi(atest.substr(offset1, offset2).c_str());
					}
					if ((offset = atest.find("Name:", 0)) != std::string::npos) {

						int offset1 = atest.find_first_of(":") + 1;
						int offset2 = atest.find(";") - offset1;
						name = atest.substr(offset1, offset2).c_str() ;
					}
					if ((offset = atest.find("Path:", 0)) != std::string::npos) {

						int offset1 = atest.find_first_of(":") + 1;
						int offset2 = atest.find(";") - offset1;
						path = atest.substr(offset1, offset2).c_str();
					}
					if ((offset = atest.find("ID:", 0)) != std::string::npos) {

						int offset1 = atest.find_first_of(":") + 1;
						int offset2 = atest.find(";") - offset1;
						id = std::stoi(atest.substr(offset1, offset2).c_str());
					}
					if ((offset = atest.find("MeshComponent", 0)) != std::string::npos) {
						SetupMesh = true;
					}
				}
				else {
					Root = new GameObject(name, id, path);
					if (SetupMesh) {
						MeshComponent* meshComp = ((MeshComponent*)(Root->CreateComponent(ComponentType::MESH)));
						std::vector<uint> per;
						a.seekg(0, std::ios::beg);
						std::vector<CronosVertex>VertexVector = GetVertexVector(a, VertexSize);
						std::vector<uint>IndexVector = GetIndexVector(a, IndexSize);
						meshComp->SetupMesh(VertexVector, IndexVector);
						Root->m_Components.push_back(meshComp);
					}
					break;
				}
			}
			for(int i = 0; i<childs;++i)
				LoadGameObjects(Root,a);		
		}
		return Root;
	}

	//bool AssetItems:: HasMeta() const {
	//	std::string test = App->filesystem->GetMetaPath() + m_AssetNameNoExtension.c_str() + ".model";
	//	if (std::filesystem::exists(std::filesystem::path(App->filesystem->GetMetaPath() + m_AssetNameNoExtension.c_str() + ".model")))
	//		return true;
	//	return false;
	//}


	AssetItems::AssetItems(std::filesystem::path m_path,Directories* parentfolder,ItemType mtype): folderDirectory(parentfolder),type(mtype),m_Path(m_path.root_path().string()) {
		
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
		if (m_Extension == ".obj") {
			type = ItemType::ITEM_OBJ;
			m_IconTex = App->filesystem->GetIcon(type);
		}
		else if (m_Extension == ".fbx"||m_Extension == ".FBX") {
			type = ItemType::ITEM_FBX;
			m_IconTex = App->filesystem->GetIcon(type);
		/*	if (!HasMeta()) {
				GameObject*temp = App->filesystem->m_CNAssimp_Importer.LoadModel(m_Path);
				temp->CleanUp();
			}*/
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
		else if (m_Extension == ".jpeg") {
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
		if (ImGui::IsItemClicked()) {
			App->EditorGUI->m_CurrentAssetSelected = this;
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

	Directories::Directories(std::filesystem::path m_Path) : m_Directories(m_Path)
	{
		m_LabelDirectories = m_Directories.string();
	}
	void Directories::Clear() {
	
		m_Container;
	}

	void Filesystem::RenameFile(AssetItems* Asset, const char* newName) {

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

	void Filesystem::CreateNewDirectory(Directories* currentDir,const char* newName) {
	
		std::string tempDirName = currentDir->m_LabelDirectories +"/"+ newName;
		std::filesystem::create_directory(tempDirName);
		Directories* TempDir = new Directories(tempDirName);
		for (auto& a : DirectoriesArray) {
			if (a==currentDir) {
				a->childs.push_back(TempDir);
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
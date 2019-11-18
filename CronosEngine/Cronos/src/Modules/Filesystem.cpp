#include "Providers/cnpch.h"

#include "Filesystem.h"
#include "Application.h"
#include "TextureManager.h"
#include "PhysFS/include/physfs.h"

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

		m_RootDirectory = std::filesystem::current_path();
		m_LabelRootDirectory = m_RootDirectory.filename().string();
		m_AssetRoot = LoadCurrentDirectories(m_RootDirectory); 
		m_LibraryPath = "lib";

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

	char* convertToData(GameObject* ToConvert)
	{
		std::string Data;
		//RootInfo
		
		Data += "Name:"+ToConvert->GetName()+";\n";
		Data += "Path:"+ToConvert->GetPath()+ ";\n";
		Data += "ID:" + std::to_string(ToConvert->GetGOID()) + ";\n";
		if (ToConvert->m_Childs.size() > 0) {
			Data += "Childs:" + std::to_string(ToConvert->m_Childs.size()) +";\n";
		}
		if (ToConvert->GetParentGameObject() != nullptr) {
			Data += "ParentName:" + ToConvert->GetParentGameObject()->GetName() + ";\n";
		}
		Data += "Components:\n";
		Data += "TransformComponent;\n";
		if (ToConvert->GetComponent<MaterialComponent>())
			Data += "MaterialComponent;\n";
		if (ToConvert->HasVertices) {	
			Data += "MeshComponent\n";
			std::vector<CronosVertex>vertexArray = ToConvert->GetComponent<MeshComponent>()->GetVertexVector();
			Data += "Size:" + std::to_string(vertexArray.size()) + "; " + "\nVertices:\n";
			for (auto&a : vertexArray) {
				Data += std::to_string(a.Position.x) + "," + std::to_string(a.Position.y) + "," + std::to_string(a.Position.z) + "," +
					std::to_string(a.Normal.x) + "," + std::to_string(a.Normal.y) + "," + std::to_string(a.Normal.z) + "," +
					std::to_string(a.TexCoords.x) + "," + std::to_string(a.TexCoords.y) + ",\n";
			}
		}
	
		for (auto&child : ToConvert->m_Childs) {
			Data+="\nEndNode;\n";
			Data += convertToData(child);
		}
		char* Test = new char[Data.length() + 1];
		strcpy(Test, Data.c_str());
		return Test;
	}

	bool Filesystem::SaveOwnFormat(GameObject* RootGameObject) 
	{

		if (RootGameObject->GetMetaPath().size()<=0) {
			RootGameObject->SetMeta("/" +RootGameObject->GetName()+".model");
		}
		char* Data = convertToData(RootGameObject);
		//std::filesystem::path meta = RootGameObject->GetMetaPath().c_str();

		std::ofstream ToSave{RootGameObject->GetMetaPath().c_str() };
		ToSave << Data << std::endl;

		SDL_free(Data);

		return true;
	}

	const std::vector<CronosVertex>&verticestest(std::ifstream& a)
	{
		std::vector<CronosVertex> testing;
		CronosVertex toPush;
		std::string Vertex;
		int offset;
		int cursor=0;
		while (!a.eof()) {	
				if(cursor==0)
					getline(a, Vertex);
				
				if (!(offset=Vertex.find("EndNode;",0)!=std::string::npos)) {
					
					if ((offset = Vertex.find(",", 0)) != std::string::npos) {
						int offset1 = (Vertex.find(",") +1)*cursor;
						int offset2 = 8;
						std::string pop;
						switch (cursor)
						{
						case 0:
							pop = Vertex.substr(offset1, offset2);
							toPush.Position.x = std::stof(Vertex.substr(offset1, offset2).c_str());
							cursor++;
							break;
						case 1:
							toPush.Position.y = std::stof(Vertex.substr(offset1, offset2).c_str());
							cursor++;
							break;
						case 2:
							toPush.Position.z = std::stof(Vertex.substr(offset1, offset2).c_str());
							cursor++;
							break;
						case 3:
							pop = Vertex.substr(offset1, offset2);
							toPush.Normal.x = std::stof(Vertex.substr(offset1, offset2).c_str());
							cursor++;
							break;
						case 4:
							toPush.Normal.y = std::stof(Vertex.substr(offset1, offset2).c_str());
							cursor++;
							break;
						case 5:
							toPush.Normal.z = std::stof(Vertex.substr(offset1, offset2).c_str());
							cursor++;
							break;
						case 6:
							toPush.TexCoords.x = std::stof(Vertex.substr(offset1, offset2).c_str());
							cursor++;
							break;
						case 7:
							toPush.TexCoords.y = std::stof(Vertex.substr(offset1, offset2).c_str());
							cursor++;
							break;
						}
						if (cursor >= 8) {
							testing.push_back(toPush);
							cursor = 0;
						}
					}
				}
				else
					break;
		}
		return testing;
	}

	void LoadGameObjects(GameObject* Parent,std::ifstream& a ) {
		
		GameObject* Child;
		int offset;
		int size;
		int childs;
		bool SetupMesh = false;
		std::string atest;
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

					if ((offset = atest.find("Size:", 0)) != std::string::npos) {

						int offset1 = atest.find_first_of(":") + 1;
						int offset2 = atest.find(";") - offset1;

						std::string testing = atest.substr(offset1, offset2);
						size = std::stoi(atest.substr(offset1, offset2).c_str());
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
					if (SetupMesh) {
						Child->CreateComponent(ComponentType::MESH_RENDERER);
						std::vector<uint> per;
						Child->GetComponent<MeshComponent>()->SetupMesh(verticestest(a), per);
					}
					break;
				}
			}
			if (childs > 0)
			{
				LoadGameObjects(Child, a);
			}
		}
		Parent->m_Childs.push_back(Child);
	}

	GameObject* Filesystem::Load(std::string MetaPath)
	{
		//std::vector<CronosVertex>&vertices
		GameObject* Root;

		std::ifstream a;
		a.open(MetaPath);
		
		int offset;
		int size;
		int childs;
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

					if ((offset = atest.find("Size:", 0)) != std::string::npos) {

						int offset1 = atest.find_first_of(":") + 1;
						int offset2 = atest.find(";") - offset1;

						std::string testing = atest.substr(offset1, offset2);
						size = std::stoi(atest.substr(offset1, offset2).c_str());
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
						Root->CreateComponent(ComponentType::MESH_RENDERER);
						std::vector<uint> per;
						Root->GetComponent<MeshComponent>()->SetupMesh(verticestest(a), per);
					}
					break;
				}
			}
			if (childs > 0) 
			{
				for(int i = 0; i<=childs;++childs)
					LoadGameObjects(Root,a);
			}
		}
		return Root;
	}




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
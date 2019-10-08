#include "Providers/cnpch.h"

#include "Filesystem.h"
#include "Application.h"
#include "imgui.h"

namespace Cronos {

	Filesystem::Filesystem(Application* app, bool start_enabled) : Module(app, "Module Filesystem", start_enabled) {

	}

	bool Filesystem::OnStart() {
		m_RootDirectory = std::filesystem::current_path();
		m_LabelRootDirectory = m_RootDirectory.filename().string();
		m_AssetRoot = LoadCurrentDirectories(m_RootDirectory);

		return true;
	}

	AssetItems::AssetItems(std::filesystem::path m_path,ItemType mtype): type(mtype),m_Path(m_path.string()) {
		m_AssetFullName = m_path.filename().string();
		sprintf_s(labelID,"%s", m_AssetFullName.c_str());
		m_AssetNameNoExtension = m_AssetShortName = m_AssetFullName;
		
		if (m_AssetFullName.length() > 10) {
			m_AssetShortName.erase(10);
			m_AssetShortName += "...";
		}
		if (m_path.has_extension()) {
			m_Extension = m_path.extension().string();
			m_AssetNameNoExtension.erase(m_AssetNameNoExtension.find(m_Extension));
		}
		if (m_Extension == "obj") {
			type = ItemType::ITEM_OBJ;
		}
		else if (m_Extension == "fbx") {
			type = ItemType::ITEM_FBX;
		}
		else if (m_Extension == "cpp" || m_Extension == "h") {
			type = ItemType::ITEM_SCRIPT;

		}
		////else {
		////	TextPath = "res/Icons/Shader_Icon.png";
		////}
	};
	void AssetItems::Clear() {
		delete folderDirectory;
	}

	void AssetItems::DrawIcons()
	{

		ImGui::BeginGroup();     
		
		if (ImGui::ImageButton(labelID, ImVec2(50, 50),ImVec2(0,0), ImVec2(0, 0),2)) {
			bool a = true;
		} 
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
				
			
			/*	while (ImGui::MenuItem("Renameit")) {
					static char buf1[64] = { (char)m_AssetNameNoExtension.c_str() }; ImGui::InputText("###", buf1, 64, ImGuiInputTextFlags_CharsNoBlank);
					ImGui::InputText("###", buf1, 64, ImGuiInputTextFlags_CharsNoBlank);
					App->filesystem->RenameFile(this, "Hello");
				}*/
				ImGui::EndMenu();
			}
			if (ImGui::MenuItem("Delete"))
			{
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
						
						AssetItems* t = new AssetItems(path.path().string().c_str(),ItemType::ITEM_FOLDER);
						t->folderDirectory = newPath;
						currentDir->m_Container.push_front(t);
						newPath->SetParentDirectory(currentDir);
						currentDir = newPath;
					}
					else if (p.depth() > LastDepth) {
						currentDir->childs.push_back(newPath);

						AssetItems* t = new AssetItems(path.path().string().c_str(),ItemType::ITEM_FOLDER);
						t->folderDirectory = newPath;
						currentDir->m_Container.push_front(t);
						newPath->SetParentDirectory(currentDir);
						currentDir = newPath;

					}

					LastDepth = p.depth();
					DirectoriesArray.push_back(newPath);
					
				}
				else {
					AssetItems* t = new AssetItems(path.path().string().c_str());
					currentDir->m_Container.push_back(t);
					AssetArray.push_back(t);
				}
			}
		}
		return SolutionDirTemp;
	}


}
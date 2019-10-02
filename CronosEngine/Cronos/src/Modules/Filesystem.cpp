#include "cnpch.h"
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

	AssetItems::AssetItems(std::filesystem::path m_Path) {
		m_Elements = m_Path.filename().string();
		if (m_Path.has_extension()) {
			m_Extension = m_Path.extension().string();
		}
		else {
			type = ItemType::ITEM_FOLDER;
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

	void AssetItems::DrawIcons()
	{

		ImGui::BeginGroup();
		ImGui::Image("", ImVec2(50, 50));
	
		ImGui::Text(m_Elements.c_str());
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
						
						AssetItems* t = new AssetItems(path.path().string().c_str());
						t->folderDirectory = newPath;
						currentDir->m_Container.push_back(*t);
						newPath->SetParentDirectory(currentDir);
						currentDir = newPath;
					}
					else if (p.depth() > LastDepth) {
						currentDir->childs.push_back(newPath);

						AssetItems* t = new AssetItems(path.path().string().c_str());
						t->folderDirectory = newPath;
						currentDir->m_Container.push_back(*t);
						newPath->SetParentDirectory(currentDir);
						currentDir = newPath;

					}

					LastDepth = p.depth();
					DirectoriesArray.push_back(newPath);
					
				}
				else {
					currentDir->m_Container.push_back(path.path());
				}
			}
		}
		return SolutionDirTemp;
	}


}
#ifndef _FILESYSTEM_H_
#define _FILESYSTEM_H_

#include "Module.h"
#include "Providers/Globals.h"
#include "imgui.h"
#include "Renderer/Textures.h"


namespace Cronos {

	class Directories;

	enum class ItemType
	{
		ITEM_NONE = 0,
		ITEM_FBX,
		ITEM_OBJ,
		ITEM_MATERIAL,
		ITEM_SHADER,
		ITEM_SCRIPT,
		ITEM_FOLDER,
		ITEM_TEXTURE_PNG,
		ITEM_TEXTURE_TGA,
		ITEM_TEXTURE_JPEG,
		MAX_ITEMS
	};

	class AssetItems {
	public:
		AssetItems(std::filesystem::path m_path,ItemType mtype=ItemType::ITEM_NONE);

		ItemType type = ItemType::ITEM_NONE;

		std::string m_AssetShortName;
		std::string m_AssetFullName;
		std::string m_AssetNameNoExtension;

		void Clear();
		virtual void DrawIcons();

		int GetElementSize();
		ItemType GetType() const { return type; }
		uint GetIconTexture() const { return m_IconTex; }
		ImVec2 GetResolution() const { return m_Resolution; }


		void SetAssetPath(std::string newPath) { m_Path = newPath; }
		std::string GetAssetPath() const { return m_Path; }
		std::string GetExtension() const { return m_Extension; }
		std::string GetDetails() const { return m_Details; }

		Directories* folderDirectory = nullptr;

	private:
		std::string m_Extension;
		int m_ElementSize;
		char labelID[150];
		bool hovered;
		std::string m_Path;
		std::string m_Details;
		//For Images
		GLuint m_IconTex;
		ImVec2 m_Resolution;
	};

	class Directories {

	public:

		Directories() {};
		Directories(std::filesystem::path m_Path);
		void Clear();
		void SetParentDirectory(Directories* parent) { parentDirectory = parent; }

		int m_DepthID;
		int m_ID;

		std::filesystem::path m_Directories;
		std::string m_LabelDirectories;

		std::list<AssetItems*> m_Container;
		std::list<Directories*>childs;

		inline Directories* GetParentDirectory() const { return parentDirectory; }

	private:

		Directories* parentDirectory = nullptr;
	};


	class Filesystem : public Module
	{
	public:

		Filesystem(Application* app, bool start_enabled = true) ;
		~Filesystem() {};

		virtual bool OnStart() override;
		virtual bool OnCleanUp() override;

	public:

		Directories *LoadCurrentDirectories(std::filesystem::path filepath);
		void UpdateDirectories();
		void CreateNewDirectory(Directories* currentDir, const char* newName);
		void DeleteDirectory(const char* path);
		void RenameFile(AssetItems* Asset, const char* newName);
		void SearchFile(Directories* tempDir,const char* name);

		inline Directories* GetAssetDirectories() const { return m_AssetRoot; };
		inline std::string GetLabelAssetRoot() const { return m_LabelRootDirectory; }
		inline GLuint GetIcon(ItemType type) const { return ArrayIconTextures[(int)type]->GetTextureID(); }

	private:
		std::vector <AssetItems*> AssetArray;
		std::vector <Directories*> DirectoriesArray;
		std::filesystem::path m_RootDirectory; //Temporary
		std::string m_LabelRootDirectory;

		Directories* m_AssetRoot = nullptr;
		Texture* ArrayIconTextures[(int)ItemType::MAX_ITEMS];

	};

}
#endif // !_FILESYSTEM_H_

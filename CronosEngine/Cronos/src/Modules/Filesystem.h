#ifndef _FILESYSTEM_H_
#define _FILESYSTEM_H_

#include "Module.h"
#include "Providers/Globals.h"

namespace Cronos {

	enum class ItemType
	{
		ITEM_NONE = 0,
		ITEM_FBX,
		ITEM_OBJ,
		ITEM_MATERIAL,
		ITEM_SHADER,
		ITEM_SCRIPT,
		ITEM_FOLDER,
		ITEM_TEXTURE
	};

	class AssetItems {
	public:
		AssetItems(std::filesystem::path m_Path);

		ItemType type = ItemType::ITEM_NONE;
		std::string m_Elements;
		void DrawIcons();

		int GetElementSize();

		//temporal
		std::string TextPath;

	private:
		std::string m_Extension;
		int m_ElementSize;
	};

	class Directories {
	public:

		Directories(std::filesystem::path m_Path);

		int m_DepthID;
		int m_ID;

		std::filesystem::path m_Directories;
		std::string m_LabelDirectories;

		std::vector<AssetItems> m_Container;
		std::list<Directories*>childs;

	};


	class Filesystem : public Module
	{
	public:
		Filesystem(Application* app, bool start_enabled = true) ;
		~Filesystem() {};
		
		virtual bool OnStart() override;

		Directories *LoadCurrentDirectories(std::filesystem::path filepath);

		inline Directories* GetAssetDirectories() const { return m_AssetRoot; };
		inline std::string GetLabelAssetRoot() const { return m_LabelRootDirectory; }

	private:

		std::vector <Directories*> DirectoriesArray;
		std::filesystem::path m_RootDirectory; //Temporary
		std::string m_LabelRootDirectory;

		Directories* m_AssetRoot;

	};

}
#endif // !_FILESYSTEM_H_

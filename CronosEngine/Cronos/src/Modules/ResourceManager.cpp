#include "Providers/cnpch.h"

#include "Core/Application.h"

#include "Filesystem.h"

#include "ResourceManager.h"
#include "Resources/Resource.h"
#include "Resources/ResourceMesh.h"

namespace Cronos {


	ResourceManager::ResourceManager(Application * app, bool start_enabled) : Module (app,"Module ResourceManager",start_enabled )
	{

	}

	void ResourceManager::AddResource(Resource* res)
	{
		m_ResourcesIDList.push_back(res->GetResID());
		m_ResourceList.push_back(res);
	}

	bool ResourceManager::isMeshLoaded(uint resID)
	{
		for (int i = 0; i < m_ResourcesIDList.size(); i++) {
			if (m_ResourcesIDList[i] == resID)
				return true;
		}
		return false;
	}

	bool ResourceManager::isMeshSaved(uint resID)
	{
		std::string filepath = App->filesystem->GetMeshLib();
		filepath += std::to_string(resID) + ".mesh";

		if (std::filesystem::exists(filepath))
			return true;

		return false;
	}

	bool ResourceManager::isMaterialLoaded(uint resID)
	{
		for (int i = 0; i < m_ResourcesIDList.size(); i++) {
			if (m_ResourcesIDList[i] == resID)
				return true;
		}

		return false;
	}

	bool ResourceManager::isMaterialSaved(uint resID)
	{
		std::string filepath = App->filesystem->GetMatLib();
		filepath += std::to_string(resID) + ".material";

		if (std::filesystem::exists(filepath))
			return true;

		return false;
	}

	bool ResourceManager::saveResources()
	{
		for (auto& res : m_ResourceList)
		{
			if (res->type == ResourceType::MATERIAL)
				App->filesystem->SaveMaterial((((ResourceMaterial*)(res))->m_Material), res->GetPath().c_str());
		}
		return false;
	}

	ResourceMesh* ResourceManager::getMeshResource(uint resID)
	{
		for (auto& res : m_ResourceList)
		{
			if (res->GetResID() == resID)
				return (ResourceMesh*)res;
		}

		return nullptr;
	}
	ResourceMaterial *ResourceManager::getMaterialResource(uint resID)
	{
		for (auto& res : m_ResourceList)
		{
			if (res->GetResID() == resID)
				return (ResourceMaterial*)res;
		}
		return (ResourceMaterial*)m_ResourceList[0];//SetDefaultShader if shader not found;
	}
}
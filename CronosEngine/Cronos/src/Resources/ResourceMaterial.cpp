#include "Providers/cnpch.h"
#include "ResourceMaterial.h"

namespace Cronos {


	ResourceMaterial::ResourceMaterial(uint ResID,Material* mat) :Resource(ResID, ResourceType::MATERIAL)
	{
		m_ResID = ResID;
		m_Material = mat;
	}

	bool ResourceMaterial::isLoaded(uint m_ResID) const 
	{
		return true;
	}

	bool ResourceMaterial::Exists()
	{
		return true;
	}
}

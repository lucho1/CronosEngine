#ifndef _RESOURCE_MATERIAL_H
#define _RESOURCE_MATERIAL_H

#include "Resource.h"
#include "Providers/Globals.h"
#include "GameObject/Components/MaterialComponent.h"
#include "Renderer/Material.h"

namespace Cronos {

	class ResourceMaterial : public Resource {
	public:

		ResourceMaterial(uint ResID,Material* mat);
		//~ResourceMesh();

		virtual bool isLoaded(uint m_ResID) const override;
		virtual bool Exists() override;

		inline uint GetResID() const override { return m_ResID; }
		Material* m_Material;

	private:

		uint m_ResID;
	
	};
}


#endif // !_RESOURCE_MATERIAL_H

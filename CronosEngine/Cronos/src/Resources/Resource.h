#ifndef _RESOURCE_H
#define _RESOURCE_H
#include "Providers/Globals.h"

namespace Cronos {

	enum ResourceType {
		NONE = -1,
		MESH,
		TEXTURE
	};

	class Resource {
	public:

		Resource(uint ResID, ResourceType type);
		//virtual ~Resource();

		virtual bool isLoaded(uint m_ResID) const = 0 ;
		virtual bool Exists()=0;
		virtual inline uint GetResID() const { return m_ResID; };
		
		//template <typename T>
		//T* GetResource()
		//{
		//	ResourceType type = T::GetType();
		//	for (auto& comp : m_Components)
		//		if (comp->GetComponentType() == type)
		//			return ((T*)(comp));

		//	//LOG("Component %i in %s Game Object NOT Found!", (int)type, m_Name.c_str());
		//	return nullptr;
		//}
		//std::vector<Resource>m_Resources;

	private:

		uint m_ResID;
		
	};

}


#endif // !_RESOURCE_H

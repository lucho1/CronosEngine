#ifndef _RESOURCE_H
#define _RESOURCE_H
#include "Providers/Globals.h"

namespace Cronos {

	enum Type {
		NONE = -1,
		MESH = 0,
		TEXTURE
	};

	class Resource {
		Resource(uint GOID, Type type);
		~Resource();

		virtual bool isLoaded()=0;
		virtual bool Exists()=0;

	private:
		uint m_GOID;
		
	};

}


#endif // !_RESOURCE_H

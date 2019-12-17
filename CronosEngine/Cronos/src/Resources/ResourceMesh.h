#ifndef _RESOURCE_MESH_H
#define _RESOURCE_MESH_H

#include "Resource.h"
#include "Providers/Globals.h"
#include "GameObject/Components/MeshComponent.h"

namespace Cronos {

	class ResourceMesh : public Resource {
	public:

		ResourceMesh(uint ResID);
		//~ResourceMesh();

		virtual bool isLoaded(uint m_ResID) const override;
		virtual bool Exists() override;
		
		//void setBufferSize(uint BufferSize[2]);	
		//uint GetBufferSize(uint arr[]);
		void toCronosVertexVector();

		inline std::vector<CronosVertex> getVector() const { return m_VertexVector; };
		inline std::vector<uint> getIndex() const { return m_IndicesVector; };

		uint* Index = nullptr;
		float*Position = nullptr;
		float*Normal = nullptr;
		float*TextureV = nullptr;

		uint m_BufferSize[4];

		inline uint GetResID() const override { return m_ResID; }

	private:

		
		uint m_ResID;
		std::vector<CronosVertex> m_VertexVector;
		std::vector<uint> m_IndicesVector;
	};
}


#endif // !_RESOURCE_MESH_H

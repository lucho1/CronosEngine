#include "ResourceMesh.h"
#include "Providers/cnpch.h"
#include "Application.h"
#include "Modules/Filesystem.h"

namespace Cronos {


	ResourceMesh::ResourceMesh(uint ResID):Resource(ResID,ResourceType::MESH)
	{
		m_ResID = ResID;
	}

	bool ResourceMesh::isLoaded(uint m_ResID) const
	{
		return false;
	}
	bool ResourceMesh::Exists()
	{
		return false;
	}
	void ResourceMesh::setBufferSize(uint BufferSize[2])
	{
		for (int i = 0; i < 2; ++i) {
			m_BufferSize[i] = BufferSize[i];
		}
		Position = new float[m_BufferSize[0] * 3];
		Normal = new float[m_BufferSize[0] * 3];
		TextureV = new float[m_BufferSize[0] * 2];
		Index = new uint[m_BufferSize[1] * 3] ;
	}

	//uint ResourceMesh::GetBufferSize(uint arr[])
	//{
	//	for (int i = 0; i < 2; ++i) {
	//		arr[i] = m_BufferSize[i];
	//	}
	//	return 0;
	//}
}
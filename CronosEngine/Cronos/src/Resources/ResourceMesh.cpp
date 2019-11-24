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

	void ResourceMesh::toCronosVertexVector() {

		for (int i = 0; i < m_BufferSize[0]; i++) {
			CronosVertex toPush;			
			toPush.Position = { Position[i*3],Position[i * 3+1],Position[i*3 + 2] };
			toPush.Normal = { Normal[i*3],Normal[i*3 + 1],Normal[i*3 + 2] };
			toPush.TexCoords = { TextureV[i*2],TextureV[i*2 + 1] };

			m_VertexVector.push_back(toPush);
		} 
		for (int i = 0; i < m_BufferSize[1]*3; ++i) {

			m_IndicesVector.push_back(Index[i]);
		}
		//std::vector<uint> test(Index, m_BufferSize[1]);

		m_IndicesVector;
	}

	//void ResourceMesh::setBufferSize(uint BufferSize[2])
	//{
	//	for (int i = 0; i < 2; ++i) {
	//		m_BufferSize[i] = BufferSize[i];
	//	}
	//	Position = new float[m_BufferSize[0] * 3];
	//	Normal = new float[m_BufferSize[0] * 3];
	//	TextureV = new float[m_BufferSize[0] * 2];
	//	Index = new uint[m_BufferSize[1] * 3] ;
	//}

	//uint ResourceMesh::GetBufferSize(uint arr[])
	//{
	//	for (int i = 0; i < 2; ++i) {
	//		arr[i] = m_BufferSize[i];
	//	}
	//	return 0;
	//}
}
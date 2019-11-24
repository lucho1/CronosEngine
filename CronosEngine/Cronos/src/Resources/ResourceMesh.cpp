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
			if(m_BufferSize[1]>0)
				toPush.Normal = { Normal[i*3],Normal[i*3 + 1],Normal[i*3 + 2] };
			if(m_BufferSize[2]>0)
			toPush.TexCoords = { TextureV[i*2],TextureV[i*2 + 1] };

			m_VertexVector.push_back(toPush);
		} 
		for (int i = 0; i < m_BufferSize[3]*3; ++i) {

			m_IndicesVector.push_back(Index[i]);
		}

	}
}
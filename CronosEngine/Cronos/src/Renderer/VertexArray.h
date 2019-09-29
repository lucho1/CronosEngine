#ifndef _VERTEXARRAY_H_
#define _VERTEXARRAY_H_

#include "Buffers.h"

namespace Cronos {

	class VertexArray {

	public:

		VertexArray();
		~VertexArray();

		void Bind() const;
		void UnBind() const;

		void AddVertexBuffer(VertexBuffer& vBuffer);
		void AddIndexBuffer(IndexBuffer& iBuffer);

		const std::vector<VertexBuffer*>& GetVertexBuffers() const { return m_VBuffers_List; }
		IndexBuffer* GetIndexBuffer() const { return m_IBuffer; }
		
		
	private:

		GLenum ConvertToOpenGLDataType(VertexDataType type);

		uint m_ID;
		IndexBuffer* m_IBuffer;
		std::vector<VertexBuffer*> m_VBuffers_List;

	};
}

#endif
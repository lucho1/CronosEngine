#include "cnpch.h"
#include "VertexArray.h"

namespace Cronos {

	VertexArray::VertexArray()
	{
		glCreateVertexArrays(1, &m_ID);
	}

	VertexArray::~VertexArray()
	{
		glDeleteVertexArrays(1, &m_ID);
	}

	void VertexArray::Bind() const
	{
		glBindVertexArray(m_ID);
	}

	void VertexArray::UnBind() const
	{
		glBindVertexArray(0);
	}

	void VertexArray::AddVertexBuffer(VertexBuffer& vBuffer)
	{
		glBindVertexArray(m_ID);
		vBuffer.Bind();

		if (vBuffer.GetLayout().GetLayoutElements().size() > 0) {

			uint i = 0;
			for (const auto& element : vBuffer.GetLayout().GetLayoutElements())
			{
				glEnableVertexAttribArray(i);
				glVertexAttribPointer(i, element.GetTypeCount(), (GLenum)element.bd_ShaderDataType,
					(element.bd_Normalized ? true : false), vBuffer.GetLayout().GetLayoutStride(), (const void*)element.bd_Offset);
				
				i++;
			}

			m_VBuffers_List.push_back(&vBuffer);
		}
	}

	void VertexArray::AddIndexBuffer(IndexBuffer& iBuffer)
	{
		glBindVertexArray(m_ID);
		iBuffer.Bind();
		m_IBuffer = &iBuffer;
	}
}
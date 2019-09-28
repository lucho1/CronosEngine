#include "cnpch.h"
#include "VertexArray.h"

namespace Cronos {

	GLenum OpenGLDataType(ShaderDataType type)
	{
		switch (type)
		{
		case ShaderDataType::FLOAT:			return GL_FLOAT;
		case ShaderDataType::VEC2F:			return GL_FLOAT;
		case ShaderDataType::VEC3F:			return GL_FLOAT;
		case ShaderDataType::VEC4F:			return GL_FLOAT;
		case ShaderDataType::MAT3:			return GL_FLOAT;
		case ShaderDataType::MAT4:			return GL_FLOAT;
		case ShaderDataType::INT:			return GL_INT;
		case ShaderDataType::VEC2I:			return GL_INT;
		case ShaderDataType::VEC3I:			return GL_INT;
		case ShaderDataType::VEC4I:			return GL_INT;
		case ShaderDataType::BOOL:			return GL_BOOL;
		}
	}


	VertexArray::VertexArray()
	{
		glCreateVertexArrays(1, &m_ID);
	}

	VertexArray::~VertexArray()
	{
		delete m_IBuffer;

		for (auto element : m_VBuffers_List)
			delete element;
		m_VBuffers_List.clear();

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
			for (auto& element : vBuffer.GetLayout().GetLayoutElements())
			{
				glEnableVertexAttribArray(i);
				glVertexAttribPointer(i, element.GetTypeCount(), OpenGLDataType(element.bd_ShaderDataType),
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
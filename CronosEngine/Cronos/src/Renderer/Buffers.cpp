#include "cnpch.h"
#include "Buffers.h"

#include <glad/glad.h>

namespace Cronos {

	uint ShaderDataTypeSize(ShaderDataType shader_data_type)
	{
		switch (shader_data_type)
		{
			case ShaderDataType::FLOAT:	return 4;			//sizeof(float) = 4...
			case ShaderDataType::VEC2F:	return 4 * 2;		//...size of 2 floats = 4*2 and so on
			case ShaderDataType::VEC3F:	return 4 * 3;
			case ShaderDataType::VEC4F:	return 4 * 4;
			case ShaderDataType::MAT3:	return 4 * 3 * 3;	//sizeof(float) * 3x3 matrix, so 4*3*3
			case ShaderDataType::MAT4:	return 4 * 4 * 4;
			case ShaderDataType::INT:	return 4;			//sizeof(int) = 4
			case ShaderDataType::VEC2I:	return 4 * 2;
			case ShaderDataType::VEC3I:	return 4 * 3;
			case ShaderDataType::VEC4I:	return 4 * 4;
			case ShaderDataType::BOOL:	return 1;			//sizeof(bool) = 1
		}

		//ASSERT(0, "Unknown Shader DataType!!");
		LOG("Unknown Shader DataType!!");
		return 0;
	}

	//-------------- VERTEX BUFFER ------------------------------------------------
	VertexBuffer::VertexBuffer(float* vertices, uint size)
	{
		//Setting the buffer to generate (an Array one), the buffer size,
		//the data source (the float* vertices), and for what we are using it
		//(to statically draw)
		glCreateBuffers(1, &m_ID);
		glBindBuffer(GL_ARRAY_BUFFER, m_ID);
		glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW);
	}

	VertexBuffer::~VertexBuffer()
	{
		glDeleteBuffers(1, &m_ID);
	}

	void VertexBuffer::Bind() const
	{
		//The type of buffer we are binding
		glBindBuffer(GL_ARRAY_BUFFER, m_ID);
	}

	void VertexBuffer::UnBind() const
	{
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}


	//-------------- INDEX BUFFER ------------------------------------------------
	IndexBuffer::IndexBuffer(uint* indices, uint count) : m_Count(count)
	{
		glCreateBuffers(1, &m_ID);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ID);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(uint), indices, GL_STATIC_DRAW);
	}

	IndexBuffer::~IndexBuffer()
	{
		glDeleteBuffers(1, &m_ID);
	}

	void IndexBuffer::Bind() const
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ID);
	}

	void IndexBuffer::UnBind() const
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}

}
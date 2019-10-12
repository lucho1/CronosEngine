#include "Providers/cnpch.h"
#include "Buffers.h"

#include <glad/glad.h>


namespace Cronos {

	uint BufferData::VertexDataTypeSize(VertexDataType vertex_data_type) const
	{
		switch (vertex_data_type)
		{
			case VertexDataType::FLOAT:	return 4;			//sizeof(float) = 4
			case VertexDataType::INT:	return 4;			//sizeof(int) = 4
			case VertexDataType::VEC2I:	return 4 * 2;		//size of 2 ints = 4*2 and so on
			case VertexDataType::VEC3I:	return 4 * 3;
			case VertexDataType::VEC4I:	return 4 * 4;
			case VertexDataType::VEC2F:	return 4 * 2;		
			case VertexDataType::VEC3F:	return 4 * 3;
			case VertexDataType::VEC4F:	return 4 * 4;
			case VertexDataType::MAT3:	return 4 * 3 * 3;	//sizeof(float) * 3x3 matrix, so 4*3*3
			case VertexDataType::MAT4:	return 4 * 4 * 4;
			case VertexDataType::BOOL:	return 1;			//sizeof(bool) = 1
		}

		CRONOS_ASSERT(0, "Unknown Vertex Data Type!!");
		return 0;
	}

	uint BufferData::VertexDataTypeCount(VertexDataType vertex_data_type) const
	{
		switch (vertex_data_type)
		{
			case VertexDataType::FLOAT:	return 1;
			case VertexDataType::INT:	return 1;
			case VertexDataType::VEC2I:	return 2;
			case VertexDataType::VEC3I:	return 3;
			case VertexDataType::VEC4I:	return 4;
			case VertexDataType::VEC2F:	return 2;
			case VertexDataType::VEC3F:	return 3;
			case VertexDataType::VEC4F:	return 4;
			case VertexDataType::MAT3:	return 3 * 3;
			case VertexDataType::MAT4:	return 4 * 4;
			case VertexDataType::BOOL:	return 1;
		}
		CRONOS_ASSERT(0, "Unknown Vertex Data Type Count!!");
		return 0;
	}

	//-------------- VERTEX BUFFER ------------------------------------------------
	VertexBuffer::VertexBuffer(CronosVertex* vertices, uint size)
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
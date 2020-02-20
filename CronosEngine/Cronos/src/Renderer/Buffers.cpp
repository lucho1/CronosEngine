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

	//-------------- UNIFORM BUFFER ----------------------------------------------
	UniformBuffer::UniformBuffer(uint size, const uint bindingPoint) : m_BindingPoint(bindingPoint)
	{
		glCreateBuffers(1, &m_ID);
		glBindBuffer(GL_UNIFORM_BUFFER, m_ID);
		glBufferData(GL_UNIFORM_BUFFER, size, NULL, GL_STATIC_DRAW);
		glBindBufferBase(GL_UNIFORM_BUFFER, bindingPoint, m_ID);
		
		CRONOS_ASSERT(bindingPoint >= 0, "WARNING! UBO's Binding point is negative!");
	}

	UniformBuffer::~UniformBuffer()
	{
		glDeleteBuffers(1, &m_ID);
	}

	void UniformBuffer::Bind() const
	{
		glBindBufferBase(GL_UNIFORM_BUFFER, m_BindingPoint, m_ID);
	}

	void UniformBuffer::UnBind() const
	{
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
	}

	void UniformBuffer::PassData(const std::string& elementName, const void* data)
	{
		for (auto& element : GetLayout().GetLayoutElements())
		{
			if (element.bd_Name != elementName)
				continue;

			glBufferSubData(GL_UNIFORM_BUFFER, element.bd_Offset, element.bd_Size, data);
		}
	}

	//-------------- SHADER STORAGE BUFFER ---------------------------------------
	ShaderStorageBuffer::ShaderStorageBuffer(uint size, const uint bindingPoint) : m_BindingPoint(bindingPoint), m_BufferSize(size)
	{
		glCreateBuffers(1, &m_ID);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_ID);
		glBufferData(GL_SHADER_STORAGE_BUFFER, size, NULL, GL_DYNAMIC_COPY);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, bindingPoint, m_ID);

		CRONOS_ASSERT((bindingPoint >= 0 || size >= 0), "WARNING! UBO's Binding point is negative!");
	}

	ShaderStorageBuffer::~ShaderStorageBuffer()
	{
		glDeleteBuffers(1, &m_ID);
	}

	void ShaderStorageBuffer::Bind() const
	{
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_ID);
	}

	void ShaderStorageBuffer::UnBind() const
	{
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
	}

	//This completely substitutes the current data in the buffer
	void ShaderStorageBuffer::PassData(uint dataSize, uint dataOffset, const void* data)
	{
		//GLvoid* gpuMemPtr = glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_WRITE_ONLY);
		//memcpy(gpuMemPtr, &data, sizeof(data));
		//glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);

	//	glBufferData(GL_SHADER_STORAGE_BUFFER, dataSize, data, GL_STATIC_DRAW);

		glBufferSubData(GL_SHADER_STORAGE_BUFFER, dataOffset, dataSize, data);

		//glBufferSubData(GL_UNIFORM_BUFFER, 0, dataSize, data);

		//for (auto& element : GetLayout().GetLayoutElements())
		//{
		//	if (element.bd_Name != elementName)
		//		continue;
		//
		//	glBufferSubData(GL_UNIFORM_BUFFER, element.bd_Offset, element.bd_Size, data);
		//	glBufferData(GL_SHADER_STORAGE_BUFFER, 100, &data, GL_STATIC_DRAW);
		//}
	}


	//-------------- FRAME BUFFER ------------------------------------------------
	FrameBuffer::FrameBuffer()
	{
	}

	FrameBuffer::~FrameBuffer()
	{
	}

	void FrameBuffer::Init(int Width, int Height)
	{
		uint renderBufferoutput;
		//Gen And Bind frameBuffer
		glGenFramebuffers(1,&m_FB);
		glBindFramebuffer(GL_FRAMEBUFFER, m_FB);
		//Gen and Bind Text
		glGenTextures(1, &m_Text);
		glBindTexture(GL_TEXTURE_2D, m_Text);
		//Copy Textures
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, Width, Height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glBindTexture(GL_TEXTURE_2D, 0);
		//Texture to FrameBuffers
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_Text, 0);
		//Prepare RenderBuffers
		glGenRenderbuffers(1, &renderBufferoutput);
		glBindRenderbuffer(GL_RENDERBUFFER, renderBufferoutput);
		//Data to renderBuffer
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, Width, Height);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, renderBufferoutput);
		glBindRenderbuffer(GL_RENDERBUFFER, 0);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);

	}

	void FrameBuffer::OnResize(int Width, int Height) 
	{
		uint TempRendBuff;

		glBindFramebuffer(GL_FRAMEBUFFER, m_FB);
		glBindTexture(GL_TEXTURE_2D, m_Text);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, Width, Height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glBindTexture(GL_TEXTURE_2D, 0);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_Text, 0);
		glGenRenderbuffers(1, &TempRendBuff);
		glBindRenderbuffer(GL_RENDERBUFFER, TempRendBuff);

		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, Width, Height);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, TempRendBuff);
		glBindRenderbuffer(GL_RENDERBUFFER, 0);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);

	}
	
	void FrameBuffer::PreUpdate()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, m_FB);
		glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	void FrameBuffer::PostUpdate()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void FrameBuffer::CleanUp()
	{
		glDeleteBuffers(1, &m_FB);
	}
}
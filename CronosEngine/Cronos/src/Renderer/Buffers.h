#ifndef _BUFFERS_H_
#define _BUFFERS_H_

namespace Cronos {

	enum class ShaderDataType { NONE = 0, FLOAT, VEC2F, VEC3F, VEC4F, MAT3, MAT4, INT, VEC2I, VEC3I, VEC4I, BOOL };
	uint ShaderDataTypeSize(ShaderDataType shader_data_type);

	struct BufferData {

		std::string bd_Name;
		bool bd_Normalized;
		uint bd_Size;
		uint bd_Offset;
		ShaderDataType bd_ShaderDataType;

		BufferData() {}
		BufferData(ShaderDataType shader_data_type, const std::string& name, bool norm = false)
			: bd_ShaderDataType(shader_data_type), bd_Name(name), bd_Normalized(norm), bd_Size(ShaderDataTypeSize(shader_data_type)) {}

		uint GetTypeCount() const //How many components has each type
		{
			switch (bd_ShaderDataType)
			{
				case ShaderDataType::FLOAT:	return 1;
				case ShaderDataType::VEC2F:	return 2;
				case ShaderDataType::VEC3F:	return 3;
				case ShaderDataType::VEC4F:	return 4;
				case ShaderDataType::MAT3:	return 3 * 3;
				case ShaderDataType::MAT4:	return 4 * 4;
				case ShaderDataType::INT:	return 1;
				case ShaderDataType::VEC2I:	return 2;
				case ShaderDataType::VEC3I:	return 3;
				case ShaderDataType::VEC4I:	return 4;
				case ShaderDataType::BOOL:	return 1;
			}
			return 0;
		}
	};


	class BufferLayout {

	public:

		BufferLayout() {}
		BufferLayout(const std::initializer_list<BufferData>& bl_data)
			: m_BufferLayoutData(bl_data)
		{
			CalculateValues();
		}

		inline const std::vector<BufferData>& GetLayoutElements() const { return m_BufferLayoutData; }
		inline const uint GetLayoutStride() const { return m_BufferLayoutStride; }
		
	private:

		void CalculateValues()
		{
			uint offset = 0;
			m_BufferLayoutStride = 0;
			for (auto& element : m_BufferLayoutData) {

				element.bd_Offset = offset;
				offset += element.bd_Size;
				m_BufferLayoutStride += element.bd_Size;
			}
		}

	private:

		std::vector<BufferData> m_BufferLayoutData;
		uint m_BufferLayoutStride;
	};

	class VertexBuffer {

	public:

		VertexBuffer(float* vertices, uint size);
		~VertexBuffer();

		void Bind() const;
		void UnBind() const;

		inline const BufferLayout& GetLayout() const { return m_VBLayout; }
		void SetLayout(const BufferLayout& bufferLayout) { m_VBLayout = bufferLayout; }

	private:

		uint m_ID;
		BufferLayout m_VBLayout;
	};

	class IndexBuffer {

	public:

		IndexBuffer(uint* indices, uint count);
		~IndexBuffer();

		void Bind() const;
		void UnBind() const;

		inline const uint GetCount() const { return m_Count; }
	
	private:

		uint m_Count;
		uint m_ID;

	};
}

#endif
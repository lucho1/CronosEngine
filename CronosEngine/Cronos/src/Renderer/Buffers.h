#ifndef _BUFFERS_H_
#define _BUFFERS_H_


namespace Cronos {

	struct CronosVertex;

	enum class VertexDataType { NONE = 0, FLOAT, INT, VEC2I, VEC3I, VEC4I, VEC2F, VEC3F, VEC4F, MAT3, MAT4, BOOL };


	struct BufferData {

		std::string bd_Name;
		bool bd_Normalized;
		uint bd_Size;
		uint bd_Offset;
		VertexDataType bd_VertexDataType;

		uint VertexDataTypeCount(VertexDataType vertex_data_type) const; //How many components has each type
		uint VertexDataTypeSize(VertexDataType vertex_data_type) const; //The size of the data type of the vertex

		BufferData() {}
		BufferData(VertexDataType vertex_data_type, const std::string& name, bool norm = false)
			: bd_VertexDataType(vertex_data_type), bd_Name(name), bd_Normalized(norm), bd_Size(VertexDataTypeSize(vertex_data_type)) {}

	};


	class BufferLayout {

	public:

		BufferLayout() {}
		BufferLayout(const std::initializer_list<BufferData>& bl_data) : m_BufferLayoutData(bl_data)
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

		VertexBuffer(CronosVertex* vertices, uint size);
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
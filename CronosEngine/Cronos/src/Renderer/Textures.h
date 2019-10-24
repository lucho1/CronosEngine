#ifndef _TEXTURES_H_
#define _TEXTURES_H_

namespace Cronos {

	class Texture 
	{
		friend class TextureManager;
	public:
		
		~Texture();

		void Bind(uint index = 0) const;
		void Unbind();

		inline int GetWidth() const { return m_Width; }
		inline int GetHeight() const { return m_Height; }
		inline int GetBPP() const { return m_BPP; }
		inline int GetFormat() const { return m_Format; }

		inline uint GetTextureID() const { return m_ID; }

	private:

		Texture(const std::string& path);

		uint m_ID;
		std::string m_Filepath;
		unsigned char* m_Data;
		int m_Width, m_Height, m_BPP, m_Format;

	};
}

#endif
#ifndef _TEXTURES_H_
#define _TEXTURES_H_

#include "imgui.h"

namespace Cronos {

	enum class TextureType { NONE = -1, ICON, DIFFUSE, SPECULAR, /*NORMALMAP, HEIGHTMAP, LIGHTMAP,*/ MAX_TEXTURES};

	class Texture
	{
		friend class TextureManager;
		std::string TypeStr[(int)TextureType::MAX_TEXTURES] = { "ICON", "DIFFUSE", "SPECULAR", /*"NORMALMAP", "HEIGHTMAP", "LIGHTMAP"*/ };

	public:

		~Texture();

		void Bind(uint index = 0) const;
		void Unbind();

		inline int GetWidth() const { return m_Width; }
		inline int GetHeight() const { return m_Height; }
		inline int GetBPP() const { return m_BPP; }
		inline int GetFormat() const { return m_Format; }
		const inline std::string& GetTexturePath() const { return m_Filepath; }

		inline uint GetTextureID() const { return m_ID; }
		inline TextureType GetTextureType() const { return m_TextureType; }

	private:

		Texture(const std::string& path, TextureType textype);

		uint m_ID;
		TextureType m_TextureType;
		std::string m_Filepath;
		unsigned char* m_Data;
		int m_Width, m_Height, m_BPP, m_Format;
	};
}

#endif

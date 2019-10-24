#ifndef _TEXTURES_H_
#define _TEXTURES_H_

#include "imgui.h"

namespace Cronos {

	class Texture
	{

	public:

		Texture(uint texture_iD=0);
		~Texture();
		void Bind() const {};
		void UnBind() const {};
		inline uint GetTexture() const { return m_ID; }

	private:
		uint m_ID;
		ImVec2 m_Resolution;
		
	};
}

#endif
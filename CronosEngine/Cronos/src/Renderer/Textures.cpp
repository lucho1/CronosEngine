#include "Providers/cnpch.h"

#include "Textures.h"
#include "Application.h"
#include "Modules/TextureManager.h"

#include "DevIL/include/ilut.h"

namespace Cronos {

	Texture::Texture(const std::string& path, TextureType textype)
		: m_ID(0), m_Width(0), m_Height(0), m_BPP(0), m_Format(0), m_Data(nullptr), m_Filepath(path), m_TextureType(textype)
	{
		ILuint TempTexImage;
		ilGenImages(1, &TempTexImage);
		ilBindImage(TempTexImage);

		if (ilLoadImage((const ILstring)path.c_str())) {

			TempTexImage = ilutGLBindTexImage();
			iluFlipImage();

			m_Height = ilGetInteger(IL_IMAGE_HEIGHT);
			m_Width = ilGetInteger(IL_IMAGE_WIDTH);
			m_BPP = ilGetInteger(IL_IMAGE_BYTES_PER_PIXEL);
			m_Format = ilGetInteger(IL_IMAGE_FORMAT);
			m_Data = ilGetData();

			if (m_Data)
			{
				glEnable(GL_TEXTURE_2D);
				glGenTextures(1, &m_ID);
				glBindTexture(GL_TEXTURE_2D, m_ID);

				//Set texture Parameters
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_NEAREST);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_NEAREST);

				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
		
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, m_Width, m_Height, 0, m_Format, GL_UNSIGNED_BYTE, m_Data);
				glGenerateMipmap(GL_TEXTURE_2D);

				//Erase textures
				glBindTexture(GL_TEXTURE_2D, 0);

				ilBindImage(0);
				ilDeleteImage(TempTexImage);

				std::string LOGPath = path.c_str();
				if(LOGPath.find("Cronos") != LOGPath.npos)
					LOGPath = path.substr(path.rfind("Cronos"));

				//LOGPath = LOGPath.substr(LOGPath.find_first_of(""), );

				LOG("	Successfully Loaded a %ix%i Texture of %i BPP \n	and Format %i from %s. ID: %i Type: %s ", m_Width, m_Height, m_BPP, m_Format, LOGPath.c_str(), m_ID, TypeStr[(int)m_TextureType].c_str());
			}
			else
				LOG("	Failed to load the Texture data of %s", path.c_str());
		}
		else
			ilDeleteImage(TempTexImage);

	}

	Texture::~Texture()
	{
		glDeleteTextures(1, &m_ID);
	}

	void Texture::Bind(uint index) const
	{
		glActiveTexture(GL_TEXTURE0 + index);
		glBindTexture(GL_TEXTURE_2D, m_ID);
	}

	void Texture::Unbind()
	{
		glBindTexture(GL_TEXTURE_2D, 0);
	}

}

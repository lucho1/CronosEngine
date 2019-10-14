#include "Providers/cnpch.h"

#include "TextureManager.h"
#include "Application.h"
#include "DevIL/include/ilut.h"
#include <codecvt>

namespace Cronos {

	TextureManager::TextureManager(Application * app, bool start_enabled) : Module(app, "Module TextureManager", start_enabled)
	{
	}

	TextureManager::~TextureManager()
	{
	}

	bool TextureManager::OnInit()
	{
		ilInit();
		iluInit();
		ilutInit();
		ilEnable(IL_CONV_PAL);
		ilutEnable(ILUT_OPENGL_CONV);
		ilutRenderer(ILUT_OPENGL);

		glEnable(GL_TEXTURE_2D);

		return true;
	}

	bool TextureManager::OnStart()
	{

		uint default_tex = CreateTexture("res/Baker_house.png");
		return true;
	}

	bool TextureManager::OnCleanUp()
	{
		return true;
	}

	uint TextureManager::CreateTexture(const char * path)
	{
		uint TempTex;
		ILuint TempImage;
		ilGenImages(1,&TempImage);
		ilBindImage(TempImage);
		glSamplerParameteri(TempImage, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glSamplerParameteri(TempImage, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glSamplerParameterf(TempImage, GL_TEXTURE_MAX_ANISOTROPY_EXT, 80.0f);

		if (ilLoadImage((const ILstring)path)) {
			TempTex = ilutGLBindTexImage();
			iluFlipImage();
			
			int height, width, level, bpp, format;

			height = ilGetInteger(IL_IMAGE_HEIGHT);
			width = ilGetInteger(IL_IMAGE_WIDTH);
			bpp = ilGetInteger(IL_IMAGE_BYTES_PER_PIXEL);
 			format = ilGetInteger(IL_IMAGE_FORMAT);
			ILubyte *Data = ilGetData();
			if (Data) {
				glEnable(GL_TEXTURE_2D);
				//glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
				glGenTextures(1, &TempTex);
				glBindTexture(GL_TEXTURE_2D, TempTex);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);


				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, format, GL_UNSIGNED_BYTE, Data);
				glGenerateMipmap(GL_TEXTURE_2D);
				//Erase textures
				glBindTexture(GL_TEXTURE_2D, 0);
				ilBindImage(0);
				ilDeleteImage(TempImage);
			}
			else {
				std::cout << "failed to load the data of " << path << std::endl;
			}

		}
		else {
			ilDeleteImage(TempImage);
			return 0;
		}
		

		return TempTex;
	}

	update_status TextureManager::OnUpdate(float dt)
	{


		return UPDATE_CONTINUE;
	}

}
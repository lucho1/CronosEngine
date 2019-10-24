#include "Providers/cnpch.h"

#include "TextureManager.h"
#include "Application.h"

#include "DevIL/include/ilut.h"

namespace Cronos {

	TextureManager::TextureManager(Application* app, bool start_enabled) : Module(app, "Module TextureManager", start_enabled)
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

	bool TextureManager::OnCleanUp()
	{
		ilShutDown();
		return true;
	}

	Texture* TextureManager::CreateTexture(const char* path, TextureType textype)
	{
		Texture* ret = new Texture(path, textype);
		return ret;
	}
}
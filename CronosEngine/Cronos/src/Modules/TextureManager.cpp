#include "Providers/cnpch.h"

#include "TextureManager.h"
#include "Application.h"
#include "Scene.h"

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
		CRONOS_ASSERT((textype != TextureType::NONE || textype != TextureType::MAX_TEXTURES), "Invalid Texture type creation!");

		Texture* ret = new Texture(path, textype);
		if (ret != nullptr && textype != TextureType::ICON)
		{
			std::list<Texture*>::iterator itemFound_inTexturesList = std::find(App->scene->m_TexturesLoaded.begin(), App->scene->m_TexturesLoaded.end(), ret);
			if (itemFound_inTexturesList == App->scene->m_TexturesLoaded.end())
				App->scene->m_TexturesLoaded.push_back(ret);
		}

		return ret;
	}
}

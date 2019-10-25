#ifndef _TEXTURE_HANDLER_H_
#define	_TEXTURE_HANDLER_H_

#include "Module.h"
#include "Providers/Globals.h"
#include "Renderer/Textures.h"
#include "imgui.h"
#include "Renderer/Textures.h"
namespace Cronos {

	class TextureManager : public Module
	{
	public:
		TextureManager(Application* app, bool start_enabled = true);
		~TextureManager();

		virtual bool OnInit() override;
		virtual bool OnCleanUp() override;

		Texture* CreateTexture(const char* path, TextureType textype);
	};

}

#endif // !_TEXTURE_HANDLER_H

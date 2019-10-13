#ifndef _TEXTURE_HANDLER_H_
#define	_TEXTURE_HANDLER_H_

#include "Module.h"
#include "Providers/Globals.h"

namespace Cronos {

	class TextureManager : public Module 
	{
	public:
		TextureManager(Application* app, bool start_enabled = true);
		~TextureManager();

		virtual bool OnInit() override;
		virtual bool OnStart() override;
		virtual bool OnCleanUp() override;

		uint CreateTexture(const char* path);


		virtual update_status OnUpdate(float dt) override;

	};





}




#endif // !1

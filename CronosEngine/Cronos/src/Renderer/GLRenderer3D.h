#ifndef _GLRENDERER3D_H_
#define _GLRENDERER3D_H_

#include "Module.h"
#include "Globals.h"
#include "glmath.h"
#include "Light.h"

namespace Cronos {

#define MAX_LIGHTS 8

	class GLRenderer3D : public Module
	{
	public:

		GLRenderer3D(Application* app, bool start_enabled = true);
		~GLRenderer3D();

		virtual bool OnInit() override;
		virtual update_status OnPreUpdate(float dt) override;
		virtual update_status OnPostUpdate(float dt) override;
		virtual bool OnCleanUp() override;

	public:

		Light lights[MAX_LIGHTS];
		SDL_GLContext context;
		//mat3x3 NormalMatrix;
		//mat4x4 ModelMatrix, ViewMatrix, ProjectionMatrix;
	};

}

#endif
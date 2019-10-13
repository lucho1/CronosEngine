#ifndef _GLRENDERER3D_H_
#define _GLRENDERER3D_H_

#include "Module.h"
#include "Providers/Globals.h"
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

		//Save/Load
		virtual void SaveModuleData(json& JSONFile) const override;
		virtual void LoadModuleData(json& JSONFile) override;

	public:

		void SetVsync(bool setStatus);
		void SetOpenGLVersion(int MajorVersion, int MinorVersion);

	public:

		Light lights[MAX_LIGHTS];
		SDL_GLContext context;
		//mat3x3 NormalMatrix;
		//mat4x4 ModelMatrix, ViewMatrix, ProjectionMatrix;

	private:

		bool m_VSyncActive;
		int m_OGL_Mv = 4, m_OGL_mv = 3; //Open GL Major and Minor version

	};

}

#endif
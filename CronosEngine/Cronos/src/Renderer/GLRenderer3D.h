#ifndef _GLRENDERER3D_H_
#define _GLRENDERER3D_H_

#include "Module.h"
#include "Providers/Globals.h"
#include "Light.h"

namespace Cronos {

//#define MAX_LIGHTS 8

	struct OpenGLSettings
	{
		bool Blend;
		bool ClipDistance;
		bool FaceCull;
		bool WireframeDraw;

		bool DepthTest;
		bool ScissorTest;
		bool StencilTest;

		bool ColorDither;
		bool AntialiasedLineAndPolygonSmooth; //x2
		bool Multisample;

		bool GL_Lighting;
		bool GL_ColorMaterial;
	};

	class GLRenderer3D : public Module
	{
	public:

		GLRenderer3D(Application* app, bool start_enabled = true);
		~GLRenderer3D();

		virtual bool OnInit() override;
		virtual update_status OnPreUpdate(float dt) override;
		virtual update_status OnPostUpdate(float dt) override;
		virtual bool OnCleanUp() override;
		void OnResize(uint width, uint height);

		//Save/Load
		virtual void SaveModuleData(json& JSONFile) const override;
		virtual void LoadModuleData(json& JSONFile) override;

	public:

		void SetVsync(bool setStatus);
		void SetOpenGLVersion(int MajorVersion, int MinorVersion);

		//OpenGL Settings
		void SetOpenGLSettings();

		void SetBlending(bool setStatus);
		//void SetClipDistance(bool setStatus);
		void SetFaceCulling(bool setStatus);
		void SetWireframeDrawMode(bool setStatus);

		void SetDepthTest(bool setStatus);
		void SetScissorTest(bool setStatus);
		void SetStencilTest(bool setStatus);

		void SetColorDither(bool setStatus);
		void SetAntialiasedSmooth(bool setStatus);
		void SetMultisampling(bool setStatus);

		void SetGLLighting(bool setStatus);
		void SetGLColorMaterial(bool setStatus);

		//Debug stuff
		void DrawQuad(const glm::vec3& pos, const glm::vec3& oppositePos);
		void DrawFloorPlane(bool drawAxis = false, float size = 35.0f);
		void DrawCube(glm::vec3 maxVec, glm::vec3 minVec);

	public:

		//Light lights[MAX_LIGHTS];
		Light centerLight;
		SDL_GLContext context;
		//mat3x3 NormalMatrix;
		//mat4x4 ModelMatrix, ViewMatrix, ProjectionMatrix;

	private:

		glm::mat4 fPlane = glm::mat4(1.0f);

		bool m_VSyncActive;
		int m_OGL_Mv = 4, m_OGL_mv = 3; //Open GL Major and Minor version

		OpenGLSettings m_CurrentSettings;
	};

}

#endif
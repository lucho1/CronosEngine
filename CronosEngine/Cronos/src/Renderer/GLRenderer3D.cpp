#include "Providers/cnpch.h"

#include "Providers/Globals.h"
#include "Application.h"
#include "GLRenderer3D.h"

#include <glad/glad.h>

namespace Cronos {

	GLRenderer3D::GLRenderer3D(Application* app, bool start_enabled) : Module(app, "Module OpenGL Renderer 3D", start_enabled)
	{
	}

	// Destructor
	GLRenderer3D::~GLRenderer3D()
	{}

	// Called before render is available
	bool GLRenderer3D::OnInit()
	{
		App->EditorGUI->AddLog("Creating 3D Render context");
		LOG("Creating 3D Renderer context");
		bool ret = true;

		//Create context
		context = SDL_GL_CreateContext(App->window->window);

		App->EditorGUI->AddLog("Loading Glad");
		gladLoadGL();
		GL_SETERRORHANDLER(m_OGL_Mv, m_OGL_mv);
		SetOpenGLSettings();

		if (context == NULL)
		{
			LOG("OpenGL context could not be created! SDL_Error: %s\n", SDL_GetError());
			ret = false;
		}

		if (ret == true)
		{
			//Use Vsync
			if (m_VSyncActive && SDL_GL_SetSwapInterval(1) < 0) {
				App->EditorGUI->AddLog(std::string("Warning: Unable to set VSync! SDL Error: %s\n", SDL_GetError()));
				LOG("Warning: Unable to set VSync! SDL Error: %s\n", SDL_GetError());
			}
			//Initialize Projection Matrix
			glMatrixMode(GL_PROJECTION);
			glLoadIdentity();

			//Check for error
			GLenum error = glGetError();
			if (error != GL_NO_ERROR)
			{
				LOG("Error initializing OpenGL! %s\n", error);
				ret = false;
			}

			//Initialize Modelview Matrix
			glMatrixMode(GL_MODELVIEW);
			glLoadIdentity();

			//Check for error
			error = glGetError();
			if (error != GL_NO_ERROR)
			{
				LOG("Error initializing OpenGL! %s\n", error);
				ret = false;
			}

			glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
			glClearDepth(1.0f);

			//Initialize clear color
			//glClearColor(0.3f, 0.3f, 0.3f, 1.0f);

			//Check for error
			error = glGetError();
			if (error != GL_NO_ERROR)
			{
				LOG("Error initializing OpenGL! %s\n", error);
				ret = false;
			}

			GLfloat LightModelAmbient[] = { 0.0f, 0.0f, 0.0f, 1.0f };
			glLightModelfv(GL_LIGHT_MODEL_AMBIENT, LightModelAmbient);

			lights[0].ref = GL_LIGHT0;
			lights[0].ambient.Set(0.75f, 0.75f, 0.75f, 1.0f); //0.25f, 0.25f, 0.25f, 1.0f
			lights[0].diffuse.Set(0.0f, 0.0f, 0.0f, 0.0f); //0.75f, 0.75f, 0.75f, 1.0f
			lights[0].SetPos(0.0f, 0.0f, 2.5f);
			lights[0].Init();

			GLfloat MaterialAmbient[] = { 1.0f, 1.0f, 1.0f, 1.0f };
			glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, MaterialAmbient);

			GLfloat MaterialDiffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };
			glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, MaterialDiffuse);

			lights[0].Active(true);
		}

		// Projection matrix for
		App->window->OnResize(App->window->GetWidth(), App->window->GetHeight());

		return ret;
	}

	// PreUpdate: clear buffer
	update_status GLRenderer3D::OnPreUpdate(float dt)
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glLoadIdentity();

		glMatrixMode(GL_MODELVIEW);
		glLoadMatrixf(App->engineCamera->GetViewMatrix());

		// light 0 on cam pos
		lights[0].SetPos(App->engineCamera->GetPosition().x, App->engineCamera->GetPosition().y, App->engineCamera->GetPosition().z);

		for (uint i = 0; i < MAX_LIGHTS; ++i)
			lights[i].Render();

		return UPDATE_CONTINUE;
	}

	// PostUpdate present buffer to screen
	update_status GLRenderer3D::OnPostUpdate(float dt)
	{
		SDL_GL_SwapWindow(App->window->window);
		return UPDATE_CONTINUE;
	}

	// Called before quitting
	bool GLRenderer3D::OnCleanUp()
	{
		LOG("Destroying 3D Renderer");
		SDL_GL_DeleteContext(context);

		return true;
	}

	void GLRenderer3D::SaveModuleData(json& JSONFile) const
	{
		JSONFile["Renderer"]["VSYNC"] = m_VSyncActive;
		JSONFile["Renderer"]["OpenGL_MajorV"] = m_OGL_Mv;
		JSONFile["Renderer"]["OpenGL_MinorV"] = m_OGL_mv;

		JSONFile["Renderer"]["Blend"] = m_CurrentSettings.Blend;
		JSONFile["Renderer"]["ClipDistance"] = m_CurrentSettings.ClipDistance;
		JSONFile["Renderer"]["FaceCulling"] = m_CurrentSettings.FaceCull;
		JSONFile["Renderer"]["WireframeDraw"] = m_CurrentSettings.WireframeDraw;

		JSONFile["Renderer"]["DepthTest"] = m_CurrentSettings.DepthTest;
		JSONFile["Renderer"]["ScissorTest"] = m_CurrentSettings.ScissorTest;
		JSONFile["Renderer"]["StencilTest"] = m_CurrentSettings.StencilTest;

		JSONFile["Renderer"]["ColorDither"] = m_CurrentSettings.ColorDither;
		JSONFile["Renderer"]["AntialiasedLPSmooth"] = m_CurrentSettings.AntialiasedLineAndPolygonSmooth;
		JSONFile["Renderer"]["Multisampling"] = m_CurrentSettings.Multisample;

		JSONFile["Renderer"]["GL_Lighting"] = m_CurrentSettings.GL_Lighting;
		JSONFile["Renderer"]["GL_ColorMaterial"] = m_CurrentSettings.GL_ColorMaterial;
	}

	void GLRenderer3D::LoadModuleData(json& JSONFile)
	{
		m_VSyncActive = JSONFile["Renderer"]["VSYNC"];
		m_OGL_Mv = JSONFile["Renderer"]["OpenGL_MajorV"];
		m_OGL_mv = JSONFile["Renderer"]["OpenGL_MinorV"];

		m_CurrentSettings.Blend = JSONFile["Renderer"]["Blend"];
		m_CurrentSettings.ClipDistance = JSONFile["Renderer"]["ClipDistance"];
		m_CurrentSettings.FaceCull = JSONFile["Renderer"]["FaceCulling"];
		m_CurrentSettings.WireframeDraw = JSONFile["Renderer"]["WireframeDraw"];
		
		m_CurrentSettings.DepthTest = JSONFile["Renderer"]["DepthTest"];
		m_CurrentSettings.ScissorTest= JSONFile["Renderer"]["ScissorTest"];
		m_CurrentSettings.StencilTest= JSONFile["Renderer"]["StencilTest"];
		
		m_CurrentSettings.ColorDither = JSONFile["Renderer"]["ColorDither"];
		m_CurrentSettings.AntialiasedLineAndPolygonSmooth = JSONFile["Renderer"]["AntialiasedLPSmooth"];
		m_CurrentSettings.Multisample = JSONFile["Renderer"]["Multisampling"];
		
		m_CurrentSettings.GL_Lighting = JSONFile["Renderer"]["GL_Lighting"];
		m_CurrentSettings.GL_ColorMaterial = JSONFile["Renderer"]["GL_ColorMaterial"];

		SetVsync(m_VSyncActive);
		SetOpenGLVersion(m_OGL_Mv, m_OGL_mv);
	}

	void GLRenderer3D::SetVsync(bool setStatus)
	{
		m_VSyncActive = setStatus;

		if (setStatus == true)
			SDL_GL_SetSwapInterval(1);
		else
			SDL_GL_SetSwapInterval(0);
	}

	void GLRenderer3D::SetOpenGLVersion(int MajorVersion, int MinorVersion)
	{
		if (MajorVersion < 4 && MajorVersion > 2 && MinorVersion > 0 && MinorVersion < 6)
		{
			m_OGL_Mv = MajorVersion;
			m_OGL_mv = MinorVersion;

			SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, m_OGL_Mv);
			SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, m_OGL_mv);
			SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_COMPATIBILITY);
			//SDL_GL_SetAttribute(SDL_GL_SHARE_WITH_CURRENT_CONTEXT, 1);
		}
	}

	//OpenGL Settings
	void GLRenderer3D::SetOpenGLSettings()
	{
		SetBlending(m_CurrentSettings.Blend);
		//void SetClipDistance(m_CurrentSettings.ClipDistance);
		SetFaceCulling(m_CurrentSettings.FaceCull);
		SetWireframeDrawMode(m_CurrentSettings.WireframeDraw);

		SetDepthTest(m_CurrentSettings.DepthTest);
		SetScissorTest(m_CurrentSettings.ScissorTest);
		SetStencilTest(m_CurrentSettings.StencilTest);

		SetColorDither(m_CurrentSettings.ColorDither);
		SetAntialiasedSmooth(m_CurrentSettings.AntialiasedLineAndPolygonSmooth);
		SetMultisampling(m_CurrentSettings.Multisample);
		
		SetGLLighting(m_CurrentSettings.GL_Lighting);
		SetGLColorMaterial(m_CurrentSettings.GL_ColorMaterial);
	}

	void GLRenderer3D::SetBlending(bool setStatus)
	{
		if (setStatus == true && glIsEnabled(GL_BLEND))
			return;
		
		m_CurrentSettings.Blend = setStatus;
		if (setStatus == true)
		{
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		}
		else
			glDisable(GL_BLEND);
	}

	//void GLRenderer3D::SetClipDistance(bool setStatus)
	//{
	//	if (glIsEnabled(GL_CLIP_DISTANCE0) && setStatus != true)
	//	{
	//		m_CurrentSettings.ClipDistance = setStatus;
	//		if (setStatus == true)
	//			glEnable(GL_CLIP_DISTANCE0);
	//		else
	//			glDisable(GL_CLIP_DISTANCE0);
	//	}
	//}

	void GLRenderer3D::SetGLLighting(bool setStatus)
	{
		if (setStatus == true && glIsEnabled(GL_LIGHTING))
			return;

		m_CurrentSettings.GL_Lighting = setStatus;
		if (setStatus == true)
			glEnable(GL_LIGHTING);
		else
			glDisable(GL_LIGHTING);
	}

	void GLRenderer3D::SetGLColorMaterial(bool setStatus)
	{
		if (setStatus == true && glIsEnabled(GL_COLOR_MATERIAL))
			return;

		m_CurrentSettings.GL_ColorMaterial = setStatus;
		if (setStatus == true)
			glEnable(GL_COLOR_MATERIAL);
		else
			glDisable(GL_COLOR_MATERIAL);
	}

	void GLRenderer3D::SetFaceCulling(bool setStatus)
	{
		if (setStatus == true && glIsEnabled(GL_CULL_FACE))
			return;
		
		m_CurrentSettings.FaceCull = setStatus;
		if (setStatus == true)
			glEnable(GL_CULL_FACE);
		else
			glDisable(GL_CULL_FACE);
	}

	void GLRenderer3D::SetWireframeDrawMode(bool setStatus)
	{
			m_CurrentSettings.WireframeDraw = setStatus;
			if (setStatus == true)
				glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			else
				glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}

	void GLRenderer3D::SetDepthTest(bool setStatus)
	{
		if (setStatus == true && glIsEnabled(GL_DEPTH_TEST))
			return;
				
		m_CurrentSettings.DepthTest = setStatus;
		if (setStatus == true)
			glEnable(GL_DEPTH_TEST);
		else
			glDisable(GL_DEPTH_TEST);
	}

	void GLRenderer3D::SetScissorTest(bool setStatus)
	{
		if (setStatus == true && glIsEnabled(GL_SCISSOR_TEST))
			return;
					
		m_CurrentSettings.ScissorTest = setStatus;
		if (setStatus == true)
			glEnable(GL_SCISSOR_TEST);
		else
			glDisable(GL_SCISSOR_TEST);
	}

	void GLRenderer3D::SetStencilTest(bool setStatus)
	{
		if (setStatus == true && glIsEnabled(GL_STENCIL_TEST))
			return;

		m_CurrentSettings.StencilTest = setStatus;
		if (setStatus == true)
			glEnable(GL_STENCIL_TEST);
		else
			glDisable(GL_STENCIL_TEST);
	}

	void GLRenderer3D::SetColorDither(bool setStatus)
	{
		if (setStatus == true && glIsEnabled(GL_DITHER))
			return;

		m_CurrentSettings.ColorDither = setStatus;
		if (setStatus == true)
			glEnable(GL_DITHER);
		else
			glDisable(GL_DITHER);
	}

	void GLRenderer3D::SetAntialiasedSmooth(bool setStatus)
	{
		if (setStatus == true && glIsEnabled(GL_LINE_SMOOTH) && glIsEnabled(GL_POLYGON_SMOOTH))
			return;

		m_CurrentSettings.AntialiasedLineAndPolygonSmooth = setStatus;
		if (setStatus == true)
		{
			glEnable(GL_POLYGON_SMOOTH);
			glEnable(GL_LINE_SMOOTH);
		}
		else
		{
			glDisable(GL_POLYGON_SMOOTH);
			glDisable(GL_LINE_SMOOTH);
		}
	}

	void GLRenderer3D::SetMultisampling(bool setStatus)
	{
		if (setStatus == true && glIsEnabled(GL_MULTISAMPLE))
			return;
		
		m_CurrentSettings.Multisample = setStatus;
		if (setStatus == true)
			glEnable(GL_MULTISAMPLE);
		else
			glDisable(GL_MULTISAMPLE);
	}

}

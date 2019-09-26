#include "cnpch.h"
#include "Globals.h"
#include "Application.h"
#include "GLRenderer3D.h"

//#include "SDL/include/SDL_opengl.h"
// #include <gl/GL.h>
//#include <gl/GLU.h>
#include <glad/glad.h>
//#pragma comment (lib, "glu32.lib")    /* link OpenGL Utility lib     */
//#pragma comment (lib, "opengl32.lib") /* link Microsoft OpenGL lib   */

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
		LOG("Creating 3D Renderer context");
		bool ret = true;

		//Create context
		context = SDL_GL_CreateContext(App->window->window);
		gladLoadGL();
		if (context == NULL)
		{
			LOG("OpenGL context could not be created! SDL_Error: %s\n", SDL_GetError());
			ret = false;
		}

		if (ret == true)
		{
			//Use Vsync
			if (VSYNC && SDL_GL_SetSwapInterval(1) < 0)
				LOG("Warning: Unable to set VSync! SDL Error: %s\n", SDL_GetError());

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
			glClearColor(0.3f, 0.3f, 0.3f, 1.0f);

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

			glEnable(GL_DEPTH_TEST);
			glEnable(GL_CULL_FACE);
			lights[0].Active(true);
			glEnable(GL_LIGHTING);
			glEnable(GL_COLOR_MATERIAL);
		}

		// Projection matrix for
		OnResize(SCREEN_WIDTH, SCREEN_HEIGHT);

		return ret;
	}

	// PreUpdate: clear buffer
	update_status GLRenderer3D::OnPreUpdate(float dt)
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glLoadIdentity();

		glMatrixMode(GL_MODELVIEW);
		glLoadMatrixf(App->camera->GetViewMatrix());

		// light 0 on cam pos
		lights[0].SetPos(App->camera->m_Position.x, App->camera->m_Position.y, App->camera->m_Position.z);

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


	void GLRenderer3D::OnResize(int width, int height)
	{
		glViewport(0, 0, width, height);

		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		ProjectionMatrix = perspective(60.0f, (float)width / (float)height, 0.125f, 512.0f);
		glLoadMatrixf(&ProjectionMatrix);

		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
	}

}
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
	{
	}

	// Called before render is available
	bool GLRenderer3D::OnInit()
	{
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

		//	/*glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
		//	glClearDepth(1.0f);*/

		//	//Initialize clear color
		//	//glClearColor(0.3f, 0.3f, 0.3f, 1.0f);

		//	//Check for error
		//	error = glGetError();
		//	if (error != GL_NO_ERROR)
		//	{
		//		LOG("Error initializing OpenGL! %s\n", error);
		//		ret = false;
		//	}

			GLfloat LightModelAmbient[] = { 0.0f, 0.0f, 0.0f, 1.0f };
			glLightModelfv(GL_LIGHT_MODEL_AMBIENT, LightModelAmbient);

			centerLight.ref = GL_LIGHT0;
			centerLight.ambient.Set(0.75f, 0.75f, 0.75f, 1.0f); //0.25f, 0.25f, 0.25f, 1.0f
			centerLight.diffuse.Set(0.0f, 0.0f, 0.0f, 0.0f); //0.75f, 0.75f, 0.75f, 1.0f
			centerLight.SetPos(0.0f, 0.0f, 2.5f);
			centerLight.Init();
			centerLight.Active(true);

		//	GLfloat MaterialAmbient[] = { 1.0f, 1.0f, 1.0f, 1.0f };
		//	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, MaterialAmbient);

		//	GLfloat MaterialDiffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };
		//	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, MaterialDiffuse);

		//	lights[0].Active(true);
		}

		ResetTree();

		// Projection matrix for
		App->window->OnResize(App->window->GetWidth(), App->window->GetHeight());
		return ret;
	}

	// Called before quitting
	bool GLRenderer3D::OnCleanUp()
	{
		LOG("Destroying 3D Renderer");

		m_RenderingOctree.CleanUp();
		RELEASE(App->scene->BasicTestShader);
		SDL_GL_DeleteContext(context);

		return true;
	}

	// PreUpdate: clear buffer
	update_status GLRenderer3D::OnPreUpdate(float dt)
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// light 0 on cam pos
//		lights[0].SetPos(App->engineCamera->GetPosition().x, App->engineCamera->GetPosition().y, App->engineCamera->GetPosition().z);

		glm::vec3 p = App->engineCamera->GetPosition();
		centerLight.SetPos(p.x, p.y, p.z);
		centerLight.Render();

		m_ObjectsInOctreeNode = m_RenderingOctree.GetObjectsContained(*m_CurrentCamera->GetFrustum());

		//for (uint i = 0; i < MAX_LIGHTS; ++i)
		//	lights[i].Render();

		return UPDATE_CONTINUE;
	}

	// PostUpdate present buffer to screen
	update_status GLRenderer3D::OnPostUpdate(float dt)
	{
		DrawFloorPlane(true);
		m_RenderingOctree.Draw();

		//Wireframe Mode (or not) ------------------------------------------------------------
		if (App->EditorGUI->GetCurrentShading() == ShadingMode::Shaded)
			SetWireframeDrawMode(false);
		else if (App->EditorGUI->GetCurrentShading() == ShadingMode::Wireframe)
		{
			glLineWidth(0.5f);
			glColor4f(m_DefaultColor.r, m_DefaultColor.g, m_DefaultColor.b, m_DefaultColor.a);
			SetWireframeDrawMode(true);
			glLineWidth(m_DefaultLinewidth);
		}

		//Shader Stuff & ZBuffer -------------------------------------------------------------
		App->scene->BasicTestShader->Bind();
		App->scene->BasicTestShader->SetUniformMat4f("u_View", m_CurrentCamera->GetViewMatrix());
		App->scene->BasicTestShader->SetUniformMat4f("u_Proj", m_CurrentCamera->GetProjectionMatrix());

		if (m_ChangeZBufferDrawing)
		{
			m_ChangeZBufferDrawing = false;
			m_DrawZBuffer = !m_DrawZBuffer;

			if (m_DrawZBuffer)
				App->scene->BasicTestShader->SetUniform1i("u_drawZBuffer", 1);
			else
				App->scene->BasicTestShader->SetUniform1i("u_drawZBuffer", 0);
		}
		if (m_DrawZBuffer)
			App->scene->BasicTestShader->SetUniformVec2f("u_CamPlanes", glm::vec2(App->engineCamera->GetNearPlane(), App->engineCamera->GetFarPlane()));


		//Objects Rendering -----------------------------------------------------------------
		if (!m_FrustumCulling || (m_FrustumCulling && m_OctreeAcceleratedFrustumCulling))
		{
			std::list<GameObject*>::iterator it = m_RenderingList.begin();
			for (; it != m_RenderingList.end(); it++)
				Render(it);
		}
		else
		{
			std::list<GameObject*>::iterator it = m_RenderingList.begin();
			for (; it != m_RenderingList.end(); it++)
			{
				if (!m_CurrentCamera->GetFrustum()->Intersects((*it)->GetAABB()) &&
					!m_CurrentCamera->GetFrustum()->Contains((*it)->GetAABB()))
					continue;

				Render(it);
			}
		}

		App->scene->BasicTestShader->Unbind();
		m_RenderingList.clear();

		//SDL_GL_SwapWindow(App->window->window);
		return UPDATE_CONTINUE;
	}

	void GLRenderer3D::RenderSubmit(GameObject* gameObject)
	{
		if (m_FrustumCulling && m_OctreeAcceleratedFrustumCulling && !m_ObjectsInOctreeNode.empty())
		{
			//You can also do:
			//auto item = std::find_if(ObjectsInOctreeNode.begin(), ObjectsInOctreeNode.end(),
			//	[&gameObject](GameObject* obj) {return obj;});
			//
			//if(item != ObjectsInOctreeNode.end())
			//	m_RenderingList.push_back(gameObject);

			auto item = std::find(m_ObjectsInOctreeNode.begin(), m_ObjectsInOctreeNode.end(), gameObject);
			if (item != m_ObjectsInOctreeNode.end())
				m_RenderingList.push_back(gameObject);
		}
		else
			m_RenderingList.push_back(gameObject);
	}

	void GLRenderer3D::Render(std::list<GameObject*>::iterator it)
	{
		App->scene->BasicTestShader->Bind();
		MaterialComponent* material = (*it)->GetComponent<MaterialComponent>();
		VertexArray* VAO = (*it)->GetComponent<MeshComponent>()->GetVAO();

		if (material != nullptr)
			material->Bind(true);

		App->scene->BasicTestShader->SetUniformMat4f("u_Model", (*it)->GetComponent<TransformComponent>()->GetGlobalTranformationMatrix());
		VAO->Bind();

		glDrawElements(GL_TRIANGLES, VAO->GetIndexBuffer()->GetCount(), GL_UNSIGNED_INT, 0);

		if (material != nullptr)
			material->Unbind();

		VAO->UnBind();
	}

	//Resize window
	void GLRenderer3D::OnResize(uint width, uint height)
	{
		glViewport(0, 0, width, height);
		App->window->ReCalculateAspectRatio(width, height);
		App->engineCamera->ChangeProjection();
	}

	//Cast a Ray from camera
	const glm::vec3 GLRenderer3D::RaycastFromCamera(glm::vec3 camPos)
	{
		//return App->engineCamera->CreateRay() + glm::vec3(0, 0, 5);
		glm::vec3 dir = App->engineCamera->RaycastForward();
		float dist = App->engineCamera->GetFarPlane();

		//dir.z *= 2.0f;

		return camPos + dir * dist; //Ray End
	}

	// -----------------------------------------------------------------------------------
	void GLRenderer3D::DrawLine(glm::vec3 minP, glm::vec3 maxP, glm::vec3 color, float linewidth, glm::mat4 modelMat)
	{
		glPushMatrix();
		glMultMatrixf(glm::value_ptr(modelMat));
		glMatrixMode(GL_PROJECTION);
		glLoadMatrixf(glm::value_ptr(App->engineCamera->GetProjectionMatrix()));
		glMatrixMode(GL_MODELVIEW);
		glLoadMatrixf(glm::value_ptr(App->engineCamera->GetViewMatrix()));

		glLineWidth(linewidth);
		glColor3f(color.r, color.g, color.b);

		glBegin(GL_LINES);

		glVertex3f(minP.x, minP.y, minP.z);
		glVertex3f(maxP.x, maxP.y, maxP.z);

		glEnd();

		glLineWidth(1.0f);

		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		glPopMatrix();
		glColor4f(m_DefaultColor.r, m_DefaultColor.g, m_DefaultColor.b, m_DefaultColor.a);
		glLineWidth(m_DefaultLinewidth);
	}

	void GLRenderer3D::DrawQuad(const glm::vec3& pos, const glm::vec3& oppositePos)
	{
		glm::vec3 posOpp_Down = glm::vec3(pos.x + oppositePos.x, pos.y, pos.z);
		glm::vec3 posOpp_Top = glm::vec3(pos.x, pos.y + oppositePos.y, oppositePos.z);

		glBegin(GL_QUADS);
			glVertex3f(pos.x, pos.y, pos.z);
			glVertex3f(posOpp_Down.x, posOpp_Down.y, posOpp_Down.z);
			glVertex3f(oppositePos.x, oppositePos.y, oppositePos.z);
			glVertex3f(posOpp_Top.x, posOpp_Top.y, posOpp_Top.z);
		glEnd();
	}

	void GLRenderer3D::DrawFloorPlane(bool drawAxis, float size)
	{
		//Set polygon draw mode and appropiated matrices for OGL
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glPushMatrix();
		glMultMatrixf(glm::value_ptr(fPlane));
		glMatrixMode(GL_PROJECTION);
		glLoadMatrixf(glm::value_ptr(App->engineCamera->GetProjectionMatrix()));
		glMatrixMode(GL_MODELVIEW);
		glLoadMatrixf(glm::value_ptr(App->engineCamera->GetViewMatrix()));

		float colorIntensity = 0.65f;

		//Axis draw
		if (drawAxis)
		{
			glLineWidth(2.0f);
			glBegin(GL_LINES);

			glColor4f(colorIntensity, 0.0f, 0.0f, 1.0f);
			glVertex3f(0.0f, 0.0f, 0.0f); glVertex3f(1.0f, 0.0f, 0.0f);
			glVertex3f(1.0f, 0.1f, 0.0f); glVertex3f(1.1f, -0.1f, 0.0f);
			glVertex3f(1.1f, 0.1f, 0.0f); glVertex3f(1.0f, -0.1f, 0.0f);

			glColor4f(0.0f, colorIntensity, 0.0f, 1.0f);
			glVertex3f(0.0f, 0.0f, 0.0f); glVertex3f(0.0f, 1.0f, 0.0f);
			glVertex3f(-0.05f, 1.25f, 0.0f); glVertex3f(0.0f, 1.15f, 0.0f);
			glVertex3f(0.05f, 1.25f, 0.0f); glVertex3f(0.0f, 1.15f, 0.0f);
			glVertex3f(0.0f, 1.15f, 0.0f); glVertex3f(0.0f, 1.05f, 0.0f);

			glColor4f(0.0f, 0.0f, colorIntensity, 1.0f);
			glVertex3f(0.0f, 0.0f, 0.0f); glVertex3f(0.0f, 0.0f, 1.0f);
			glVertex3f(-0.05f, 0.1f, 1.05f); glVertex3f(0.05f, 0.1f, 1.05f);
			glVertex3f(0.05f, 0.1f, 1.05f); glVertex3f(-0.05f, -0.1f, 1.05f);
			glVertex3f(-0.05f, -0.1f, 1.05f); glVertex3f(0.05f, -0.1f, 1.05f);

			glEnd();
		}

		//Plane draw
		glColor3f(colorIntensity, colorIntensity, colorIntensity);
		glLineWidth(1.0f);
		glBegin(GL_LINES);

		float d = size;
		for (float i = -d; i <= d; i += 1.0f)
		{
			glVertex3f(i, 0.0f, -d);
			glVertex3f(i, 0.0f, d);
			glVertex3f(-d, 0.0f, i);
			glVertex3f(d, 0.0f, i);
		}

		glEnd();

		//Set again Identity for OGL Matrices & Polygon draw to fill again
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		glPopMatrix();
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glColor4f(m_DefaultColor.r, m_DefaultColor.g, m_DefaultColor.b, m_DefaultColor.a);
		glLineWidth(m_DefaultLinewidth);
	}

	void GLRenderer3D::DrawCube(glm::vec3 maxVec, glm::vec3 minVec, glm::vec3 color, float linewidth, glm::mat4 modelMat)
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glPushMatrix();
		glMultMatrixf(glm::value_ptr(modelMat));
		glMatrixMode(GL_PROJECTION);
		glLoadMatrixf(glm::value_ptr(App->engineCamera->GetProjectionMatrix()));
		glMatrixMode(GL_MODELVIEW);
		glLoadMatrixf(glm::value_ptr(App->engineCamera->GetViewMatrix()));

		glColor3f(color.r, color.g, color.b);
		glLineWidth(linewidth);

		//Top
		glBegin(GL_QUADS);
		glNormal3f(0.0f, 1.0f, 0.0f);

		glVertex3f(minVec.x, maxVec.y, minVec.z);
		glVertex3f(maxVec.x, maxVec.y, minVec.z);
		glVertex3f(maxVec.x, maxVec.y, maxVec.z);
		glVertex3f(minVec.x, maxVec.y, maxVec.z);

		//Front
		glNormal3f(0.0f, 0.0f, 1.0f);

		glVertex3f(minVec.x, minVec.y, minVec.z);
		glVertex3f(maxVec.x, minVec.y, minVec.z);
		glVertex3f(maxVec.x, maxVec.y, minVec.z);
		glVertex3f(minVec.x, maxVec.y, minVec.z);

		//Right
		glNormal3f(1.0f, 0.0f, 0.0f);

		glVertex3f(maxVec.x, minVec.y, minVec.z);
		glVertex3f(maxVec.x, minVec.y, maxVec.z);
		glVertex3f(maxVec.x, maxVec.y, maxVec.z);
		glVertex3f(maxVec.x, maxVec.y, minVec.z);

		//Left
		glNormal3f(-1.0f, 0.0f, 0.0f);

		glVertex3f(minVec.x, minVec.y, minVec.z);
		glVertex3f(minVec.x, maxVec.y, minVec.z);
		glVertex3f(minVec.x, maxVec.y, maxVec.z);
		glVertex3f(minVec.x, minVec.y, maxVec.z);

		//Bottom
		glNormal3f(0.0f, -1.0f, 0.0f);

		glVertex3f(minVec.x, minVec.y, minVec.z);
		glVertex3f(maxVec.x, minVec.y, minVec.z);
		glVertex3f(maxVec.x, minVec.y, maxVec.z);
		glVertex3f(minVec.x, minVec.y, maxVec.z);

		//Back
		glNormal3f(0.0f, 0.0f, -1.0f);

		glVertex3f(maxVec.x, maxVec.y, maxVec.z);
		glVertex3f(maxVec.x, minVec.y, maxVec.z);
		glVertex3f(minVec.x, minVec.y, maxVec.z);
		glVertex3f(minVec.x, maxVec.y, maxVec.z);
		glEnd();

		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		glPopMatrix();
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glColor4f(m_DefaultColor.r, m_DefaultColor.g, m_DefaultColor.b, m_DefaultColor.a);
		glLineWidth(m_DefaultLinewidth);
	}

	void GLRenderer3D::DrawRotatedCube(glm::vec3 maxVec, glm::vec3 minVec, glm::quat rotation, glm::vec3 color, float linewidth)
	{
		glm::quat q1 = glm::quat(0.0f, maxVec);
		glm::quat q2 = glm::quat(0.0f, minVec);

		glm::quat res1 = rotation * q1 * glm::conjugate(rotation);
		glm::quat res2 = rotation * q2 * glm::conjugate(rotation);

		glm::vec3 resVecMax = glm::vec3(res1.x, res1.y, res1.z);
		glm::vec3 resVecMin = glm::vec3(res2.x, res2.y, res2.z);

		DrawCube(resVecMax, resVecMin, color, linewidth);
	}


	// -----------------------------------------------------------------------------------
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
		m_CurrentSettings.ScissorTest = JSONFile["Renderer"]["ScissorTest"];
		m_CurrentSettings.StencilTest = JSONFile["Renderer"]["StencilTest"];

		m_CurrentSettings.ColorDither = JSONFile["Renderer"]["ColorDither"];
		m_CurrentSettings.AntialiasedLineAndPolygonSmooth = JSONFile["Renderer"]["AntialiasedLPSmooth"];
		m_CurrentSettings.Multisample = JSONFile["Renderer"]["Multisampling"];

		m_CurrentSettings.GL_Lighting = JSONFile["Renderer"]["GL_Lighting"];
		m_CurrentSettings.GL_ColorMaterial = JSONFile["Renderer"]["GL_ColorMaterial"];

		SetVsync(m_VSyncActive);
		SetOpenGLVersion(m_OGL_Mv, m_OGL_mv);
	}


	// -----------------------------------------------------------------------------------
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

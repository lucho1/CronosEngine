#ifndef _GLRENDERER3D_H_
#define _GLRENDERER3D_H_

#include "Module.h"
#include "Providers/Globals.h"
#include "GL_Light.h"
#include "Helpers/Camera.h"
#include "Helpers/CNOctree.h"

#include "GameObject/Components/MaterialComponent.h"
#include "GameObject/Components/TransformComponent.h"
#include "GameObject/Components/LightComponent.h"

#include "VertexArray.h"

//Make sure this values matches the shader ones
#define MAX_POINTLIGHTS 20
#define MAX_SPOTLIGHTS 20
#define MAX_DIRLIGHTS 8

namespace Cronos {

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

		//Module Methods
		GLRenderer3D(Application* app, bool start_enabled = true);
		~GLRenderer3D();

		virtual bool OnInit() override;
		virtual bool OnStart() override;
		virtual update_status OnPreUpdate(float dt) override;
		virtual update_status OnPostUpdate(float dt) override;
		virtual bool OnCleanUp() override;

		//Save/Load
		virtual void SaveModuleData(json& JSONFile) const override;
		virtual void LoadModuleData(json& JSONFile) override;

	public:

		//Renderer Public Methods
		void RenderSubmit(GameObject* gameObject);
		void OnResize(uint width, uint height);
		
		//Rendering Octree Methods
		void InsertInTree(GameObject* gameObject)	{ m_RenderingOctree.Insert(gameObject); }
		void RemoveFromTree(GameObject* gameObject)	{ m_RenderingOctree.TakeOut(gameObject); }
		void ResetTree()							{ AABB OT_AABB = math::AABB(math::float3(-100.0f), math::float3(100.0f)); m_RenderingOctree = CnOctree(OT_AABB, 2); }
		
		//Camera
		const inline Camera* GetCurrentCamera() const { return m_CurrentCamera; }

		//Materials things
		void AddMaterialToList(Material* material)	{ m_MaterialsList.push_back(material); }
		void PopMaterial(Material* material);

		inline const std::vector<Material*> GetMaterialsList()	const { return m_MaterialsList; }
		inline Material* GetMaterialInMaterialsList(uint index)	const { return m_MaterialsList[index]; }
		inline Material* GetDefaultMaterial()					const { return *m_MaterialsList.begin(); }
		inline Material* GetLightMaterial()						const { return m_MaterialsList[1]; }
		inline const uint GetDefaultMaterialID()				const { return (*m_MaterialsList.begin())->GetMaterialID(); }
		inline Shader* GetBasicShader()							const { return m_BasicShader; }
		inline Shader* GetDefaultShader()						const { return m_DefaultShader; }
		//Lights
		void AddLight(LightComponent* lightCompObj) { m_LightsList.push_back(lightCompObj); }
		void PopLight(LightComponent* lightCompObj) { m_LightsList.erase(std::find(m_LightsList.begin(), m_LightsList.end(), lightCompObj)); }
		void ClearLightList() { m_LightsList.clear(); m_PointLightsVec.clear(); m_DirectionalLightsVec.clear(); m_SpotLightsVec.clear(); }
		//Shader

		void AddShader(Shader* shader) { m_ShaderList.push_back(shader); }
		bool isShaderLoaded(uint shaderID) const;
		Shader* GetShaderFromList(uint shaderID)const;
		Shader* GetShaderFromList(int pos)						const { return m_ShaderList[pos]; }
		int getPositionShader(uint shaderID);
		std::vector<Shader*>GetShaderList()						const { return m_ShaderList; }

	public:

		//Setters
		void SetVsync(bool setStatus);
		void SetOpenGLVersion(int MajorVersion, int MinorVersion);

		void SetDefaultColor(glm::vec4 color) { m_DefaultColor = color; }
		void SetDefaultLineWidth(float LWidth) { m_DefaultLinewidth = LWidth; }


		//OpenGL Settings
		const inline OpenGLSettings GetOGLSettings() const { return m_CurrentSettings; }

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

		bool &SetFrustrum() { return m_FrustumCulling; }
		bool &SetOctreeOptimization() { return m_OctreeAcceleratedFrustumCulling; }
		bool &SetOctreeDraw() { return m_SeeOctree; }


	public:

		//Debug stuff
		void DrawQuad(const glm::vec3& pos, const glm::vec3& oppositePos);
		void DrawFloorPlane(bool drawAxis = false, float size = 35.0f);
		void DrawCube(glm::vec3 maxVec, glm::vec3 minVec, glm::vec3 color = glm::vec3(1.0f), float linewidth = 0.5f, glm::mat4 modelMat = glm::mat4(1.0f));
		void DrawRotatedCube(glm::vec3 maxVec, glm::vec3 minVec, glm::quat rotation, glm::vec3 color = glm::vec3(1.0f), float linewidth = 0.5f);
		void DrawLine(glm::vec3 minP, glm::vec3 maxP, glm::vec3 color = glm::vec3(1.0f), float linewidth = 1.0f, glm::mat4 modelMat = glm::mat4(1.0f));

		//Z Buffer Rendering
		bool& SetZBufferRendering() { return m_ChangeZBufferDrawing; }
		void SetZBuffer() { m_ChangeZBufferDrawing = !m_ChangeZBufferDrawing; }

		//Change Lighting System
		bool& SetLightSystem() { return m_ChangeLightSystem; }
		void SetLightingSystem() { m_ChangeLightSystem = !m_ChangeLightSystem; }
		std::string GetCameraListNames() const						{ return CameraNameList; }
		std::string GetShaderListNames() const						{ return ShaderNameList; }

		//Chose camera to render from
		void SetRenderingCamera(Camera& camera) { m_CurrentCamera = &camera; }

	private:

		void Render(std::list<GameObject*>::iterator it);

	public:

		//Light lights[MAX_LIGHTS];
		GL_Light centerLight;
		SDL_GLContext context;
		CnOctree m_RenderingOctree;
		std::vector<GameObject*>m_CameraList;

		std::vector<LightComponent*> m_PointLightsVec;
		std::vector<LightComponent*> m_DirectionalLightsVec;
		std::vector<LightComponent*> m_SpotLightsVec;

	private:
		
		UniformBuffer* m_UBO;

		std::vector<LightComponent*> m_LightsList;
		std::vector<Shader*>m_ShaderList;
		bool m_BlinnPhongLighting = true;
		bool m_ChangeLightSystem = false;

		std::vector<Material*> m_MaterialsList;
		Shader* m_BasicShader = nullptr;
		Shader* m_DefaultShader = nullptr;
		Timer m_BasicSh_RunTime;

		std::string CameraNameList;
		std::string ShaderNameList;

		//Octree Rendrering
		std::vector<GameObject*> m_ObjectsInOctreeNode;
		std::list<GameObject*> m_RenderingList;
		bool m_FrustumCulling = true;
		bool m_OctreeAcceleratedFrustumCulling = false;
		bool m_SeeOctree = false;

		//ZBuffer rendering
		bool m_DrawZBuffer = false;
		bool m_ChangeZBufferDrawing = false;

		//Current camera we're rendering with
		Camera* m_CurrentCamera = nullptr;

		//Other stuff
		glm::mat4 fPlane = glm::mat4(1.0f);

		bool m_VSyncActive;
		int m_OGL_Mv = 4, m_OGL_mv = 3; //Open GL Major and Minor version

		OpenGLSettings m_CurrentSettings;

		glm::vec4 m_DefaultColor = glm::vec4(1.0f);
		float m_DefaultLinewidth = 1.0f;
	};

}

#endif
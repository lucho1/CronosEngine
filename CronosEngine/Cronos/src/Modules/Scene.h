#ifndef _SCENE_H_
#define _SCENE_H_

#include "Module.h"
#include "Providers/Globals.h"
#include "GameObject/GameObject.h"
#include "GameObject/PrimitiveGameObject.h"
#include "Renderer/Shaders.h"
#include "ImGui/ImGuizmo.h"
#include "Providers/AssimpImporter.h"
#include "GameObject/Components/CameraComponent.h"
#include "GameObject/Components/TransformComponent.h"

#include "Helpers/CNOctree.h"

namespace Cronos {

	class AssimpCronosTranslator;

	class Scene : public Module
	{
	public:

		//Module Methods
		Scene(Application* app, bool start_enabled = true);
		~Scene();

		virtual bool OnStart() override;
		virtual bool OnCleanUp() override;

		virtual update_status OnPreUpdate(float dt) override;
		virtual update_status OnUpdate(float dt) override;
		virtual update_status OnPostUpdate(float dt) override;

	public:

		//Scene Methods
		GameObject* CreateModel(const char* path);

		bool SaveScene(const char* SceneName);
		bool LoadScene(const char* SceneName);

	public:

		bool mustSave = false;
		bool mustLoad = false;
		bool mustCleanScene = false;

	public:

		std::vector<GameObject*>m_GameObjects;
		GameObject* ToCopy;
				
		std::list<Texture*> m_TexturesLoaded;

		std::string m_SceneName;

		ImGuizmo::OPERATION guizmo_operation = ImGuizmo::TRANSLATE;
		ImGuizmo::MODE      guizmo_mode = ImGuizmo::LOCAL;
		void DrawGuizmo(Camera* camera, GameObject* go);

		//WaveModifiers
		float WaveMaxTime = 149.5f;
		float WaveAmplitude = 3.4f;
		float WaveLenght = 45.7f;
		float WaveVelocity = 25.5f;
		float FoamVelocity = 15.2f;
		float WaveColorGrading = 0.037f;
		float FoamDirectionX = 0.540f;
		float FoamDirectionY = 0.037f;
		float WaveMovementMultuplier = 0.60f;
		glm::vec4 WaveColor;

		//m_WaterShader->SetUniform1f("u_MaxTime", maxT);
		//m_WaterShader->SetUniform1f("u_Amplitude", 5.0f);
		//m_WaterShader->SetUniform1f("u_WaveLength", 35.0f);
		//m_WaterShader->SetUniform1f("u_Velocity", 15.0f);
		//m_WaterShader->SetUniform1f("u_FOAMVelocity", 5.0f);
		//m_WaterShader->SetUniform1f("u_ColorGradingOffset", 0.1f); //Fragment Shader
		//m_WaterShader->SetUniformVec2f("u_FOAMDirection", glm::vec2(1.0f, 1.0f));
		//m_WaterShader->SetUniform1f("u_WaveMovementMultiplicator", 2.0f);

	private:

		AssimpCronosImporter m_CNAssimp_Importer;
		GameObject* m_StreetModel;

		GameObject* m_Wave = nullptr;
		Material* m_WaveMaterial = nullptr;
		Shader* m_WaterShader = nullptr;
		Texture* m_WaveTexture = nullptr;
		Texture* m_WaveSpecText = nullptr;
		Timer m_WaveTimer;
	};

}

#endif

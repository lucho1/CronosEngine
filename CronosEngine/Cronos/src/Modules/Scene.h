#ifndef _SCENE_H_
#define _SCENE_H_

#include "Module.h"
#include "Providers/Globals.h"
#include "GameObject/GameObject.h"
#include "GameObject/PrimitiveGameObject.h"
#include "Renderer/Shaders.h"

#include "Providers/AssimpImporter.h"

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

	private:

		AssimpCronosImporter m_CNAssimp_Importer;
		GameObject* m_StreetModel;

		GameObject* m_Wave = nullptr;
		Shader* m_WaterShader = nullptr;
		Texture* m_WaveTexture = nullptr;
		Timer m_WaveTimer;
	};

}

#endif

#ifndef _SCENE_H_
#define _SCENE_H_

#include "Module.h"
#include "Providers/Globals.h"
#include "Renderer/Primitive.h"
#include "GameObject/GameObject.h"
#include "GameObject/PrimitiveGameObject.h"
#include "Renderer/Shaders.h"

#include "Providers/AssimpImporter.h"

namespace Cronos {

	class AssimpCronosTranslator;

	class Scene : public Module
	{
	public:

		Scene(Application* app, bool start_enabled = true);
		~Scene();

		virtual bool OnStart() override;
		virtual bool OnCleanUp() override;

		virtual update_status OnPreUpdate(float dt) override;
		virtual update_status OnUpdate(float dt) override;
		virtual update_status OnPostUpdate(float dt) override;

		GameObject* CreateModel(const char* path);

	public:

		std::vector<GameObject*>m_GameObjects;

		Shader* BasicTestShader = nullptr; //To test

		std::list<Texture*> m_TexturesLoaded;

	private:

		//Plane m_FloorPlane;
		PrimitiveGameObject* m_SceneFloorPlane;
		AssimpCronosImporter m_CNAssimp_Importer;
		GameObject* m_HouseModel;
	};

}

#endif

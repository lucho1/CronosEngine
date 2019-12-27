#pragma once

#include "Core/Module.h"

namespace Cronos {

	class ResourceMesh;
	class Resource;

	class ResourceManager :public Module {
	public:

		ResourceManager(Application *app, bool start_enabled=true);
		virtual ~ResourceManager() {};

		virtual bool OnStart() override { return true; }
		virtual bool OnCleanUp() override { return true; }

		void AddResource(Resource* res);
		bool isMeshLoaded(uint resID);
		bool isMeshSaved(uint resID);
		bool isMaterialLoaded(uint resID);
		bool isMaterialSaved(uint resID);

		bool saveResources();

		ResourceMesh* getMeshResource(uint resID);
		ResourceMaterial* getMaterialResource(uint resID);

	private:

		std::vector<uint>m_ResourcesIDList;
		std::vector<Resource*>m_ResourceList;
	};

}
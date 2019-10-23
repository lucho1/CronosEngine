#ifndef _MODULE_H_
#define _MODULE_H_

#include "json/json.hpp"
using json = nlohmann::json;

namespace Cronos {

	class Application;

	class Module
	{

	public:

		std::string m_ModuleName;
		bool m_ModuleActive;

	public:

		Module(Application* parent, std::string name/* = "Module Unnamed"*/, bool start_enabled = true):m_ModuleName(name)
		{
		}

		virtual ~Module()
		{
		}

		virtual bool OnInit() { return true; }
		virtual bool OnStart() { return true; }
		virtual bool OnCleanUp() { return true; }

		virtual update_status OnPreUpdate(float dt) { return UPDATE_CONTINUE; }
		virtual update_status OnUpdate(float dt) { return UPDATE_CONTINUE; }
		virtual update_status OnPostUpdate(float dt) { return UPDATE_CONTINUE; }

		virtual void SaveModuleData(json& JSONFile) const {}
		virtual void LoadModuleData(json& JSONFile) {}

	};

}

#endif
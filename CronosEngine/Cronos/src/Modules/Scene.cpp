#include "Providers/cnpch.h"
#include "Providers/Globals.h"

#include "Application.h"
#include "Scene.h"

#include "Renderer/Primitive.h"

namespace Cronos {

	Scene::Scene(Application* app, bool start_enabled) : Module(app, "Module Scene", start_enabled)
	{
	}

	Scene::~Scene()
	{}

	// Load assets
	bool Scene::OnStart()
	{
		LOG("Loading Intro assets");
		bool ret = true;

		App->engineCamera->Move(vec3(1.0f, 1.0f, 0.0f)); //Camera begins one unit up in Y and one unit to the right
		App->engineCamera->LookAt(vec3(0.0f, 0.0f, 0.0f)); //To look at center

		return ret;
	}

	// Load assets
	bool Scene::OnCleanUp()
	{
		LOG("Unloading Intro scene");
		return true;
	}

	// Update: draw background
	update_status Scene::OnUpdate(float dt)
	{
		// "Floor" Plane
		Plane p(0.0f, 1.0f, 0.0f, 0.0f); //Here we express the normal so it's 0 centered
		p.axis = true; //Enable axis render
		p.Render();

		return UPDATE_CONTINUE;
	}

	// PreUpdate
	update_status Scene::OnPreUpdate(float dt)
	{
		return UPDATE_CONTINUE;
	}

	// UPostUpdate
	update_status Scene::OnPostUpdate(float dt)
	{
		return UPDATE_CONTINUE;
	}

}
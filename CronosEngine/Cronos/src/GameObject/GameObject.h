#ifndef _GAMEOBJECT_H_
#define _GAMEOBJECT_H_

#include "Components/Component.h"

namespace Cronos {

	class Component;

	class GameObject
	{
	public:

		GameObject();
		~GameObject();

		void Update();
		void CreateComponent(ComponentType type);

	private:

		bool Active;
		std::string m_Name;

		std::vector<Component*> Components;

	};

}

#endif // !_GAMEOBJECT_H_

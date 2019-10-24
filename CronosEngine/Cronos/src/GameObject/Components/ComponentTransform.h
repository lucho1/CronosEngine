#ifndef _COMPONENTTRANSFORM_H_
#define _COMPONENTTRANSFORM_H_

#include "glm/glm.hpp"
#include "Component.h"

namespace Cronos {

	class GameObject;

	class ComponentTransform : public Component
	{
	public:

		ComponentTransform(glm::vec3 pos,glm::vec3 rot,glm::vec3 scale,bool active = true);
		~ComponentTransform();

		virtual void OnStart() override;
		virtual void Update() override ;

		glm::vec3 m_Position;
		glm::vec3 m_Rotation;
		glm::vec3 m_Scale;
		
	};

}

#endif // !_COMPONENTTRANSFORM_H_

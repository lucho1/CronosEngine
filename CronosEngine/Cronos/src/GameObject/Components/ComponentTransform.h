#ifndef _COMPONENTTRANSFORM_H_
#define _COMPONENTTRANSFORM_H_

#include "glm/glm.hpp"
#include "Component.h"

namespace Cronos {

	class GameObject;

	class ComponentTransform : public Component
	{
	public:

		ComponentTransform(GameObject* parentGo, bool active = true);
		~ComponentTransform();

		virtual void OnStart() override;
		virtual void Update(float dt) override ;

		void setPosition(glm::vec3 pos) { m_Position = pos; }
		void setRotation(glm::vec3 rot) { m_Rotation = rot; }
		void setScale(glm::vec3 scale) { m_Scale= scale; }

		inline glm::vec3 GetPosition() const { return m_Position; };
		inline glm::vec3 GetRotation() const { return m_Rotation; };
		inline glm::vec3 GetScale() const { return m_Scale; };

	public:

		glm::vec3 m_Position;
		glm::vec3 m_Rotation;
		glm::vec3 m_Scale;
		
	};

}

#endif // !_COMPONENTTRANSFORM_H_

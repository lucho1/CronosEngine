#ifndef _TRANSFORMCOMPONENT_H_
#define _TRANSFORMCOMPONENT_H_

#include "glm/glm.hpp"
#include "Component.h"

namespace Cronos {

	class GameObject;

	class TransformComponent : public Component
	{
	public:

		TransformComponent(GameObject* parentGo, bool active = true);
		~TransformComponent();

		virtual void OnStart() override;
		virtual void Update(float dt) override;

		void SetPosition(glm::vec3 pos) { m_Position = pos; }
		void SetRotation(glm::vec3 rot) { m_Rotation = rot; }
		void SetScale(glm::vec3 scale) { m_Scale = scale; }

		inline glm::vec3 GetPosition() const { return m_Position; };
		inline glm::vec3 GetRotation() const { return m_Rotation; };
		inline glm::vec3 GetScale() const { return m_Scale; };

		static ComponentType GetType() { return ComponentType::TRANSFORM; };

	private:

		glm::vec3 m_Position;
		glm::vec3 m_Rotation;
		glm::vec3 m_Scale;
		
	};

}

#endif // !_COMPONENTTRANSFORM_H_

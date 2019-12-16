#ifndef _TRANSFORMCOMPONENT_H_
#define _TRANSFORMCOMPONENT_H_

#include "glm/glm.hpp"
#include "Component.h"
#include "MaterialComponent.h"

#include "GameObject/PrimitiveGameObject.h"

namespace Cronos {

	class GameObject;

	class TransformComponent : public Component
	{
	public:

		//Component Methods
		TransformComponent(GameObject* attachedGO, bool active = true);
		~TransformComponent() {}

		void Update(float dt) override;

	public:

		//Setters
		//This is to SET the game object to a desired transformation
		void SetPosition(glm::vec3 pos);
		void SetScale(glm::vec3 scale);
		void SetOrientation(glm::vec3 euler_angles);
		
		//This is to ADD the game object a transformation
		void Move(glm::vec3 translation);
		void Scale(glm::vec3 scale);
		void Rotate(glm::vec3 euler_angles);

	public:

		//Getters
		const inline glm::vec3 GetTranslation()					const { return m_Translation; };
		const inline glm::vec3 GetScale()						const { return m_Scale; };
		const inline glm::vec3 GetOrientation()					const { return glm::degrees(m_EulerAngles); };
		const inline glm::quat GetOrientationQuaternion()		const { return m_Orientation; }
		const inline glm::vec3 GetGlobalTranslation()			const { glm::vec3 pos; glm::decompose(m_GlobalTransformationMatrix, glm::vec3(), glm::quat(), pos, glm::vec3(), glm::vec4()); return pos; }

		const inline glm::mat4 GetLocalTranformationMatrix()	const { return m_LocalTransformationMatrix; }
		const inline glm::mat4 GetGlobalTranformationMatrix()	const { return m_GlobalTransformationMatrix; }
		
		//Type
		static ComponentType GetType()							{ return ComponentType::TRANSFORM; };

	private:

		void UpdateTransform();

	private:

		glm::mat4 m_LocalTransformationMatrix;
		glm::mat4 m_GlobalTransformationMatrix;

		glm::vec3 m_Translation;
		glm::vec3 m_Scale;

		glm::quat m_Orientation;
		glm::vec3 m_EulerAngles;
	};

}

#endif // !_COMPONENTTRANSFORM_H_

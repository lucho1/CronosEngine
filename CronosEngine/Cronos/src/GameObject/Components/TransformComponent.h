#ifndef _TRANSFORMCOMPONENT_H_
#define _TRANSFORMCOMPONENT_H_

#include "glm/glm.hpp"
#include "Component.h"
#include "MaterialComponent.h"

#include <AABB.hpp>

namespace Cronos {

	class GameObject;

	class TransformComponent : public Component
	{
	public:

		TransformComponent(GameObject* attachedGO, bool active = true);
		~TransformComponent();

		virtual void OnStart() override;
		virtual void Update(float dt) override;

		//Type
		static ComponentType GetType() { return ComponentType::TRANSFORM; };

		//AABBs & Axis
		void SetAABB(const glm::vec3& minVec, const glm::vec3& maxVec) { m_ContainerAABBCube = AABB(minVec, maxVec); }
		void SetAABB(const AABB& aabbCube) { m_ContainerAABBCube = AABB(aabbCube); }
		
		bool &SetDrawAxis() { return DrawAxis; }
		void DrawCentralAxis();

		const inline AABB GetAABB() const { return m_ContainerAABBCube; }
		const inline glm::vec3 GetCentralAxis() const { return m_ContainerAABBCube.getCenter(); }

	public:

		//This is to SET the game object to a desired transformation
		void SetPosition(glm::vec3 pos);
		void SetScale(glm::vec3 scale);
		void SetOrientation(glm::vec3 eulerAxAngles);
		
		//This is to ADD the game object a transformation
		void Move(glm::vec3 translation);
		void Scale(glm::vec3 scale);
		void Rotate(glm::vec3 eulerAxAngles);

		//Getters
		const inline glm::vec3 GetTranslation() const { return m_Translation; };
		const inline glm::vec3 GetScale() const { return m_Scale; };
		const inline glm::vec3 GetOrientation() const { return m_Rotation_InEulerAngles; };
		//const inline glm::vec3 GetRotationVector() const { return glm::quat::value_type(m_Rotation); }

		const inline glm::mat4 GetLocalTranformationMatrix() const { return m_LocalTransformationMatrix; }
		const inline glm::mat4 GetGlobalTranformationMatrix() const { return m_GlobalTransformationMatrix; }

		

	private:

		//Decompose the matrix in the position, scale vectors and orientation quaternion
		void UpdateTransform();

	public:

		bool DrawAxis = false;

	private:

		glm::mat4 m_LocalTransformationMatrix;
		glm::mat4 m_GlobalTransformationMatrix = glm::mat4(1.0f);

		glm::vec3 m_Translation;
		glm::vec3 m_Scale;
		glm::quat m_Orientation;


		glm::vec3 m_Rotation_InEulerAngles;

		glm::mat4 rotMat = glm::mat4(1.0f);


		//glm::vec3 m_Orientation_eulerAnglesRAD;
		//glm::vec3 m_Orientation_eulerAnglesDEG;

		AABB m_ContainerAABBCube;
	};

}

#endif // !_COMPONENTTRANSFORM_H_

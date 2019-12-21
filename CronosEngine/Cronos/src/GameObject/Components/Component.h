#ifndef _COMPONENT_H_
#define _COMPONENT_H_

namespace Cronos {

	class GameObject;

	enum class ComponentType 
	{
		NONE = -1,
		TRANSFORM,
		MESH,
		MESH_RENDERER,
		MATERIAL,
		CAMERA,
		LIGHT
	};

	class Component 
	{
		friend class GameObject;
	private:

		ComponentType m_Type;

	protected:

		bool m_Active;

	public:

		Component(ComponentType type, GameObject* attachedGO, bool start_enabled = true)
			: m_Type(type), m_Parent(attachedGO), m_Active(start_enabled) {};

		virtual void OnStart() {}
		virtual void Update(float dt) {}

		virtual void Enable() { m_Active = true; }
		virtual void Disable() { m_Active = false; }

		void SetParent(GameObject* parent) { m_Parent = parent; }

		//TODO CHANGE TO PRIVATE WHEN MATERIAL COMPONENT ADDED
		GameObject* m_Parent = nullptr;

		//Getters
		inline GameObject* GetParent() const { return m_Parent; }
		inline ComponentType GetComponentType() const { return m_Type; }
		inline bool isEnabled() const { return m_Active; }
	};

}

#endif // !_COMPONENT_H_

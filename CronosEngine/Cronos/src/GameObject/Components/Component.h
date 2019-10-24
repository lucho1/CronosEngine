#ifndef _COMPONENT_H_
#define _COMPONENT_H_

namespace Cronos {

	class GameObject;

	enum class ComponentType 
	{
		none = -1,
		Transform,
		Mesh,
		MeshRenderer,
		Material		
	};

	class Component 
	{
	public:

		GameObject* m_Parent;
		ComponentType m_Type;
		bool m_Active;

	public:

		Component(ComponentType type, GameObject* parentGO, bool start_enabled = true):m_Type(type),m_Parent(parentGO),m_Active(start_enabled) {};

		virtual void OnStart() {};
		virtual void Update(float dt) {};

		virtual void Enable() { m_Active = true; }
		virtual void Disable() { m_Active = false; }

		void SetParent(GameObject* parent) { m_Parent = parent; }
		inline GameObject* GetParent() const { return m_Parent; }
		inline ComponentType GetComponentType() const { return m_Type; }
		inline bool isEnabled() const { return m_Active; }

	};

}

#endif // !_COMPONENT_H_

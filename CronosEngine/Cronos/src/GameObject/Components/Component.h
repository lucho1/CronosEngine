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
		bool m_Active;
		ComponentType m_Type;

	public:

		Component(ComponentType type, bool start_enabled = true):m_Type(type),m_Active(start_enabled) {};

		virtual void OnStart() = 0;
		virtual void Update() = 0;

		virtual void Enable() { m_Active = true; }
		virtual void Disable() { m_Active = false; }

		void SetParent(GameObject* parent) { m_Parent = parent; }
		inline GameObject* GetParent() const { return m_Parent; }
		inline ComponentType GetComponentType() const { return m_Type; }
		inline bool isEnabled() const { return m_Active; }

	};

}

#endif // !_COMPONENT_H_

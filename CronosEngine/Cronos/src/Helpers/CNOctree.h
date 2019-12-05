//#ifndef _CNQUADTREE_H_
//#define _CNQUADTREE_H_
//
//#include "GameObject/GameObject.h"
//
//namespace Cronos {
//
//	enum class NodeType { NONE = -1, ROOT, PARENT, CHILD };
//
//	class CnOT_Node
//	{
//		//friend class CnQuadtree;
//	public:
//
//		CnOT_Node() {}
//		CnOT_Node(AABB partitionSpace, NodeType nodeType, int maxObjectsInside);
//		~CnOT_Node() {}
//
//		//Node methods
//		void Draw();
//		void Split();
//		void CleanUp();
//
//		bool Insert(GameObject* GObj);
//
//		//Getters 
//		const bool IsChild()  const { return m_IsChild; }
//		const AABB& GetCubicSpace() const { return m_CubicSpace; }
//
//		std::vector<GameObject*> GetObjectsContained(AABB cubicSpace);
//
//	private:
//		
//		CnOT_Node* m_Nodes = nullptr;
//		int m_ChildsQuantity = 0, m_MaxObjectsInside = 0;
//
//		bool m_IsChild = true;
//		AABB m_CubicSpace;
//		NodeType m_NodeType = NodeType::NONE;
//		
//		std::vector<GameObject*> GObjectsContained_Vector;
//	};
//
//	class CnOctree
//	{
//	public:
//
//		CnOctree() {}
//		CnOctree(AABB space, int maxObjectsInside);
//		~CnOctree() {}
//
//		//Octree Methods
//		void Draw();
//		void CleanUp();
//
//		bool Insert(GameObject* GObject);
//
//		//Getters
//		std::vector<GameObject*> GetObjectsContained(AABB cubicSpace);
//		const bool IsSplitted() const { return isSplitted; }
//		const AABB& GetCubicSpace() const { return m_Root->GetCubicSpace(); }
//
//	private:
//
//		bool isSplitted = false;
//		CnOT_Node* m_Root = nullptr;
//	};
//}
//#endif

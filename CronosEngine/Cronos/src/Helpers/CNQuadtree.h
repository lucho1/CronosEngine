#ifndef _CNQUADTREE_H_
#define _CNQUADTREE_H_

#include "Helpers/AABB.h"
#include "GameObject/GameObject.h"

namespace Cronos {

	enum class NodeType { NONE = -1, ROOT, PARENT, CHILD };

	class CnQT_Node
	{
		//friend class CnQuadtree;
	public:

		CnQT_Node() {}
		CnQT_Node(AABB partitionSpace, NodeType nodeType, int capacity);
		~CnQT_Node();

		void CleanUp();
		void Split();

		void Draw();

		bool IsLeaf() { return m_Leaf; }

		bool Insert(GameObject* GObj);

		std::vector<GameObject*> GetObjectsContained(AABB cubicSpace);

	private:

		bool m_Leaf = true;
		NodeType m_NodeType = NodeType::NONE;
		AABB m_CubicSpace;

		CnQT_Node* m_Nodes = nullptr;
		int m_ChildsQuantity = 0;
		int m_Capacity = 0;

		std::vector<GameObject*> GObjectsContained_Vector;
	};

	class CnQuadtree
	{
	public:

		CnQuadtree() {}
		CnQuadtree(AABB space, int capacity);
		~CnQuadtree();

		void Draw();
		void CleanUp();

		bool Insert(GameObject* GObject);
		std::vector<GameObject*> GetObjectsContained(AABB cubicSpace);
		
		CnQT_Node* m_Root = nullptr;
	};
}
#endif

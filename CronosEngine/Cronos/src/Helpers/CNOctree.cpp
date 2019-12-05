#include "Providers/cnpch.h"
//#include "CNOctree.h"
//
//#include "Renderer/GLRenderer3D.h"
//#include "Application.h"
//#include "GameObject/Components/TransformComponent.h"
//
//namespace Cronos {
//
//	//Quadtree -----------------------------------------------------------------
//	CnOctree::CnOctree(AABB space, int maxObjectsInside)
//	{
//		m_Root = new CnOT_Node(space, NodeType::ROOT, maxObjectsInside);
//	}
//
//	void CnOctree::Draw()
//	{
//		m_Root->Draw();
//	}
//
//	void CnOctree::CleanUp()
//	{
//		m_Root->CleanUp();
//	}
//
//	bool CnOctree::Insert(GameObject* GObject)
//	{
//		isSplitted = true;
//		return m_Root->Insert(GObject);
//	}
//
//	std::vector<GameObject*> CnOctree::GetObjectsContained(AABB cubicSpace)
//	{
//		return m_Root->GetObjectsContained(cubicSpace);
//	}
//
//
//	//Octree Node ------------------------------------------------------------
//	CnOT_Node::CnOT_Node(AABB partitionSpace, NodeType nodeType, int maxObjectsInside)
//		: m_CubicSpace(partitionSpace), m_NodeType(nodeType), m_MaxObjectsInside(maxObjectsInside)
//	{
//	}
//
//	std::vector<GameObject*> CnOT_Node::GetObjectsContained(AABB cubicSpace)
//	{
//		std::vector<GameObject*> objectsInside;
//		if (m_CubicSpace.intersects(cubicSpace) == true)
//		{
//			LOG("No Objects intersecting this cube! Return value empty");
//			return objectsInside;
//		}
//
//		std::vector<GameObject*>::iterator it = GObjectsContained_Vector.begin();
//		for (; it != GObjectsContained_Vector.end(); it++)
//			objectsInside.push_back(*it);
//
//		if (m_ChildsQuantity > 0)
//		{
//			std::vector<GameObject*> childrenObjects;
//			for (uint i = 0; i < m_ChildsQuantity; i++)
//			{
//				std::vector<GameObject*>nodeObjs = m_Nodes[i].GetObjectsContained(cubicSpace);
//				if (nodeObjs.size() > 0)
//					childrenObjects.insert(childrenObjects.begin(), nodeObjs.begin(), nodeObjs.end());
//			}
//
//			if (childrenObjects.size() > 0)
//				objectsInside.insert(objectsInside.begin(), childrenObjects.begin(), childrenObjects.end());
//		}
//
//		return objectsInside;
//	}
//
//	void CnOT_Node::Draw()
//	{
//		if (m_IsChild == false)
//			for (int i = 0; i < m_ChildsQuantity; ++i)
//				m_Nodes[i].Draw();
//
//		App->renderer3D->DrawCube(m_CubicSpace.getMax(), m_CubicSpace.getMin(), glm::vec3(Red.r, Red.g, Red.b));
//	}
//
//	void CnOT_Node::CleanUp()
//	{
//		for (uint i = 0; i < m_ChildsQuantity; i++)
//			m_Nodes[i].CleanUp();
//
//		if (m_Nodes != nullptr)
//		{
//			delete[] m_Nodes;
//			m_Nodes = nullptr;
//		}
//	}
//
//	void CnOT_Node::Split()
//	{
//		//Get the current AABB Max and Min
//		glm::vec3 c_Min = m_CubicSpace.getMin();
//		glm::vec3 c_Max = m_CubicSpace.getMax();
//		glm::vec3 max, min;
//
//		//Let's begin
//		AABB children[8];
//
//		//First Node
//		max = glm::vec3(c_Max.x, c_Max.y, c_Max.z);
//		min = glm::vec3((c_Max.x + c_Min.x) / 2.0f, (c_Min.y + c_Max.y) / 2.0f, (c_Max.z + c_Min.z) / 2.0f);
//
//		///Set new node with those max and min
//		children[0] = AABB(min, max);
//
//		//Second Node
//		max = glm::vec3((c_Max.x + c_Min.x) / 2.0f, c_Max.y, c_Max.z);
//		min = glm::vec3(c_Min.x, (c_Min.y + c_Max.y) / 2.0f, (c_Max.z + c_Min.z) / 2.0f);
//
//		///Set new node with those max and min
//		children[1] = AABB(min, max);
//
//		//Third Node
//		max = glm::vec3(c_Max.x, c_Max.y, (c_Max.z + c_Min.z) / 2.0f);
//		min = glm::vec3((c_Max.x + c_Min.x) / 2.0f, (c_Min.y + c_Max.y) / 2.0f, c_Min.z);
//
//		///Set new node with those max and min
//		children[2] = AABB(min, max);
//
//		//Fourth Node
//		max = glm::vec3((c_Max.x + c_Min.x) / 2.0f, c_Max.y, (c_Max.z + c_Min.z) / 2.0f);
//		min = glm::vec3(c_Min.x, (c_Min.y + c_Max.y) / 2.0f, c_Min.z);
//
//		///Set new node with those max and min
//		children[3] = AABB(min, max);
//
//		//Fifth Node
//		max = glm::vec3(c_Max.x, (c_Min.y + c_Max.y) / 2.0f, c_Max.z);
//		min = glm::vec3((c_Max.x + c_Min.x) / 2.0f, c_Min.y, (c_Max.z + c_Min.z) / 2.0f);
//
//		///Set new node with those max and min
//		children[4] = AABB(min, max);
//
//		//Sixth Node
//		max = glm::vec3((c_Max.x + c_Min.x)/ 2.0f, (c_Min.y + c_Max.y) / 2.0f, c_Max.z);
//		min = glm::vec3(c_Min.x, c_Min.y, (c_Max.z + c_Min.z) / 2.0f);
//
//		///Set new node with those max and min
//		children[5] = AABB(min, max);
//
//		//Seventh Node
//		max = glm::vec3(c_Max.x, (c_Min.y + c_Max.y) / 2.0f, (c_Max.z + c_Min.z) / 2.0f);
//		min = glm::vec3((c_Max.x + c_Min.x) / 2.0f, c_Min.y, c_Min.z);
//
//		///Set new node with those max and min
//		children[6] = AABB(min, max);
//
//		//Eight and Final Node
//		max = glm::vec3((c_Max.x + c_Min.x) / 2.0f, (c_Min.y + c_Min.y) / 2.0f, (c_Max.z + c_Min.z) / 2.0f);
//		min = glm::vec3(c_Min.x, c_Min.y, c_Min.z);
//
//		///Set new node with those max and min
//		children[7] = AABB(min, max);
//
//		//Create those new nodes as actual Octree Nodes ---------------------------------------------------------
//		if (m_NodeType != NodeType::ROOT)
//			m_NodeType = NodeType::PARENT;
//
//		m_ChildsQuantity = 8;
//		m_IsChild = false;
//		m_Nodes = new CnOT_Node[8];
//		for (int i = 0; i < 8; i++)
//			m_Nodes[i] = CnOT_Node(children[i], NodeType::CHILD, m_MaxObjectsInside);
//	}
//
//	bool CnOT_Node::Insert(GameObject* GObj)
//	{
//		AABB GOAABB = GObj->GetComponent<TransformComponent>()->GetAABB();
//		if (m_CubicSpace.intersects(GOAABB) == false)
//			return false;
//
//		int nodesContained = 0, container = 0;
//		if (m_NodeType == NodeType::PARENT || (m_NodeType == NodeType::ROOT && m_ChildsQuantity > 0))
//		{
//			for (uint i = 0; i < m_ChildsQuantity; i++)
//			{
//				if (m_Nodes[i].m_CubicSpace.intersects(GOAABB) == true)
//				{
//					nodesContained++;
//					container = i;
//				}
//
//				if (nodesContained > 1)
//					break;
//			}
//
//			if (nodesContained == 1)
//				m_Nodes[container].Insert(GObj);
//			else if (nodesContained > 1)
//				GObjectsContained_Vector.push_back(GObj);
//
//			return true;
//		}
//		else if (m_NodeType == NodeType::CHILD || (m_NodeType == NodeType::ROOT && m_ChildsQuantity == 0))
//		{
//			GObjectsContained_Vector.push_back(GObj);
//			if (GObjectsContained_Vector.size() > m_MaxObjectsInside)
//			{
//				std::vector<GameObject*> newObjects = GObjectsContained_Vector;
//				GObjectsContained_Vector.clear();
//
//				for (uint j = 0; j < newObjects.size(); j++)
//				{
//					nodesContained = 0;
//					container = 0;
//					for (uint i = 0; i < m_ChildsQuantity; i++)
//					{
//						if (m_Nodes[i].m_CubicSpace.intersects(newObjects[j]->GetComponent<TransformComponent>()->GetAABB()) == true)
//						{
//							nodesContained++;
//							container = i;
//						}
//						if(nodesContained > 1)
//							break;
//					}
//
//					if (nodesContained == 1)
//						m_Nodes[container].Insert(newObjects[j]);
//					else if (nodesContained > 1)
//						GObjectsContained_Vector.push_back(newObjects[j]);
//				}
//
//				Split();
//			}
//			return true;
//		}
//
//		return false;
//	}
//
//}//Namespace Cronos
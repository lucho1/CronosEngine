#include "Providers/cnpch.h"
#include "CNOctree.h"

#include "Renderer/GLRenderer3D.h"
#include "Application.h"
#include "GameObject/Components/TransformComponent.h"

namespace Cronos {

	//Octree ------------------------------------------------------------------
	CnOctree::CnOctree(math::AABB space, int maxObjectsInside)
	{
		m_Root = new CnOT_Node(space, NodeType::ROOT, maxObjectsInside);
	}

	void CnOctree::Draw()
	{
		m_Root->Draw();
	}

	void CnOctree::CleanUp()
	{
		m_Root->CleanUp();
	}

	bool CnOctree::Insert(GameObject* GObject)
	{
		isSplitted = true;
		return m_Root->Insert(GObject);
	}

	void CnOctree::TakeOut(GameObject* GObject)
	{
		m_Root->TakeOut(GObject);
	}

	std::vector<GameObject*> CnOctree::GetObjectsContained(const math::AABB cubicSpace)
	{
		return m_Root->GetObjectsContained(cubicSpace);
	}

	std::vector<GameObject*> CnOctree::GetObjectsContained(const math::Frustum cameraFrustum)
	{
		return m_Root->GetObjectsContained(cameraFrustum);
	}


	//Octree Node ------------------------------------------------------------
	CnOT_Node::CnOT_Node(math::AABB partitionSpace, NodeType nodeType, int maxObjectsInside)
		: m_CubicSpace(partitionSpace), m_NodeType(nodeType), m_MaxObjectsInside(maxObjectsInside)
	{
	}


	std::vector<GameObject*> CnOT_Node::GetObjectsContained(const math::AABB cubicSpace)
	{
		std::vector<GameObject*> objectsInside;
		if (!m_CubicSpace.Intersects(cubicSpace) && !m_CubicSpace.Contains(cubicSpace))
		{
			//LOG("No Objects intersecting this cube! Return value empty");
			return objectsInside;
		}

		std::vector<GameObject*>::iterator it = GObjectsContained_Vector.begin();
		for (; it != GObjectsContained_Vector.end(); it++)
			objectsInside.push_back(*it);

		if (m_ChildsQuantity > 0)
		{
			std::vector<GameObject*> childrenObjects;
			for (uint i = 0; i < m_ChildsQuantity; i++)
			{
				std::vector<GameObject*>nodeObjs = m_Nodes[i].GetObjectsContained(cubicSpace);
				if (nodeObjs.size() > 0)
					childrenObjects.insert(childrenObjects.begin(), nodeObjs.begin(), nodeObjs.end());
			}

			if (childrenObjects.size() > 0)
				objectsInside.insert(objectsInside.begin(), childrenObjects.begin(), childrenObjects.end());
		}

		return objectsInside;
	}


	std::vector<GameObject*> CnOT_Node::GetObjectsContained(const math::Frustum cameraFrustum)
	{
		std::vector<GameObject*> objectsInside;
		if (!m_CubicSpace.Intersects(cameraFrustum) && !m_CubicSpace.Contains(cameraFrustum))
		{
			//LOG("No Objects intersecting this cube! Return value empty");
			return objectsInside;
		}

		std::vector<GameObject*>::iterator it = GObjectsContained_Vector.begin();
		for (; it != GObjectsContained_Vector.end(); it++)
			objectsInside.push_back(*it);

		if (m_ChildsQuantity > 0)
		{
			std::vector<GameObject*> childrenObjects;
			for (uint i = 0; i < m_ChildsQuantity; i++)
			{
				std::vector<GameObject*>nodeObjs = m_Nodes[i].GetObjectsContained(cameraFrustum);
				if (nodeObjs.size() > 0)
					childrenObjects.insert(childrenObjects.begin(), nodeObjs.begin(), nodeObjs.end());
			}

			if (childrenObjects.size() > 0)
				objectsInside.insert(objectsInside.begin(), childrenObjects.begin(), childrenObjects.end());
		}

		return objectsInside;
	}


	void CnOT_Node::Draw()
	{
		if (m_IsChild == false)
			for (int i = 0; i < m_ChildsQuantity; ++i)
				m_Nodes[i].Draw();

		glm::vec3 max = glm::vec3(m_CubicSpace.MaxX(), m_CubicSpace.MaxY(), m_CubicSpace.MaxZ());
		glm::vec3 min = glm::vec3(m_CubicSpace.MinX(), m_CubicSpace.MinY(), m_CubicSpace.MinZ());
		App->renderer3D->DrawCube(max, min, glm::vec3(Red.r, Red.g, Red.b));
	}


	void CnOT_Node::CleanUp()
	{
		for (uint i = 0; i < m_ChildsQuantity; i++)
			m_Nodes[i].CleanUp();

		if (m_Nodes != nullptr)
		{
			delete[] m_Nodes;
			m_Nodes = nullptr;
		}
		
		GObjectsContained_Vector.clear();
	}

	void CnOT_Node::CleanNodes()
	{
		for (uint i = 0; i < m_ChildsQuantity; i++)
			m_Nodes[i].CleanUp();

		if (m_Nodes != nullptr)
		{
			delete[] m_Nodes;
			m_Nodes = nullptr;
		}

		m_ChildsQuantity = 0;
		if (m_NodeType != NodeType::ROOT)
			m_NodeType = NodeType::CHILD;
	}


	void CnOT_Node::Split()
	{
		//Get the current AABB Max and Min
		math::float3 c_Min = m_CubicSpace.minPoint;
		math::float3 c_Max = m_CubicSpace.maxPoint;
		math::float3 max, min;

		//Let's begin
		math::AABB children[8];

		//First Node
		max = c_Max;
		min = { (c_Max.x + c_Min.x) / 2.0f, (c_Min.y + c_Max.y) / 2.0f, (c_Max.z + c_Min.z) / 2.0f };

		///Set new node with those max and min
		children[0] = math::AABB(min, max);

		//Second Node
		max = { (c_Max.x + c_Min.x) / 2.0f, c_Max.y, c_Max.z };
		min = { c_Min.x, (c_Min.y + c_Max.y) / 2.0f, (c_Max.z + c_Min.z) / 2.0f };

		///Set new node with those max and min
		children[1] = math::AABB(min, max);

		//Third Node
		max = { c_Max.x, c_Max.y, (c_Max.z + c_Min.z) / 2.0f };
		min = { (c_Max.x + c_Min.x) / 2.0f, (c_Min.y + c_Max.y) / 2.0f, c_Min.z };

		///Set new node with those max and min
		children[2] = math::AABB(min, max);

		//Fourth Node
		max = { (c_Max.x + c_Min.x) / 2.0f, c_Max.y, (c_Max.z + c_Min.z) / 2.0f };
		min = { c_Min.x, (c_Min.y + c_Max.y) / 2.0f, c_Min.z };

		///Set new node with those max and min
		children[3] = math::AABB(min, max);

		//Fifth Node
		max = { c_Max.x, (c_Min.y + c_Max.y) / 2.0f, c_Max.z };
		min = { (c_Max.x + c_Min.x) / 2.0f, c_Min.y, (c_Max.z + c_Min.z) / 2.0f };

		///Set new node with those max and min
		children[4] = math::AABB(min, max);

		//Sixth Node
		max = { (c_Max.x + c_Min.x) / 2.0f, (c_Min.y + c_Max.y) / 2.0f, c_Max.z };
		min = { c_Min.x, c_Min.y, (c_Max.z + c_Min.z) / 2.0f };

		///Set new node with those max and min
		children[5] = math::AABB(min, max);

		//Seventh Node
		max = { c_Max.x, (c_Min.y + c_Max.y) / 2.0f, (c_Max.z + c_Min.z) / 2.0f };
		min = { (c_Max.x + c_Min.x) / 2.0f, c_Min.y, c_Min.z };

		///Set new node with those max and min
		children[6] = math::AABB(min, max);

		//Eight and Final Node
		max = { (c_Max.x + c_Min.x) / 2.0f, (c_Min.y + c_Min.y) / 2.0f, (c_Max.z + c_Min.z) / 2.0f };
		min = { c_Min.x, c_Min.y, c_Min.z };

		///Set new node with those max and min
		children[7] = math::AABB(min, max);

		//Create those new nodes as actual Octree Nodes ---------------------------------------------------------
		if (m_NodeType != NodeType::ROOT)
			m_NodeType = NodeType::PARENT;

		m_ChildsQuantity = 8;
		m_IsChild = false;
		m_Nodes = new CnOT_Node[8];
		for (int i = 0; i < 8; i++)
			m_Nodes[i] = CnOT_Node(children[i], NodeType::CHILD, m_MaxObjectsInside);
	}


	bool CnOT_Node::Insert(GameObject* GObj)
	{
		math::AABB GOAABB = GObj->GetAABB();
		if (!m_CubicSpace.Intersects(GOAABB))
			return false;

		int nodesContained = 0, container = 0;
		if (m_NodeType == NodeType::PARENT || (m_NodeType == NodeType::ROOT && m_ChildsQuantity > 0))
		{
			for (uint i = 0; i < m_ChildsQuantity; i++)
			{
				if (m_Nodes[i].m_CubicSpace.Intersects(GOAABB))
				{
					nodesContained++;
					container = i;
				}

				if (nodesContained > 1)
					break;
			}

			if (nodesContained == 1)
				m_Nodes[container].Insert(GObj);
			else if (nodesContained > 1)
				GObjectsContained_Vector.push_back(GObj);

			return true;
		}
		else if (m_NodeType == NodeType::CHILD || (m_NodeType == NodeType::ROOT && m_ChildsQuantity == 0))
		{
			GObjectsContained_Vector.push_back(GObj);
			if (GObjectsContained_Vector.size() > m_MaxObjectsInside)
			{
				Split();

				std::vector<GameObject*> newObjects = GObjectsContained_Vector;
				GObjectsContained_Vector.clear();

				for (uint j = 0; j < newObjects.size(); j++)
				{
					nodesContained = 0;
					container = 0;
					for (uint i = 0; i < m_ChildsQuantity; i++)
					{
						if (m_Nodes[i].m_CubicSpace.Intersects(newObjects[j]->GetAABB()))
						{
							nodesContained++;
							container = i;
						}
						if(nodesContained > 1)
							break;
					}

					if (nodesContained == 1)
						m_Nodes[container].Insert(newObjects[j]);
					else if (nodesContained > 1)
						GObjectsContained_Vector.push_back(newObjects[j]);
				}
			}
			return true;
		}

		return false;
	}


	void CnOT_Node::TakeOut(GameObject* GObject)
	{
		for (int i = 0; i < GObjectsContained_Vector.size(); ++i)
		{
			if (GObjectsContained_Vector[i] != GObject)
				continue;

			GObjectsContained_Vector.erase(GObjectsContained_Vector.begin() + i);
			return;			
		}

		if (m_ChildsQuantity > 0)
		{
			for (int i = 0; i < m_ChildsQuantity; ++i)
				m_Nodes[i].TakeOut(GObject);

			//In case the subnodes are the leaves, and become empty
			//after erasing the object, clear them
			if (m_Nodes[0].m_NodeType == NodeType::CHILD)
			{
				bool nodesEmpty = true;
				for (int i = 0; i < m_ChildsQuantity; ++i)
				{
					if (m_Nodes[i].GObjectsContained_Vector.empty() == false)
					{
						nodesEmpty = false;
						break;
					}
				}
				if (nodesEmpty)
					CleanNodes();
			}
		}
	}
}
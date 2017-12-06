#include "MyOctree.h"
using namespace Simplex;

uint MyOctant::m_uMaxDepth;
uint MyOctant::m_uIdealCount;
uint MyOctant::m_uNodeCount;
uint MyOctant::m_uCurrentID;


MyOctant::MyOctant(uint a_uMaxDepth, uint a_uIdealCount)
{
	m_pEntityMngr = EntityManager::GetInstance();
	m_pMeshMngr = MeshManager::GetInstance();

	m_uMaxDepth = a_uMaxDepth;
	m_uIdealCount = a_uIdealCount;

	Init();

}

MyOctant::MyOctant(vector3 a_v3Center, float a_v3Width)
{
	m_pEntityMngr = EntityManager::GetInstance();
	m_pMeshMngr = MeshManager::GetInstance();

	m_uID = m_uNodeCount;
	m_uNodeCount++;

	m_v3Max = a_v3Center + a_v3Width;
	m_v3Min = a_v3Center - a_v3Width;

	m_v3Center = a_v3Center;
	m_v3Width = vector3(a_v3Width);
}

MyOctant::MyOctant(vector3 a_v3Max, vector3 a_v3Min, float a_v3Width)
{
	m_pEntityMngr = EntityManager::GetInstance();
	m_pMeshMngr = MeshManager::GetInstance();

	m_uID = m_uNodeCount;
	m_uNodeCount++;

	m_v3Max = a_v3Max;
	m_v3Min = a_v3Min;

	m_v3Center = (m_v3Max + m_v3Min) / 2.f;

	m_v3Width = vector3(a_v3Width);
}

MyOctant::MyOctant(MyOctant const& other)
{
	m_uID = other.m_uID;
	m_uMaxDepth = other.m_uMaxDepth;
	m_uChildCount = other.m_uChildCount;

	m_v3Width = other.m_v3Width;
	
}


Simplex::MyOctant::~MyOctant(void)
{
	Release();
}

void Simplex::MyOctant::Swap(MyOctant & other)
{
}

vector3 Simplex::MyOctant::GetWidth(void)
{
	return m_v3Width;
}

vector3 Simplex::MyOctant::GetGlobalCenter(void)
{
	return m_v3Center;
}

vector3 Simplex::MyOctant::GetGlobalMin(void)
{
	return m_v3Min;
}

vector3 Simplex::MyOctant::GetGlobalMax(void)
{
	return m_v3Max;
}

uint Simplex::MyOctant::GetOctantCount(void)
{
	return m_uNodeCount;
}

bool Simplex::MyOctant::IsWithinOctant(uint a_uIndex)
{
	vector3 Pos = m_pEntityMngr->GetEntity(a_uIndex)->GetPosition();

	if (Pos.x >= m_v3Min.x && Pos.x <= m_v3Max.x)
	{
		if (Pos.y >= m_v3Min.y && Pos.y <= m_v3Max.y)
		{
			if (Pos.z >= m_v3Min.z && Pos.z <= m_v3Max.z)
			{
				return true;
			}
		}
	}

	return false;
}

void Simplex::MyOctant::Display(uint a_nIndex, vector3 a_v3Color)
{
	m_pMeshMngr->AddWireCubeToRenderList(glm::translate(m_v3Center) * glm::scale(m_v3Width), a_v3Color);
}

void Simplex::MyOctant::Display(vector3 a_v3Color)
{
	m_pMeshMngr->AddWireCubeToRenderList(glm::translate(m_v3Center) * glm::scale(m_v3Width), a_v3Color);

	if (m_uChildCount != 0)
	{
		for (int i = 0; i < 8; i++)
		{
			m_pChildren[i]->Display(C_YELLOW);
		}
	}
}

void Simplex::MyOctant::DisplayLeafs(vector3 a_v3Color)
{
	if (m_uID == m_uCurrentID)
	{
		m_pMeshMngr->AddWireCubeToRenderList(glm::translate(m_v3Center) * glm::scale(m_v3Width), a_v3Color);
	}

	if (!IsLeaf())
	{
		for (int i = 0; i < 8; i++)
		{
			m_pChildren[i]->DisplayLeafs();
		}
	}
}

void Simplex::MyOctant::ClearEntityList(void)
{
	m_auObjectsInNode.clear();
}

void Simplex::MyOctant::AddChildren(void)
{

	float m_fSize = m_v3Width.x;

	m_pChildren[0] = new MyOctant(m_v3Max, m_v3Center, m_fSize / 2.0f);
	m_pChildren[1] = new MyOctant(vector3(m_v3Max.x, m_v3Max.y, m_v3Center.z), vector3(m_v3Center.x, m_v3Center.y, m_v3Min.z), m_fSize / 2.0f);
	m_pChildren[2] = new MyOctant(vector3(m_v3Center.x, m_v3Max.y, m_v3Center.z), vector3(m_v3Min.x, m_v3Center.y, m_v3Min.z), m_fSize / 2.0f);
	m_pChildren[3] = new MyOctant(vector3(m_v3Center.x, m_v3Max.y, m_v3Max.z), vector3(m_v3Min.x, m_v3Center.y, m_v3Center.z), m_fSize / 2.0f);
	m_pChildren[4] = new MyOctant(vector3(m_v3Max.x, m_v3Center.y, m_v3Max.z), vector3(m_v3Center.x, m_v3Min.y, m_v3Center.z), m_fSize / 2.0f);
	m_pChildren[5] = new MyOctant(vector3(m_v3Max.x, m_v3Center.y, m_v3Center.z), vector3(m_v3Center.x, m_v3Min.y, m_v3Min.z), m_fSize / 2.0f);
	m_pChildren[6] = new MyOctant(m_v3Center, m_v3Min, m_fSize / 2.0f);
	m_pChildren[7] = new MyOctant(vector3(m_v3Center.x, m_v3Center.y, m_v3Max.z), vector3(m_v3Min.x, m_v3Min.y, m_v3Center.z), m_fSize / 2.0f);


	for (int i = 0; i < 8; i++)
	{
		m_pChildren[i]->m_pParent = this;
		m_uChildCount++;
	}
}

MyOctant * Simplex::MyOctant::GetChild(uint a_uChild)
{
	return m_pChildren[a_uChild];
}

MyOctant * Simplex::MyOctant::GetParent(void)
{
	return m_pParent;
}

bool Simplex::MyOctant::IsLeaf(void)
{
	return m_uChildCount == 0;
}

bool Simplex::MyOctant::Overflowing(uint a_uEntities)
{
	return a_uEntities > m_auObjectsInNode.size();
}

void Simplex::MyOctant::RemoveChildren(void)
{
	// Loop through the children of this node
	for (int i = 0; i < 8; i++)
	{
		if (m_pChildren[i] != nullptr)
		{
			// Remove the children from this node
			m_pChildren[i]->RemoveChildren();

			// Then delete this node and set it's pointer to be a nullptr
			delete m_pChildren[i];
			m_pChildren[i] = nullptr;
		}
	}
}

void Simplex::MyOctant::Build(uint a_uMaxDepth)
{
	if (a_uMaxDepth <= 1) { return; }

	AssignIDToEntity();
	if (m_auObjectsInNode.size() > m_uIdealCount)
	{
		m_auObjectsInNode.clear();
		ClearAllEntityIDs();
		AddChildren();

		for (uint i = 0; i < 8; i++)
		{
			m_pChildren[i]->Build(a_uMaxDepth - 1);
		}

		AssignIDToEntity();
	}
}

void Simplex::MyOctant::AssignIDToEntity(void)
{
	if (m_uChildCount != 0)
	{
		for (uint i = 0; i < 8; i++)
		{
			m_pChildren[i]->AssignIDToEntity();
		}
	}
	else
	{
		for (uint i = 0; i < m_pEntityMngr->GetEntityCount(); i++)
		{
			if (IsWithinOctant(i))
			{
				m_pEntityMngr->GetEntity(i)->AddDimension(m_uID);
				m_auObjectsInNode.push_back(i);
			}
		}
	}
}

void Simplex::MyOctant::ClearAllEntityIDs(void)
{
	for (uint i = 0; i < m_pEntityMngr->GetEntityCount(); i++)
	{
		m_pEntityMngr->GetEntity(i)->ClearDimensionSet();
	}
}

uint Simplex::MyOctant::GetCurrentOctantId(void)
{
	return m_uCurrentID;
}

void Simplex::MyOctant::SetCurrentOctantID(uint a_uID)
{
	m_uCurrentID = a_uID;
}

void Simplex::MyOctant::Release(void)
{
	for (int i = 0; i < m_pEntityMngr->GetEntityCount(); i++)
	{
		m_pEntityMngr->GetEntity(i)->ClearDimensionSet();
	}

	// Remove all ChildNodes
	RemoveChildren();

	// Set the Mesh and Entity Managers to be nullptrs
	m_pMeshMngr = nullptr;
	m_pEntityMngr = nullptr;

	// Get rid of our root, octantCount, and id
	m_uNodeCount = 0;
	m_uID = 0;
	m_pRoot = nullptr;
}

void Simplex::MyOctant::Init(void)
{
	m_uID = m_uNodeCount;
	m_uNodeCount++;

	std::vector<vector3> minMaxList;	

										
	for (int i = 0; i < m_pEntityMngr->GetEntityCount(); i++)
	{
		minMaxList.push_back(m_pEntityMngr->GetRigidBody(i)->GetMinGlobal());
		minMaxList.push_back(m_pEntityMngr->GetRigidBody(i)->GetMaxGlobal());
		m_auObjectsInNode.push_back(i);
	}

	
	RigidBody* referenceRB = new RigidBody(minMaxList);

	
	m_v3Max = referenceRB->GetMaxGlobal();
	m_v3Min = referenceRB->GetMinGlobal();

	
	SafeDelete(referenceRB);
	minMaxList.clear();

	
	m_v3Center = (m_v3Max + m_v3Min) / 2.0f;
	m_v3Width = m_v3Max - m_v3Min;

	Build(m_uMaxDepth);
}

uint Simplex::MyOctant::GetCurrentOctantID(void)
{
	return m_uCurrentID;
}

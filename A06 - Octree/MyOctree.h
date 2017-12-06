#ifndef _MY_OCTREE_H
#define _MY_OCTREE_H

#include "Simplex\Physics\EntityManager.h";

namespace Simplex
{
	class MyOctant
	{
		static uint m_uMaxDepth;
		static uint m_uIdealCount;
		static uint m_uCurrentID;
		static uint m_uNodeCount;

		MyOctant* m_pRoot = nullptr;
		MyOctant* m_pParent = nullptr;
		MyOctant* m_pChildren[8];

		vector3 m_v3Max;
		vector3 m_v3Min;
		vector3 m_v3Center;
		vector3 m_v3Width;

		MeshManager* m_pMeshMngr;
		EntityManager* m_pEntityMngr;

		std::vector<uint> m_auObjectsInNode;

		uint m_uID = 0;
		uint m_uDepth = 0;
		uint m_uChildCount = 0;

	public:

		MyOctant(uint a_uMaxDepth = 2, uint a_uIdealCount = 5);

		MyOctant(vector3 a_v3Center, float a_v3Width);

		MyOctant(vector3 a_v3Max, vector3 a_v3Min, float a_v3Width);

		MyOctant(MyOctant const& other);

		MyOctant& operator=(MyOctant const& other);

		~MyOctant(void);

		void Swap(MyOctant& other);

		vector3 GetWidth(void);

		vector3 GetGlobalCenter(void);

		vector3 GetGlobalMin(void);

		vector3 GetGlobalMax(void);

		uint GetOctantCount(void);

		bool IsWithinOctant(uint a_uIndex);

		void Display(uint a_nIndex, vector3 a_v3Color = C_YELLOW);

		void Display(vector3 a_v3Color = C_YELLOW);

		void DisplayLeafs(vector3 a_v3Color = C_YELLOW);

		void ClearEntityList(void);

		void AddChildren(void);

		MyOctant* GetChild(uint a_uChild);

		MyOctant* GetParent(void);

		bool IsLeaf(void);

		bool Overflowing(uint a_uEntities);

		void RemoveChildren(void);

		void Build(uint a_uMaxDepth = 3);

		void AssignIDToEntity(void);

		void ClearAllEntityIDs(void);

		void GetMyOctantCount(void);

		uint GetCurrentOctantId(void);

		void SetCurrentOctantID(uint a_uID);

		uint Simplex::MyOctant::GetCurrentOctantID(void);

	private:

		void Release(void);

		void Init(void);

		void ConstructList(void);



	};
}

#endif //_MY_OCTREE_H
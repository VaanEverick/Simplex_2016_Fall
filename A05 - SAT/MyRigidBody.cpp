#include "MyRigidBody.h"
using namespace Simplex;
//Allocation
void MyRigidBody::Init(void)
{
	m_pMeshMngr = MeshManager::GetInstance();
	m_bVisibleBS = false;
	m_bVisibleOBB = true;
	m_bVisibleARBB = false;

	m_fRadius = 0.0f;

	m_v3ColorColliding = C_RED;
	m_v3ColorNotColliding = C_WHITE;

	m_v3Center = ZERO_V3;
	m_v3MinL = ZERO_V3;
	m_v3MaxL = ZERO_V3;

	m_v3MinG = ZERO_V3;
	m_v3MaxG = ZERO_V3;

	m_v3HalfWidth = ZERO_V3;
	m_v3ARBBSize = ZERO_V3;

	m_m4ToWorld = IDENTITY_M4;
}
void MyRigidBody::Swap(MyRigidBody& a_pOther)
{
	std::swap(m_pMeshMngr, a_pOther.m_pMeshMngr);
	std::swap(m_bVisibleBS, a_pOther.m_bVisibleBS);
	std::swap(m_bVisibleOBB, a_pOther.m_bVisibleOBB);
	std::swap(m_bVisibleARBB, a_pOther.m_bVisibleARBB);

	std::swap(m_fRadius, a_pOther.m_fRadius);

	std::swap(m_v3ColorColliding, a_pOther.m_v3ColorColliding);
	std::swap(m_v3ColorNotColliding, a_pOther.m_v3ColorNotColliding);

	std::swap(m_v3Center, a_pOther.m_v3Center);
	std::swap(m_v3MinL, a_pOther.m_v3MinL);
	std::swap(m_v3MaxL, a_pOther.m_v3MaxL);

	std::swap(m_v3MinG, a_pOther.m_v3MinG);
	std::swap(m_v3MaxG, a_pOther.m_v3MaxG);

	std::swap(m_v3HalfWidth, a_pOther.m_v3HalfWidth);
	std::swap(m_v3ARBBSize, a_pOther.m_v3ARBBSize);

	std::swap(m_m4ToWorld, a_pOther.m_m4ToWorld);

	std::swap(m_CollidingRBSet, a_pOther.m_CollidingRBSet);
}
void MyRigidBody::Release(void)
{
	m_pMeshMngr = nullptr;
	ClearCollidingList();
}
//Accessors
bool MyRigidBody::GetVisibleBS(void) { return m_bVisibleBS; }
void MyRigidBody::SetVisibleBS(bool a_bVisible) { m_bVisibleBS = a_bVisible; }
bool MyRigidBody::GetVisibleOBB(void) { return m_bVisibleOBB; }
void MyRigidBody::SetVisibleOBB(bool a_bVisible) { m_bVisibleOBB = a_bVisible; }
bool MyRigidBody::GetVisibleARBB(void) { return m_bVisibleARBB; }
void MyRigidBody::SetVisibleARBB(bool a_bVisible) { m_bVisibleARBB = a_bVisible; }
float MyRigidBody::GetRadius(void) { return m_fRadius; }
vector3 MyRigidBody::GetColorColliding(void) { return m_v3ColorColliding; }
vector3 MyRigidBody::GetColorNotColliding(void) { return m_v3ColorNotColliding; }
void MyRigidBody::SetColorColliding(vector3 a_v3Color) { m_v3ColorColliding = a_v3Color; }
void MyRigidBody::SetColorNotColliding(vector3 a_v3Color) { m_v3ColorNotColliding = a_v3Color; }
vector3 MyRigidBody::GetCenterLocal(void) { return m_v3Center; }
vector3 MyRigidBody::GetMinLocal(void) { return m_v3MinL; }
vector3 MyRigidBody::GetMaxLocal(void) { return m_v3MaxL; }
vector3 MyRigidBody::GetCenterGlobal(void){	return vector3(m_m4ToWorld * vector4(m_v3Center, 1.0f)); }
vector3 MyRigidBody::GetMinGlobal(void) { return m_v3MinG; }
vector3 MyRigidBody::GetMaxGlobal(void) { return m_v3MaxG; }
vector3 MyRigidBody::GetHalfWidth(void) { return m_v3HalfWidth; }
matrix4 MyRigidBody::GetModelMatrix(void) { return m_m4ToWorld; }
void MyRigidBody::SetModelMatrix(matrix4 a_m4ModelMatrix)
{
	//to save some calculations if the model matrix is the same there is nothing to do here
	if (a_m4ModelMatrix == m_m4ToWorld)
		return;

	//Assign the model matrix
	m_m4ToWorld = a_m4ModelMatrix;

	//Calculate the 8 corners of the cube
	
	//Back square
	m_v3Corners[0] = m_v3MinL;
	m_v3Corners[1] = vector3(m_v3MaxL.x, m_v3MinL.y, m_v3MinL.z);
	m_v3Corners[2] = vector3(m_v3MinL.x, m_v3MaxL.y, m_v3MinL.z);
	m_v3Corners[3] = vector3(m_v3MaxL.x, m_v3MaxL.y, m_v3MinL.z);

	//Front square
	m_v3Corners[4] = vector3(m_v3MinL.x, m_v3MinL.y, m_v3MaxL.z);
	m_v3Corners[5] = vector3(m_v3MaxL.x, m_v3MinL.y, m_v3MaxL.z);
	m_v3Corners[6] = vector3(m_v3MinL.x, m_v3MaxL.y, m_v3MaxL.z);
	m_v3Corners[7] = m_v3MaxL;

	//Place them in world space
	for (uint uIndex = 0; uIndex < 8; ++uIndex)
	{
		m_v3Corners[uIndex] = vector3(m_m4ToWorld * vector4(m_v3Corners[uIndex], 1.0f));
	}

	//Identify the max and min as the first corner
	m_v3MaxG = m_v3MinG = m_v3Corners[0];

	//get the new max and min for the global box
	for (uint i = 1; i < 8; ++i)
	{
		if (m_v3MaxG.x < m_v3Corners[i].x) m_v3MaxG.x = m_v3Corners[i].x;
		else if (m_v3MinG.x > m_v3Corners[i].x) m_v3MinG.x = m_v3Corners[i].x;

		if (m_v3MaxG.y < m_v3Corners[i].y) m_v3MaxG.y = m_v3Corners[i].y;
		else if (m_v3MinG.y >m_v3Corners[i].y) m_v3MinG.y = m_v3Corners[i].y;

		if (m_v3MaxG.z < m_v3Corners[i].z) m_v3MaxG.z = m_v3Corners[i].z;
		else if (m_v3MinG.z > m_v3Corners[i].z) m_v3MinG.z = m_v3Corners[i].z;
	}

	//we calculate the distance between min and max vectors
	m_v3ARBBSize = m_v3MaxG - m_v3MinG;
}
//The big 3
MyRigidBody::MyRigidBody(std::vector<vector3> a_pointList)
{
	Init();
	//Count the points of the incoming list
	uint uVertexCount = a_pointList.size();

	//If there are none just return, we have no information to create the BS from
	if (uVertexCount == 0)
		return;

	//Max and min as the first vector of the list
	m_v3MaxL = m_v3MinL = a_pointList[0];

	//Get the max and min out of the list
	for (uint i = 1; i < uVertexCount; ++i)
	{
		if (m_v3MaxL.x < a_pointList[i].x) m_v3MaxL.x = a_pointList[i].x;
		else if (m_v3MinL.x > a_pointList[i].x) m_v3MinL.x = a_pointList[i].x;

		if (m_v3MaxL.y < a_pointList[i].y) m_v3MaxL.y = a_pointList[i].y;
		else if (m_v3MinL.y > a_pointList[i].y) m_v3MinL.y = a_pointList[i].y;

		if (m_v3MaxL.z < a_pointList[i].z) m_v3MaxL.z = a_pointList[i].z;
		else if (m_v3MinL.z > a_pointList[i].z) m_v3MinL.z = a_pointList[i].z;
	}

	//with model matrix being the identity, local and global are the same
	m_v3MinG = m_v3MinL;
	m_v3MaxG = m_v3MaxL;

	//with the max and the min we calculate the center
	m_v3Center = (m_v3MaxL + m_v3MinL) / 2.0f;

	//we calculate the distance between min and max vectors
	m_v3HalfWidth = (m_v3MaxL - m_v3MinL) / 2.0f;

	//Get the distance between the center and either the min or the max
	m_fRadius = glm::distance(m_v3Center, m_v3MinL);
}
MyRigidBody::MyRigidBody(MyRigidBody const& a_pOther)
{
	m_pMeshMngr = a_pOther.m_pMeshMngr;

	m_bVisibleBS = a_pOther.m_bVisibleBS;
	m_bVisibleOBB = a_pOther.m_bVisibleOBB;
	m_bVisibleARBB = a_pOther.m_bVisibleARBB;

	m_fRadius = a_pOther.m_fRadius;

	m_v3ColorColliding = a_pOther.m_v3ColorColliding;
	m_v3ColorNotColliding = a_pOther.m_v3ColorNotColliding;

	m_v3Center = a_pOther.m_v3Center;
	m_v3MinL = a_pOther.m_v3MinL;
	m_v3MaxL = a_pOther.m_v3MaxL;

	m_v3MinG = a_pOther.m_v3MinG;
	m_v3MaxG = a_pOther.m_v3MaxG;

	m_v3HalfWidth = a_pOther.m_v3HalfWidth;
	m_v3ARBBSize = a_pOther.m_v3ARBBSize;

	m_m4ToWorld = a_pOther.m_m4ToWorld;

	m_CollidingRBSet = a_pOther.m_CollidingRBSet;
}
MyRigidBody& MyRigidBody::operator=(MyRigidBody const& a_pOther)
{
	if (this != &a_pOther)
	{
		Release();
		Init();
		MyRigidBody temp(a_pOther);
		Swap(temp);
	}
	return *this;
}
MyRigidBody::~MyRigidBody() { Release(); };
//--- a_pOther Methods
void MyRigidBody::AddCollisionWith(MyRigidBody* a_pOther)
{
	/*
		check if the object is already in the colliding set, if
		the object is already there return with no changes
	*/
	auto element = m_CollidingRBSet.find(a_pOther);
	if (element != m_CollidingRBSet.end())
		return;
	// we couldn't find the object so add it
	m_CollidingRBSet.insert(a_pOther);
}
void MyRigidBody::RemoveCollisionWith(MyRigidBody* a_pOther)
{
	m_CollidingRBSet.erase(a_pOther);
}
void MyRigidBody::ClearCollidingList(void)
{
	m_CollidingRBSet.clear();
}
bool MyRigidBody::IsColliding(MyRigidBody* const a_pOther)
{
	//check if spheres are colliding as pre-test
	bool bColliding = (glm::distance(GetCenterGlobal(), a_pOther->GetCenterGlobal()) < m_fRadius + a_pOther->m_fRadius);
	
	//if they are colliding check the SAT
	if (bColliding)
	{
		if(SAT(a_pOther) != eSATResults::SAT_NONE)
			bColliding = false;// reset to false
		MeshManager *m_pManager;
	}

	if (bColliding) //they are colliding
	{
		this->AddCollisionWith(a_pOther);
		a_pOther->AddCollisionWith(this);
	}
	else //they are not colliding
	{
		this->RemoveCollisionWith(a_pOther);
		a_pOther->RemoveCollisionWith(this);
	}

	return bColliding;
}
void MyRigidBody::AddToRenderList(void)
{
	if (m_bVisibleBS)
	{
		if (m_CollidingRBSet.size() > 0)
			m_pMeshMngr->AddWireSphereToRenderList(glm::translate(m_m4ToWorld, m_v3Center) * glm::scale(vector3(m_fRadius)), C_BLUE_CORNFLOWER);
		else
			m_pMeshMngr->AddWireSphereToRenderList(glm::translate(m_m4ToWorld, m_v3Center) * glm::scale(vector3(m_fRadius)), C_BLUE_CORNFLOWER);
	}
	if (m_bVisibleOBB)
	{
		if (m_CollidingRBSet.size() > 0)
			m_pMeshMngr->AddWireCubeToRenderList(glm::translate(m_m4ToWorld, m_v3Center) * glm::scale(m_v3HalfWidth * 2.0f), m_v3ColorColliding);
		else
			m_pMeshMngr->AddWireCubeToRenderList(glm::translate(m_m4ToWorld, m_v3Center) * glm::scale(m_v3HalfWidth * 2.0f), m_v3ColorNotColliding);
	}
	if (m_bVisibleARBB)
	{
		if (m_CollidingRBSet.size() > 0)
			m_pMeshMngr->AddWireCubeToRenderList(glm::translate(GetCenterGlobal()) * glm::scale(m_v3ARBBSize), C_YELLOW);
		else
			m_pMeshMngr->AddWireCubeToRenderList(glm::translate(GetCenterGlobal()) * glm::scale(m_v3ARBBSize), C_YELLOW);
	}
}

uint MyRigidBody::SAT(MyRigidBody* const a_pOther)
{
	//This is an optomistic method, meaning it has a series of bailout points as soon as it realizes that the end result is a non-collision

	std::vector<vector3> axes; 

	axes.push_back(glm::normalize(m_v3Corners[1] - m_v3Corners[0]));	//This->X axis
	axes.push_back(glm::normalize(m_v3Corners[2] - m_v3Corners[0]));	//This->Y axis
	axes.push_back(glm::normalize(m_v3Corners[4] - m_v3Corners[0]));	//This->Z axis

	axes.push_back(glm::normalize(a_pOther->m_v3Corners[1] - a_pOther->m_v3Corners[0]));	//Other->X axis
	axes.push_back(glm::normalize(a_pOther->m_v3Corners[2] - a_pOther->m_v3Corners[0]));	//Other->Y axis
	axes.push_back(glm::normalize(a_pOther->m_v3Corners[4] - a_pOther->m_v3Corners[0]));	//Other->Z axis

	//check intersects on each projection using a pair of min and max values
	float fMin1;
	float fMin2;
	float fMax1;
	float fMax2;

	//Loop through each axis with each RigidBody to check for an obvious seperation
	//Saves a lot of hassle with some quick checks
	for (uint i = 0; i < 6; i++)
	{
		GetMinMax(m_v3Corners, axes[i], fMin1, fMax1);
		GetMinMax(a_pOther->m_v3Corners, axes[i], fMin2, fMax2);

		//if the new max values are less than the new min values, there is a clear seperation on that axis
		//A plane exists, return a number
		if (fMax1 < fMin2 || fMax2 < fMin1)
		{
			switch (i)
			{
			case 0: return eSATResults::SAT_AX; break;
			case 1: return eSATResults::SAT_AY; break;
			case 2: return eSATResults::SAT_AZ; break;
			case 3: return eSATResults::SAT_BX; break;
			case 4: return eSATResults::SAT_BY; break;
			case 5: return eSATResults::SAT_BZ; break;
			} //end plane switch
		} //end if overlap
	} // end for axis

	//If you're hear, it means the cheap way wasn't enough, and we have to start checking some serious projections
	//An internal return on the for loop ahead will return an eSATResult if the cross product detects a seperation plane

	//This is our test axis. we will be checking it as a cross product result to determine if we have to keep moving through the list
	vector3 v3Axis;

	int k = 6; //Newly added axes updates

	for (uint i = 0; i <= 2; i++) //This-> axis updates
	{
		for (uint j = 3; j <= 5; j++) //Other-> axis updates
		{
			v3Axis = glm::cross(axes[i], axes[j]);

			if (v3Axis != vector3(0.f, 0.f, 0.f))
			{
				axes.push_back(glm::normalize(v3Axis));
				GetMinMax(m_v3Corners, axes[k], fMin1, fMax1);
				GetMinMax(a_pOther->m_v3Corners, axes[k], fMin2, fMax2);
				if (fMax1 < fMin2 || fMax2 < fMin1)
				{
					return k + 1;
				}
				k++;
			} //end if
		} // end Other updates
	} //end this updates

	//there is no axis test that separates this two objects
	return eSATResults::SAT_NONE;
}

void Simplex::MyRigidBody::GetMinMax(vector3 * a_pv3Corners, vector3 a_v3Axis, float & a_fMin, float & a_fMax)
{
	a_fMin = std::numeric_limits<float>::max();
	a_fMax = -std::numeric_limits<float>::max();

	for (uint i = 0; i < 8; i++)
	{
		float fDot = glm::dot(a_v3Axis, a_pv3Corners[i]);

		//set min and max with this dot result using a min/max comparison
		a_fMin = glm::min(a_fMin, fDot);
		a_fMax = glm::max(a_fMax, fDot);

	}
}

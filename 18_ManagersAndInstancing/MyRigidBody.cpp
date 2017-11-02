#include "MyRigidBody.h";

MyRigidBody::MyRigidBody(Simplex::MyMesh* a_pObject)
{

	if (a_pObject == nullptr) { return; }

	std::vector<Simplex::vector3> pointsList = a_pObject->GetVertexList();

	if (pointsList.size() < 1) { return; }
	
	m_poBoundingSphere = new Simplex::Mesh();
	m_BB = new Simplex::Mesh();
	
	m_v3Min = pointsList[0];
	m_v3Max = pointsList[0];

	for (Simplex::uint i = 1; i < pointsList.size(); i++)
	{
		if (pointsList[i].x < m_v3Min.x) { m_v3Min.x = pointsList[i].x; }
		else if (pointsList[i].x > m_v3Max.x) { m_v3Max.x = pointsList[i].x; }
		if (pointsList[i].y < m_v3Min.y) { m_v3Min.y = pointsList[i].y; }
		else if (pointsList[i].y > m_v3Max.y) { m_v3Max.y = pointsList[i].y; }
		if (pointsList[i].z < m_v3Min.z) { m_v3Min.z = pointsList[i].z; }
		else if (pointsList[i].z > m_v3Max.z) { m_v3Max.z = pointsList[i].z; }
	}

	m_v3Center = m_v3Min + m_v3Max;
	m_v3Center /= 2.f;
	float fRadius = glm::distance(m_v3Center, m_v3Max);

	//m_poBoundingSphere->GenerateIcoSphere(fRadius, 2, Simplex::C_BLUE);

	
	m_BB->GenerateCube(fRadius, Simplex::C_WHITE);
}

void MyRigidBody::Render(Simplex::MyCamera* a_pCamera, Simplex::matrix4 a_m4Model)
{
	Simplex::vector3 v3Size = m_v3Max - m_v3Min;
	Simplex::matrix4 resize = a_m4Model * glm::scale(Simplex::IDENTITY_M4, Simplex::vector3());
	memcpy(matrix, glm::value_ptr(resize), 16 * sizeof(float));

	float* matrix = new float[16];
	memcpy(matrix, glm::value_ptr(a_m4Model), 16 * sizeof(float));
	//m_poBoundingSphere->RenderWire(a_pCamera->GetProjectionMatrix(), a_pCamera->GetViewMatrix(), matrix, 1);
	m_BB->RenderWire(a_pCamera->GetProjectionMatrix(), a_pCamera->GetViewMatrix(), matrix, 1);
	SafeDelete(matrix);
}

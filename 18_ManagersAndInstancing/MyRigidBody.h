#ifndef MYRIGIDBODY_H
#define MYRIGIDBODY_H

#include "MyMesh.h"
#include "MyCamera.h"
#include "Definitions.h"

class MyRigidBody
{
public:


	Simplex::MeshManager* m_pMeshManager;
	Simplex::Mesh* m_poBoundingSphere = nullptr;
	Simplex::Mesh* m_BB = nullptr;

	bool m_bVisible = true;
	float m_fRadius = 0.0f;

	Simplex::vector3 m_v3Colour = Simplex::C_WHITE;

	Simplex::vector3 m_v3Min;
	Simplex::vector3 m_v3Max;
	Simplex::vector3 m_v3Center;

	MyRigidBody(Simplex::MyMesh* a_pObject);
	void Render(Simplex::MyCamera* a_pCamera, Simplex::matrix4 a_m4Model);

};

#endif // !MYRIGIDBODY_H
#include "AppClass.h"
void Application::InitVariables(void)
{
	////Change this to your name and email
	m_sProgrammer = "Christopher Banks - cdb6474@g.rit.edu";

	////Alberto needed this at this position for software recording.
	//m_pWindow->setPosition(sf::Vector2i(710, 0));
	
	//init the mesh
	m_pMesh = new MyMesh();
	//m_pMesh->GenerateCube(1.0f, C_WHITE);
	m_pMesh->GenerateCube(1.0f, C_WHITE);
	for (int i = 0; i < 46; i++)
	{
		m_pMeshes.push_back(new MyMesh(*m_pMesh));
	}
	locs = {2, 8, 14, 18, 24, 25, 26, 27,28,29,30,34,35,37,38,39,41,42,44,45,46,47,48,49,50,51,52,53,54,55,57,58,59,60,61,62,63,65,66,68,74,76,80,81,83,84};
	m_fSinValue = 0.0f;
	m_fCosValue = 1.0f;
	m_fRadIterator = 0.f;
	m_vOrigin = vector2(1, 1);
}
void Application::Update(void)
{
	//Update the system so it knows how much time has passed since the last call
	m_pSystem->Update();

	//Is the arcball active?
	ArcBall();

	//Is the first person camera active?
	CameraRotation();
}
void Application::Display(void)
{
	// Clear the screen
	ClearScreen();

	matrix4 m4View = m_pCameraMngr->GetViewMatrix();
	matrix4 m4Projection = m_pCameraMngr->GetProjectionMatrix();
	
	matrix4 m4Scale = glm::scale(IDENTITY_M4, vector3(2.0f,2.0f,2.0f));
	static float value = 0.0f;
	for (int row = 0; row < 8; row++)
	{
		for (int col = 0; col < 11; col++)
		{
			if (locs.find(11 * row + col) != locs.end())
			{
				matrix4 m4Translate = glm::translate(IDENTITY_M4, vector3((col + m_vOrigin.x), (-row), 3.0f));

				//matrix4 m4Model = m4Translate * m4Scale;
				matrix4 m4Model = m4Scale * m4Translate;

				m_pMesh->Render(m4Projection, m4View, m4Model);
			}
		}
	}

	m_fSinValue = std::sin(((360.f / (float)180.f) * m_fRadIterator) * (PI / 180.f));
	m_fCosValue = std::cos(((360 / (float)180.f) * m_fRadIterator) * (PI / 180.f));

	m_fRadIterator++;
	m_vOrigin.x+= .01f;

	if (m_fRadIterator > 360)
	{
		m_fRadIterator = 0;
	}
	// draw a skybox
	m_pMeshMngr->AddSkyboxToRenderList();
	
	//render list call
	m_uRenderCallCount = m_pMeshMngr->Render();

	//clear the render list
	m_pMeshMngr->ClearRenderList();
	
	//draw gui
	DrawGUI();
	
	//end the current frame (internally swaps the front and back buffers)
	m_pWindow->display();
}
void Application::Release(void)
{
	SafeDelete(m_pMesh);

	//release GUI
	ShutdownGUI();
}
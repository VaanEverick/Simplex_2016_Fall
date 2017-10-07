#include "AppClass.h"
void Application::InitVariables(void)
{
	////Change this to your name and email
	m_sProgrammer = "Christopher Banks - cdb6474@g.rit.edu";

	////Alberto needed this at this position for software recording.
	//m_pWindow->setPosition(sf::Vector2i(710, 0));
	
	//Set the position and target of the camera
	//(I'm at [0,0,10], looking at [0,0,0] and up is the positive Y axis)
	m_pCameraMngr->SetPositionTargetAndUp(AXIS_Z * 20.0f, ZERO_V3, AXIS_Y);

	//if the light position is zero move it
	if (m_pLightMngr->GetPosition(1) == ZERO_V3)
		m_pLightMngr->SetPosition(vector3(0.0f, 0.0f, 3.0f));

	//if the background is cornflowerblue change it to black (its easier to see)
	if (vector3(m_v4ClearColor) == C_BLUE_CORNFLOWER)
	{
		m_v4ClearColor = vector4(ZERO_V3, 1.0f);
	}
	
	//if there are no segments create 7
	if(m_uOrbits < 1)
		m_uOrbits = 7;

	float fSize = 1.0f; //initial size of orbits

	//creating a color using the spectrum 
	uint uColor = 650; //650 is Red
	
	//prevent division by 0
	float decrements = 250.0f / (m_uOrbits > 1? static_cast<float>(m_uOrbits - 1) : 1.0f); //decrement until you get to 400 (which is violet)
	/*
		This part will create the orbits, it start at 3 because that is the minimum subdivisions a torus can have
	*/
	uint uSides = 3; //start with the minimal 3 sides
	for (uint i = uSides; i < m_uOrbits + uSides; i++)
	{
		vector3 v3Color = WaveLengthToRGB(uColor); //calculate color based on wavelength
		m_shapeList.push_back(m_pMeshMngr->GenerateTorus(fSize, fSize - 0.1f, 3, i, v3Color)); //generate a custom torus and add it to the meshmanager //HERE : vector storing each orbit. This is our loop
		fSize += 0.5f; //increment the size for the next orbit
		uColor -= static_cast<uint>(decrements); //decrease the wavelength
	}

	for (int i = 0; i < m_uOrbits; i++)
	{
		spheres.push_back(mySphere());
		spheres[i].NumStops = i + 3;
		float fSin = 0;
		float fCos = 1;
		float SubSpace = (360.f / (float)spheres[i].NumStops) * (PI / 180.f); //divisions for each torus
		spheres[i].fSize += (.5f * i);

		for (int j = 0; j < spheres[i].NumStops; j++)
		{
			fSin = std::sin(SubSpace * j);
			fCos = std::cos(SubSpace * j);
			spheres[i].tracePoints.push_back(vector3(fSize * fSin, fSize * fCos, 0));
		}

	}
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

	matrix4 m4View = m_pCameraMngr->GetViewMatrix(); //view Matrix
	matrix4 m4Projection = m_pCameraMngr->GetProjectionMatrix(); //Projection Matrix
	matrix4 m4Offset = IDENTITY_M4; //offset of the orbits, starts as the global coordinate system
	/*
		The following offset will orient the orbits as in the demo, start without it to make your life easier.
	*/
	//m4Offset = glm::rotate(IDENTITY_M4, 90.0f, AXIS_Z);

	// draw a shapes
	for (uint i = 0; i < m_uOrbits; ++i)
	{
		m_pMeshMngr->AddMeshToRenderList(m_shapeList[i], glm::rotate(m4Offset, 90.0f, AXIS_X));
		
		//calculate the current position of the spheres

		spheres[i].v3Start = spheres[i].tracePoints[spheres[i].route];
		spheres[i].v3End = spheres[i].tracePoints[(spheres[i].route + 1) % spheres[i].tracePoints.size()];

		float fTimeBetweenStops = 1.125f;
		static DWORD startTime = GetTickCount();
		DWORD currentTime = GetTickCount();
		float fCurrentTime = (currentTime - startTime) / 1000.f;

		float fPercentage = MapValue(fCurrentTime, 0.f, fTimeBetweenStops, 0.0f, 1.0f);

		spheres[i].v3CurrentPos = glm::lerp(spheres[i].v3Start, spheres[i].v3End, fPercentage);
	
		matrix4 m4Model = glm::translate(m4Offset, spheres[i].v3CurrentPos);

		if (fCurrentTime >= fTimeBetweenStops)
		{
			spheres[i].route++;
			spheres[i].route %= spheres[i].tracePoints.size();
			startTime = GetTickCount();
		}

		//draw spheres
		m_pMeshMngr->AddSphereToRenderList(m4Model * glm::scale(vector3(0.1)), C_WHITE);
	}

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
	//release GUI
	ShutdownGUI();
}


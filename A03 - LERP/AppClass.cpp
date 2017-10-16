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
		m_shapeList.push_back(m_pMeshMngr->GenerateTorus(fSize, fSize - 0.1f, 3, i, v3Color)); //generate a custom torus and add it to the meshmanager 
		fSize += 0.5f; //increment the size for the next orbit
		uColor -= static_cast<uint>(decrements); //decrease the wavelength
	}

	//Don't see anyway to get the torii, the method it points to is never called (very mysterious)
	//Recreating orbits dynamically. Not really following the routes, but it does follow the routes. 
	for (int i = 0; i < m_uOrbits; i++)
	{
		spheres.push_back(mySphere());
		spheres[i].NumStops = i + 3; //Minimum route size is three, a triangle
		float fSin = 0;
		float fCos = 1;
		float SubSpace = (360.f / (float)spheres[i].NumStops) * (PI / 180.f); //divisions for each torus
		spheres[i].fSize += (.5f * i); //make each orbit larger

		for (int j = 0; j < spheres[i].NumStops; j++)
		{
			fSin = std::sin((SubSpace * j) + ((PI / 180.f) * 3));
			fCos = std::cos((SubSpace * j) + ((PI / 180.f) * 3));
			spheres[i].tracePoints.push_back(vector3(spheres[i].fSize * fSin, spheres[i].fSize * fCos, 0));
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
	m4Offset = glm::rotate(IDENTITY_M4, 90.0f, AXIS_Z);

	// draw a shapes
	for (uint i = 0; i < m_uOrbits; ++i)
	{
		m_pMeshMngr->AddMeshToRenderList(m_shapeList[i], glm::rotate(m4Offset, 90.0f, AXIS_X));
		
		//calculate the current position of the spheres

		//Get the start and end nodes of each route segment route the route number, loop back to 0 at end
		spheres[i].v3Start = spheres[i].tracePoints[spheres[i].route];
		spheres[i].v3End = spheres[i].tracePoints[(spheres[i].route + 1) % spheres[i].tracePoints.size()];

		//get time values independent of this program, to maintain a constant increment across any machine
		float fTimeBetweenStops = .5f;
		static DWORD startTime = GetTickCount();
		DWORD currentTime = GetTickCount();
		float fCurrentTime = (currentTime - startTime) / 1000.f;

		//update how far along each route each sphere is
		float fPercentage = MapValue(fCurrentTime, 0.f, fTimeBetweenStops, 0.0f, 1.0f);

		//give each sphere an updated position along each node-node route
		spheres[i].v3CurrentPos = glm::lerp(spheres[i].v3Start, spheres[i].v3End, fPercentage);
		
		//translate the sphere model along its route
		matrix4 m4Model = glm::translate(IDENTITY_M4, spheres[i].v3CurrentPos);

		//move along to the next segment once the timer passes the limit 
		//overall time (reset at each stop) is comparative to the percentage along each segment
		if (fCurrentTime >= fTimeBetweenStops)
		{
			for (int k = 0; k < spheres.size(); k++)
			{
				spheres[k].route++;
				spheres[k].route %= spheres[k].tracePoints.size();
			}
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


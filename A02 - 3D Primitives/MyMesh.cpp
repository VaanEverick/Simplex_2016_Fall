#include "MyMesh.h"
void MyMesh::Init(void)
{
	m_bBinded = false;
	m_uVertexCount = 0;

	m_VAO = 0;
	m_VBO = 0;

	m_pShaderMngr = ShaderManager::GetInstance();
}
void MyMesh::Release(void)
{
	m_pShaderMngr = nullptr;

	if (m_VBO > 0)
		glDeleteBuffers(1, &m_VBO);

	if (m_VAO > 0)
		glDeleteVertexArrays(1, &m_VAO);

	m_lVertex.clear();
	m_lVertexPos.clear();
	m_lVertexCol.clear();
}
MyMesh::MyMesh()
{
	Init();
}
MyMesh::~MyMesh() { Release(); }
MyMesh::MyMesh(MyMesh& other)
{
	m_bBinded = other.m_bBinded;

	m_pShaderMngr = other.m_pShaderMngr;

	m_uVertexCount = other.m_uVertexCount;

	m_VAO = other.m_VAO;
	m_VBO = other.m_VBO;
}
MyMesh& MyMesh::operator=(MyMesh& other)
{
	if (this != &other)
	{
		Release();
		Init();
		MyMesh temp(other);
		Swap(temp);
	}
	return *this;
}
void MyMesh::Swap(MyMesh& other)
{
	std::swap(m_bBinded, other.m_bBinded);
	std::swap(m_uVertexCount, other.m_uVertexCount);

	std::swap(m_VAO, other.m_VAO);
	std::swap(m_VBO, other.m_VBO);

	std::swap(m_lVertex, other.m_lVertex);
	std::swap(m_lVertexPos, other.m_lVertexPos);
	std::swap(m_lVertexCol, other.m_lVertexCol);

	std::swap(m_pShaderMngr, other.m_pShaderMngr);
}
void MyMesh::CompleteMesh(vector3 a_v3Color)
{
	uint uColorCount = m_lVertexCol.size();
	for (uint i = uColorCount; i < m_uVertexCount; ++i)
	{
		m_lVertexCol.push_back(a_v3Color);
	}
}
void MyMesh::AddVertexPosition(vector3 a_v3Input)
{
	m_lVertexPos.push_back(a_v3Input);
	m_uVertexCount = m_lVertexPos.size();
}
void MyMesh::AddVertexColor(vector3 a_v3Input)
{
	m_lVertexCol.push_back(a_v3Input);
}
void MyMesh::CompileOpenGL3X(void)
{
	if (m_bBinded)
		return;

	if (m_uVertexCount == 0)
		return;

	CompleteMesh();

	for (uint i = 0; i < m_uVertexCount; i++)
	{
		//Position
		m_lVertex.push_back(m_lVertexPos[i]);
		//Color
		m_lVertex.push_back(m_lVertexCol[i]);
	}
	glGenVertexArrays(1, &m_VAO);//Generate vertex array object
	glGenBuffers(1, &m_VBO);//Generate Vertex Buffered Object

	glBindVertexArray(m_VAO);//Bind the VAO
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);//Bind the VBO
	glBufferData(GL_ARRAY_BUFFER, m_uVertexCount * 2 * sizeof(vector3), &m_lVertex[0], GL_STATIC_DRAW);//Generate space for the VBO

	// Position attribute
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 2 * sizeof(vector3), (GLvoid*)0);

	// Color attribute
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 2 * sizeof(vector3), (GLvoid*)(1 * sizeof(vector3)));

	m_bBinded = true;

	glBindVertexArray(0); // Unbind VAO
}
void MyMesh::Render(matrix4 a_mProjection, matrix4 a_mView, matrix4 a_mModel)
{
	// Use the buffer and shader
	GLuint nShader = m_pShaderMngr->GetShaderID("Basic");
	glUseProgram(nShader); 

	//Bind the VAO of this object
	glBindVertexArray(m_VAO);

	// Get the GPU variables by their name and hook them to CPU variables
	GLuint MVP = glGetUniformLocation(nShader, "MVP");
	GLuint wire = glGetUniformLocation(nShader, "wire");

	//Final Projection of the Camera
	matrix4 m4MVP = a_mProjection * a_mView * a_mModel;
	glUniformMatrix4fv(MVP, 1, GL_FALSE, glm::value_ptr(m4MVP));
	
	//Solid
	glUniform3f(wire, -1.0f, -1.0f, -1.0f);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glDrawArrays(GL_TRIANGLES, 0, m_uVertexCount);  

	//Wire
	glUniform3f(wire, 1.0f, 0.0f, 1.0f);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glEnable(GL_POLYGON_OFFSET_LINE);
	glPolygonOffset(-1.f, -1.f);
	glDrawArrays(GL_TRIANGLES, 0, m_uVertexCount);
	glDisable(GL_POLYGON_OFFSET_LINE);

	glBindVertexArray(0);// Unbind VAO so it does not get in the way of other objects
}
void MyMesh::AddTri(vector3 a_vBottomLeft, vector3 a_vBottomRight, vector3 a_vTopLeft)
{
	//C
	//| \
	//A--B
	//This will make the triangle A->B->C 
	AddVertexPosition(a_vBottomLeft);
	AddVertexPosition(a_vBottomRight);
	AddVertexPosition(a_vTopLeft);
}
void MyMesh::AddQuad(vector3 a_vBottomLeft, vector3 a_vBottomRight, vector3 a_vTopLeft, vector3 a_vTopRight)
{
	//C--D
	//|  |
	//A--B
	//This will make the triangle A->B->C and then the triangle C->B->D
	AddVertexPosition(a_vBottomLeft);
	AddVertexPosition(a_vBottomRight);
	AddVertexPosition(a_vTopLeft);

	AddVertexPosition(a_vTopLeft);
	AddVertexPosition(a_vBottomRight);
	AddVertexPosition(a_vTopRight);
}
void MyMesh::GenerateCube(float a_fSize, vector3 a_v3Color)
{
	if (a_fSize < 0.01f)
		a_fSize = 0.01f;

	Release();
	Init();

	float fValue = a_fSize * 0.5f;
	//3--2
	//|  |
	//0--1

	vector3 point0(-fValue,-fValue, fValue); //0
	vector3 point1( fValue,-fValue, fValue); //1
	vector3 point2( fValue, fValue, fValue); //2
	vector3 point3(-fValue, fValue, fValue); //3

	vector3 point4(-fValue,-fValue,-fValue); //4
	vector3 point5( fValue,-fValue,-fValue); //5
	vector3 point6( fValue, fValue,-fValue); //6
	vector3 point7(-fValue, fValue,-fValue); //7

	//F
	AddQuad(point0, point1, point3, point2);

	//B
	AddQuad(point5, point4, point6, point7);

	//L
	AddQuad(point4, point0, point7, point3);

	//R
	AddQuad(point1, point5, point2, point6);

	//U
	AddQuad(point3, point2, point7, point6);

	//D
	AddQuad(point4, point5, point0, point1);

	// Adding information about color
	CompleteMesh(a_v3Color);
	CompileOpenGL3X();
}
void MyMesh::GenerateCuboid(vector3 a_v3Dimensions, vector3 a_v3Color)
{
	Release();
	Init();

	vector3 v3Value = a_v3Dimensions * 0.5f;
	//3--2
	//|  |
	//0--1
	vector3 point0(-v3Value.x, -v3Value.y, v3Value.z); //0
	vector3 point1(v3Value.x, -v3Value.y, v3Value.z); //1
	vector3 point2(v3Value.x, v3Value.y, v3Value.z); //2
	vector3 point3(-v3Value.x, v3Value.y, v3Value.z); //3

	vector3 point4(-v3Value.x, -v3Value.y, -v3Value.z); //4
	vector3 point5(v3Value.x, -v3Value.y, -v3Value.z); //5
	vector3 point6(v3Value.x, v3Value.y, -v3Value.z); //6
	vector3 point7(-v3Value.x, v3Value.y, -v3Value.z); //7

	//F
	AddQuad(point0, point1, point3, point2);

	//B
	AddQuad(point5, point4, point6, point7);

	//L
	AddQuad(point4, point0, point7, point3);

	//R
	AddQuad(point1, point5, point2, point6);

	//U
	AddQuad(point3, point2, point7, point6);

	//D
	AddQuad(point4, point5, point0, point1);

	// Adding information about color
	CompleteMesh(a_v3Color);
	CompileOpenGL3X();
}
void MyMesh::GenerateCone(float a_fRadius, float a_fHeight, int a_nSubdivisions, vector3 a_v3Color)
{
	if (a_fRadius < 0.01f)
		a_fRadius = 0.01f;

	if (a_fHeight < 0.01f)
		a_fHeight = 0.01f;

	if (a_nSubdivisions < 3)
		a_nSubdivisions = 3;
	if (a_nSubdivisions > 360)
		a_nSubdivisions = 360;

	Release();
	Init();


// Determine |height|
float m_fAbHeight = a_fHeight / 2.0f;

// determine top point based on 1/2 height
vector3 m_vTop(0, m_fAbHeight, 0);

// determine center of base based on -1/2 height
vector3 m_vBot(0, -m_fAbHeight, 0);

// determine each subdivision point based on radius * sin(360 / sub) and radius * cos(360 / sub) and -1/2 height (x, z, y)
std::vector<vector3> m_vSubbies;
float m_fTrueSubs = a_nSubdivisions - 1;
float m_fSinValue = 0.f;
float m_fCosValue = 1.f;

for (int i = 0; i < a_nSubdivisions; i++) 
{
	// 360 / nSubs == arc; sin(arc * i);
	m_fSinValue = std::sin(((360.f / (float)a_nSubdivisions) * i) * (PI / 180.f));
	m_fCosValue = std::cos(((360.f / (float)a_nSubdivisions) * i) * (PI / 180.f));

	//Push each point into the data array
	m_vSubbies.push_back(vector3(m_fSinValue * a_fRadius, -m_fAbHeight, m_fCosValue * a_fRadius));
	
}

// Link each pair of sub points together with the base point (AddTri(sub i, sub i++, base); i++;)
for (int i = 0; i < m_fTrueSubs; i++)
{
	//AddTri(m_vSubbies[i], m_vSubbies[i++], m_vBot);
	AddTri(m_vSubbies[i + 1], m_vSubbies[i], m_vBot);
	AddTri(m_vSubbies[i], m_vSubbies[i + 1], m_vTop);
}

//Link first point to last point (Top)
AddTri(m_vSubbies[0], m_vSubbies[m_fTrueSubs], m_vBot);

//Link first point to last point (Top)
AddTri(m_vSubbies[m_fTrueSubs], m_vSubbies[0], m_vTop);

// -------------------------------

// Adding information about color
CompleteMesh(a_v3Color);
CompileOpenGL3X();
}
void MyMesh::GenerateCylinder(float a_fRadius, float a_fHeight, int a_nSubdivisions, vector3 a_v3Color)
{
	if (a_fRadius < 0.01f)
		a_fRadius = 0.01f;

	if (a_fHeight < 0.01f)
		a_fHeight = 0.01f;

	if (a_nSubdivisions < 3)
		a_nSubdivisions = 3;
	if (a_nSubdivisions > 360)
		a_nSubdivisions = 360;

	Release();
	Init();

	// Determine |height|
	float m_fAbHeight = a_fHeight / 2.0f;

	// determine top point based on 1/2 height
	vector3 m_vTop(0, m_fAbHeight, 0);

	// determine center of base based on -1/2 height
	vector3 m_vBot(0, -m_fAbHeight, 0);

	// determine each subdivision point based on radius * sin(360 / sub) and radius * cos(360 / sub) and -1/2 height (x, z, y)
	std::vector<vector3> m_vTopShelf;
	std::vector<vector3> m_vBottomShelf;
	float m_fTrueSubs = a_nSubdivisions - 1;
	float m_fSinValue = 0.f;
	float m_fCosValue = 1.f;

	for (int i = 0; i < a_nSubdivisions; i++) 
	{
		// 360 / nSubs == arc; sin(arc * i);
		m_fSinValue = std::sin(((360.f / (float)a_nSubdivisions) * i) * (PI / 180.f));
		m_fCosValue = std::cos(((360 / (float)a_nSubdivisions) * i) * (PI / 180.f));

		m_vTopShelf.push_back(vector3(m_fSinValue * a_fRadius, m_fAbHeight, m_fCosValue * a_fRadius));
		m_vBottomShelf.push_back(m_vTopShelf[i] - vector3(0, a_fHeight, 0));
	}


	//TODO :  link each pair of sub points together with the top point (AddTri(sub i, sub i++, top); i++;)
	for (int i = 0; i < m_vTopShelf.size() - 1; i++)
	{
		AddTri(m_vTopShelf[i], m_vTopShelf[i + 1], m_vTop);
		AddTri(m_vBottomShelf[i + 1], m_vBottomShelf[i], m_vBot);
		AddQuad(m_vBottomShelf[i], m_vBottomShelf[i + 1], m_vTopShelf[i], m_vTopShelf[i + 1]);
	}

	//Link first points to last points
	AddTri(m_vTopShelf[m_fTrueSubs], m_vTopShelf[0], m_vTop);
	AddTri(m_vBottomShelf[0], m_vBottomShelf[m_fTrueSubs], m_vBot);
	AddQuad( m_vBottomShelf[m_fTrueSubs], m_vBottomShelf[0],  m_vTopShelf[m_fTrueSubs], m_vTopShelf[0] );

	// -------------------------------

	// Adding information about color
	CompleteMesh(a_v3Color);
	CompileOpenGL3X();
}
void MyMesh::GenerateTube(float a_fOuterRadius, float a_fInnerRadius, float a_fHeight, int a_nSubdivisions, vector3 a_v3Color)
{
	if (a_fOuterRadius < 0.01f) { a_fOuterRadius = 0.01f; }

	if (a_fInnerRadius < 0.005f) { a_fInnerRadius = 0.005f; }

	if (a_fInnerRadius > a_fOuterRadius) { std::swap(a_fInnerRadius, a_fOuterRadius); }

	if (a_fHeight < 0.01f) { a_fHeight = 0.01f; }

	if (a_nSubdivisions < 3) { a_nSubdivisions = 3; }

	if (a_nSubdivisions > 360) { a_nSubdivisions = 360; }

Release();
Init();

// Determine |height|
float m_fAbHeight = a_fHeight / 2.0f;

// determine each subdivision point based on radius * sin(360 / sub) and radius * cos(360 / sub) and -1/2 height (x, z, y)
std::vector<vector3> m_vInTop;
std::vector<vector3> m_vOutTop;
std::vector<vector3> m_vInBottom;
std::vector<vector3> m_vOutBottom;
float m_fTrueSubs = a_nSubdivisions - 1;
float m_fSinValue = 0.f;
float m_fCosValue = 1.f;

for (int i = 0; i < a_nSubdivisions; i++) 
{
	// 360 / nSubs == arc; sin(arc * i);
	m_fSinValue = std::sin(((360.f / (float)a_nSubdivisions) * i) * (PI / 180.f));
	m_fCosValue = std::cos(((360 / (float)a_nSubdivisions) * i) * (PI / 180.f));

	m_vInTop.push_back(vector3(m_fSinValue * a_fInnerRadius, m_fAbHeight, m_fCosValue * a_fInnerRadius));
	m_vOutTop.push_back(vector3(m_fSinValue * a_fOuterRadius, m_fAbHeight, m_fCosValue * a_fOuterRadius));
	m_vInBottom.push_back(vector3(m_fSinValue * a_fInnerRadius, -m_fAbHeight, m_fCosValue * a_fInnerRadius));
	m_vOutBottom.push_back(vector3(m_fSinValue * a_fOuterRadius, -m_fAbHeight, m_fCosValue * a_fOuterRadius));
}

//TODO :  Link Top Quads Together
for (int i = 0; i < m_vInTop.size() - 1; i++)
{
	AddQuad(m_vInTop[i + 1], m_vInTop[i], m_vOutTop[i + 1], m_vOutTop[i]); //Upper Loop
	AddQuad(m_vInBottom[i], m_vInBottom[i + 1], m_vOutBottom[i], m_vOutBottom[i + 1]); //Lower Loop
	AddQuad(m_vOutTop[i + 1], m_vOutTop[i], m_vOutBottom[i + 1], m_vOutBottom[i]);//Outside wall
	AddQuad(m_vInTop[i], m_vInTop[i + 1], m_vInBottom[i], m_vInBottom[i + 1]); //Inside Wall
}

//Link first points to last points
AddQuad(m_vInTop[0], m_vInTop[m_fTrueSubs], m_vOutTop[0], m_vOutTop[m_fTrueSubs]); //Upper Loop
AddQuad(m_vInBottom[m_fTrueSubs], m_vInBottom[0], m_vOutBottom[m_fTrueSubs], m_vOutBottom[0]); //Lower Loop
AddQuad(m_vOutTop[0], m_vOutTop[m_fTrueSubs], m_vOutBottom[0], m_vOutBottom[m_fTrueSubs]);//Outside wall
AddQuad(m_vInTop[m_fTrueSubs], m_vInTop[0], m_vInBottom[m_fTrueSubs], m_vInBottom[0]); //Inside Wall

// Adding information about color
CompleteMesh(a_v3Color);
CompileOpenGL3X();
}
void MyMesh::GenerateTorus(float a_fOuterRadius, float a_fInnerRadius, int a_nSubdivisionsA, int a_nSubdivisionsB, vector3 a_v3Color)
{
	if (a_fOuterRadius < 0.01f) { a_fOuterRadius = 0.01f; }

	if (a_fInnerRadius < 0.005f) { a_fInnerRadius = 0.005f; }

	if (a_fInnerRadius > a_fOuterRadius) { std::swap(a_fInnerRadius, a_fOuterRadius); }

	if (a_nSubdivisionsA < 3) { a_nSubdivisionsA = 3; }

	if (a_nSubdivisionsA > 360) { a_nSubdivisionsA = 360; }

	if (a_nSubdivisionsB < 3) { a_nSubdivisionsB = 3; }

	if (a_nSubdivisionsB > 360) { a_nSubdivisionsB = 360; }

	Release();
	Init();

	/*
		Torus is basically just a circle folded in on itself in the middle, so the equation is pretty close to that of a circle
		c - 1/2 of the sum of the two radii
		a - 1/2 of the difference of two radii
		sepA - arc length for the outer radii subdivisions
		sepB - arc length for the inner radii subdivisions
		toRad - because c++ is too good for degrees
		Easy Peezy Lemon Squeezy
		(They said it would be difficult difficult lemon difficult)
	*/
	float c = .5f * (a_fOuterRadius + a_fInnerRadius);
	float a = .5f * (a_fOuterRadius - a_fInnerRadius);
	float sepA = 360.f / (float)a_nSubdivisionsA;
	float sepB = 360.f / (float)a_nSubdivisionsB;
	float toRad = PI / 180.f;

	std::vector<std::vector<vector3>> Rings; //An array holding arrays holding each Ring (the around points at each subdivision)
	std::vector<vector3> Points; //an array holding all the points that make up a Ring

	for (int i = 0; i < a_nSubdivisionsA; i++)
	{
		Points.clear();
		for (int j = 0; j < a_nSubdivisionsB; j++)
		{
			float x = (c + a * cos(sepA * i * toRad)) * cos(sepB * j * toRad);
			float y = (c + a * cos(sepA * i * toRad)) * sin(sepB * j * toRad);
			float z = a * sin(sepA * i * toRad);
			Points.push_back(vector3(x, y, z));
		}
		Rings.push_back(Points);
	}

	for (int i = 0; i < a_nSubdivisionsA -1; i++)
	{
		for (int j = 0; j < a_nSubdivisionsB - 1; j++)
		{
			AddQuad(Rings[i][j], Rings[i][j+1], Rings[i+1][j], Rings[i+1][j+1]);
			AddQuad(Rings[a_nSubdivisionsA - 1][j], Rings[a_nSubdivisionsA - 1][j + 1], Rings[0][j], Rings[0][j + 1]);
		}
		AddQuad(Rings[i][a_nSubdivisionsB - 1], Rings[i][0], Rings[i + 1][a_nSubdivisionsB - 1], Rings[i + 1][0]);
	}
	
	AddQuad(Rings[0][0], Rings[0][a_nSubdivisionsB - 1], Rings[a_nSubdivisionsA -1][0], Rings[a_nSubdivisionsA -1][a_nSubdivisionsB -1]);

	// -------------------------------

	// Adding information about color
	CompleteMesh(a_v3Color);
	CompileOpenGL3X();
}
void MyMesh::GenerateSphere(float a_fRadius, int a_nSubdivisions, vector3 a_v3Color)
{
	if (a_fRadius < 0.01f) { a_fRadius = 0.01f; }

	//Sets minimum and maximum of subdivisions
	if (a_nSubdivisions < 1)
	{
		GenerateCube(a_fRadius * 2.0f, a_v3Color);
		return;
	}
	if (a_nSubdivisions < 6) { a_nSubdivisions = 6; }

	Release();
	Init();

	/*
		Okay. Spheres. 
		x = r * cos (theta) * sin(phi)
		y = r * sin (theta) * sin(phi)
		z = r * cos (phi);

		theta 0 - 2PI. Longitude.
		phi 0 - PI latitude; 

		sphere has equal subdivisions longitutde and latitude.

		*/

	//vector of vectors of points forming each loop of the sphere
	std::vector<std::vector<vector3>> Rings;
	//vector of points in each loop
	std::vector<vector3> Points;

	//Determine ratio in which theta increases
	float theta = 360.f / (float)a_nSubdivisions;
	theta = theta * (PI / 180);

	//Phi spans half the arc that theta does, so it either has a ratio of 1/2 theta. 
	float phi = theta / 2.f;

	//Generate Points
	for (int i = 0; i < a_nSubdivisions + 2; i++)
	{
		Points.clear();
		for (int j = 0; j < a_nSubdivisions + 2; j++)
		{
			float x = a_fRadius * cos(theta * i) * sin(phi * j);
			float y = a_fRadius * sin(theta * i) * sin(phi * j);
			float z = a_fRadius * cos(phi * j);
			Points.push_back(vector3(x, y, z));
		}
		Rings.push_back(Points);
	}

	//Link points together
	for (int i = 0; i < a_nSubdivisions; i++)
	{
		for (int j = 0; j < a_nSubdivisions; j++)
		{
			AddQuad(Rings[i][j], Rings[i][j + 1], Rings[i + 1][j], Rings[i + 1][j + 1]);
			AddQuad(Rings[a_nSubdivisions - 1][j], Rings[a_nSubdivisions - 1][j + 1], Rings[0][j], Rings[0][j + 1]);
		}
		AddQuad(Rings[i][a_nSubdivisions - 1], Rings[i][0], Rings[i + 1][a_nSubdivisions - 1], Rings[i + 1][0]);
	}

	//Fill the end quad
	AddQuad(Rings[0][0], Rings[0][a_nSubdivisions - 1], Rings[a_nSubdivisions - 1][0], Rings[a_nSubdivisions - 1][a_nSubdivisions - 1]);

	// -------------------------------

	// Adding information about color
	CompleteMesh(a_v3Color);
	CompileOpenGL3X();
}
#include "types.h"
#include "FileUtil.h"
#include "CImage.h"
#include "3d/camera.h"
#include "3d/model.h"
#include "3d/font.h"
#include "3d/physics.h"

#if defined(WIN32)
#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glew32s.lib")
#endif

// Global variables
bool	gKeyboardStates[GLFW_KEY_LAST+1];	// Keys states
bool	gMouseRightPressed;					// Mouse button state
bool	gMouseMiddlePressed;				// Mouse button state
bool	gMouseLeftPressed;					// Mouse button state
float	gMousePositionX;					// Mouse position (relative to the start position)
float	gMousePositionY;					// Mouse position (relative to the start position)
float	gMouseRelativeX;					// Mouse relative movement
float	gMouseRelativeY;					// Mouse relative movement

Camera		gCamera;
Physics		gPhysics;
Model		gModelSky;
glm::mat4	gMat4Sky;


Model			gModelCity;
glm::mat4		gMat4City;
btRigidBody*	gCollisionCity = NULL;

Model			gModelHero;
glm::mat4		gMat4Hero;
btRigidBody*	gCollisionHero = NULL;

Model			gModelKnot;
glm::mat4		gMat4Knot;
float			knotAngleX = 0;
float			knotAngleY = 0;
float			knotAngleZ = 0;



Font	gFont;

Shader	gShaderTest;
GLuint	VAO_Test;
GLuint	VBO_Test;
GLuint	TEX_Test;
GLfloat vertices_Test[] =
{
	// Define a rectangle (2 triangles)
	// Positions + Texture Coords + Normals + couleurs RGBA
	-10.0f,-7.0f, 0.0f,   0.0f,0.0f,   0.0f, 0.0f, 1.0f,   1.0f, 0.0f, 0.0f, 1.0f,
	-10.0f, 7.0f, 0.0f,   0.0f,1.0f,   0.0f, 0.0f, 1.0f,   0.0f, 1.0f, 0.0f, 1.0f,
	 10.0f, 7.0f, 0.0f,   1.0f,1.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f, 1.0f, 1.0f,

	-10.0f,-7.0f, 0.0f,   0.0f,0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 1.0f, 1.0f, 1.0f, 
	 10.0f, 7.0f, 0.0f,   1.0f,1.0f,   0.0f, 0.0f, 1.0f,   1.0f, 1.0f, 0.0f, 1.0f, 
	 10.0f,-7.0f, 0.0f,   1.0f,0.0f,   0.0f, 0.0f, 1.0f,   1.0f, 0.0f, 1.0f, 1.0f
};


void callback_error(int error, const char* description)
{
	fprintf(stderr, "Error %d : %s\n", error, description);
}

void callback_keyboard(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, GLFW_TRUE);
		return;
	}

	if (action == GLFW_PRESS)
	{
		gKeyboardStates[key] = true;
	}
	else if (action == GLFW_RELEASE)
	{
		gKeyboardStates[key] = false;
	}
}

void callback_mouse(GLFWwindow * window, double xpos, double ypos)
{
	static bool  bFirstTime     = true;
	static float mouseStartPosX = 0.0;
	static float mouseStartPosY = 0.0;
	static float mouseLastPosX  = 0.0;
	static float mouseLastPosY  = 0.0;

	if (bFirstTime)
	{
		mouseStartPosX = 0.0f;
		mouseStartPosY = 0.0f;
		mouseLastPosX  =  (float)xpos;
		mouseLastPosY  = -(float)ypos;
		bFirstTime = false;
	}

	gMousePositionX =  ((float)xpos - mouseStartPosX);
	gMousePositionY = -((float)ypos - mouseStartPosY);
	gMouseRelativeX = gMousePositionX - mouseLastPosX;
	gMouseRelativeY = gMousePositionY - mouseLastPosY;
	mouseLastPosX   = gMousePositionX;
	mouseLastPosY   = gMousePositionY;

}

void callback_mouseButton(GLFWwindow * window, int button, int action, int mods)
{
	bool bIsButtonPressed = (action == GLFW_PRESS);
	if (button == GLFW_MOUSE_BUTTON_RIGHT)
	{
		gMouseRightPressed = bIsButtonPressed;
	}
	else if (button == GLFW_MOUSE_BUTTON_MIDDLE)
	{
		gMouseMiddlePressed = bIsButtonPressed;
	}
	else if (button == GLFW_MOUSE_BUTTON_LEFT)
	{
		gMouseLeftPressed = bIsButtonPressed;
	}
}

void InitInputs(GLFWwindow* pWindow)
{
	// Connect callback for input (keyboard, mouse), hide the mouse cursor.
	glfwSetInputMode			(pWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetKeyCallback			(pWindow, callback_keyboard);
	glfwSetCursorPosCallback	(pWindow, callback_mouse);
	glfwSetMouseButtonCallback	(pWindow, callback_mouseButton);
	glfwSetScrollCallback		(pWindow, callback_mouseWheel);

	for (size_t i=0; i<sizeofarray(gKeyboardStates); i++)
	{
		gKeyboardStates[i] = false;
	}
	gMouseRightPressed	= false;
	gMouseMiddlePressed	= false;
	gMouseLeftPressed	= false;
	gMouseRelativeX		= 0.0f;
	gMouseRelativeY		= 0.0f;
}

void InitializeOpenGLView(GLFWwindow* pWindow)
{
	glGenVertexArrays(1, &VAO_Test);
	glGenBuffers     (1, &VBO_Test);
	glBindVertexArray(VAO_Test);
	glBindBuffer(GL_ARRAY_BUFFER, VBO_Test);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices_Test), vertices_Test, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 12*sizeof(float), (GLvoid*)0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 12*sizeof(float), (GLvoid*)(5*sizeof(float)));
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 12*sizeof(float), (GLvoid*)(3*sizeof(float)));
	glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, 12*sizeof(float), (GLvoid*)(8*sizeof(float)));
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glEnableVertexAttribArray(3);
	glBindVertexArray(0);	// unbinds the VAO

	glGenTextures(1, &TEX_Test);
	glBindTexture(GL_TEXTURE_2D, TEX_Test);
	CImage texture;
	if (texture.Load("images/colors.png") == false)
	{
		ConsoleWriteErr("Failed to load texture");
	}
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texture.lenx, texture.leny, 0, GL_RGBA, GL_UNSIGNED_BYTE, texture.data);
	glGenerateMipmap(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);

	if (gShaderTest.Load(getCurrentDirectory()+"shaders/ambient_lum.vert", getCurrentDirectory()+"shaders/ambient_lum.frag") == false)
	{
		ConsoleWriteErr("Failed to load shader");
	}

}

void UpdateGameLogic(float deltaTimeInSecond)
{
	if (gMouseLeftPressed)						gCamera.Move(FORWARD,	10*deltaTimeInSecond);
	if (gMouseRightPressed)						gCamera.Move(BACKWARD,	10*deltaTimeInSecond);
	if (gKeyboardStates[GLFW_KEY_LEFT_SHIFT] || gKeyboardStates[GLFW_KEY_RIGHT_SHIFT])
	{
		if (gKeyboardStates[GLFW_KEY_UP			])	gPhysics.SetVelocity(gCollisionHero, +gCamera.m_forwardAxis*100.f);
		if (gKeyboardStates[GLFW_KEY_DOWN		])	gPhysics.SetVelocity(gCollisionHero, -gCamera.m_forwardAxis*100.f);
		if (gKeyboardStates[GLFW_KEY_LEFT		])	gPhysics.SetVelocity(gCollisionHero, -gCamera.m_rightAxis  *100.f);
		if (gKeyboardStates[GLFW_KEY_RIGHT		])	gPhysics.SetVelocity(gCollisionHero, +gCamera.m_rightAxis  *100.f);
		if (gKeyboardStates[GLFW_KEY_PAGE_UP	])	gPhysics.SetVelocity(gCollisionHero, +gCamera.m_upAxis     *100.f);
		if (gKeyboardStates[GLFW_KEY_PAGE_DOWN	])	gPhysics.SetVelocity(gCollisionHero, -gCamera.m_upAxis     *100.f);
	}
	else
	{
		if (gKeyboardStates[GLFW_KEY_UP			])	gCamera.Move(FORWARD,	10*deltaTimeInSecond);
		if (gKeyboardStates[GLFW_KEY_DOWN		])	gCamera.Move(BACKWARD,	10*deltaTimeInSecond);
		if (gKeyboardStates[GLFW_KEY_LEFT		])	gCamera.Move(LEFT,		10*deltaTimeInSecond);
		if (gKeyboardStates[GLFW_KEY_RIGHT		])	gCamera.Move(RIGHT,		10*deltaTimeInSecond);
		if (gKeyboardStates[GLFW_KEY_PAGE_UP	])	gCamera.Move(UP,		10*deltaTimeInSecond);
		if (gKeyboardStates[GLFW_KEY_PAGE_DOWN	])	gCamera.Move(DOWN,		10*deltaTimeInSecond);
	}

	if (gMouseRelativeX != 0 || gMouseRelativeY != 0)
	{
		gCamera.Rotate(gMouseRelativeY,-gMouseRelativeX);
		gMouseRelativeX = 0;
		gMouseRelativeY = 0;
	}

	knotAngleX += deltaTimeInSecond*10;
	knotAngleY += deltaTimeInSecond*20;
	knotAngleZ += deltaTimeInSecond*30;

	gPhysics.UpdatePhysics(deltaTimeInSecond);
	gMat4Hero = gPhysics.GetObjectUpdatedMatrix(gCollisionHero);
}

void DrawGame(float truc)
{
	glClearColor(0.2f, 0.2f, 0.4f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Transformation & projection
	glm::mat4 model		= glm::mat4(1.0);
	glm::mat4 view		= gCamera.GetViewMatrix();
	glm::mat4 proj		= gCamera.GetProjMatrix();
	glm::vec3 lightPos	= glm::vec3(0,1000,0);
	glm::vec3 lightColor= glm::vec3(1,1,1);

	// Ciel
	gModelSky.Draw(gCamera,gMat4Sky,lightPos,lightColor);

	// Ville
	gModelCity.Draw(gCamera,gMat4City,lightPos,lightColor, true);

	// Hero
	gModelHero.Draw(gCamera,gMat4Hero,lightPos,lightColor);

	// Test Phong
	gMat4Knot = glm::mat4(1);
	gMat4Knot = glm::translate(gMat4Knot,glm::vec3(500,200,-500));
	gMat4Knot = glm::rotate(gMat4Knot,glm::radians(knotAngleX),glm::vec3(1,0,0));
	gMat4Knot = glm::rotate(gMat4Knot,glm::radians(knotAngleY),glm::vec3(0,1,0));
	gMat4Knot = glm::rotate(gMat4Knot,glm::radians(knotAngleZ),glm::vec3(0,0,1));
	gModelKnot.Draw(gCamera,gMat4Knot,lightPos,lightColor);

	
	float myVar = truc;
	model = glm::mat4(1);
	model = glm::rotate(model,glm::radians(myVar*360),glm::vec3(0,1,0));
//	model = glm::scale(model,glm::vec3(myVar,myVar,myVar));

	// Draw rectangle
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, TEX_Test);
	gShaderTest.Use();
	gShaderTest.SetUniform("texture_diffuse", 0);
	gShaderTest.SetUniform("lum", truc);
	gShaderTest.SetUniform("model",	model);
	gShaderTest.SetUniform("view",	view );
	gShaderTest.SetUniform("proj",	proj );
	glBindVertexArray(VAO_Test);
	glDrawArrays(GL_TRIANGLES, 0, 6);	// 6 vertices
	glBindVertexArray(0);

//	gFont.Draw("plop !",20,20,30.0f);
}

GLFWwindow* InitializeEverything()
{
	// Initialize GLFW
	glfwSetErrorCallback(callback_error);
	if (!glfwInit())
	{
		return NULL;
	}
	else
	{
		string version = glfwGetVersionString();
		ConsoleWriteOk("GLFW (window/input library) :");
		ConsoleWrite  (" -> version : %s", version.c_str());
	}

	// Setup OpenGL with GLFW (we set OpenGL 3.3 minimum, window not resizable)
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);


	// Create a windowed mode window and its OpenGL context
	GLFWwindow* pWindow = glfwCreateWindow(800, 600, "Hello World", NULL, NULL);
	if (pWindow == NULL)
	{
		glfwTerminate();
		return NULL;
	}
	InitInputs(pWindow);
	glfwMakeContextCurrent(pWindow);	// Make the window's context current

	glewExperimental = GL_TRUE;
	GLenum glewErr = glewInit();
	if (glewErr != GLEW_OK)
	{
		ConsoleWriteOk("GLew error : %s", glewGetErrorString(glewErr));
		return NULL;
	}
	else
	{
		string version = (const char*)glewGetString(GLEW_VERSION);
		ConsoleWriteOk("GLew (OpenGL extender) :");
		ConsoleWrite  (" -> version : %s", version.c_str());
	}

	// Setup Bullet
	gPhysics.Initialize();

	return pWindow;
}

int main()
{
	// Initialize libraries, create a window with an OpenGL context
	GLFWwindow* pWindow = InitializeEverything();
	if (pWindow == NULL)
	{
		return -1;
	}

	// Load images/3d objects/shaders before the main loop
	if (gModelSky.Load("meshes/sky.obj") == false)
	{
		ConsoleWriteErr("Failed to load 3D object");
	}
	if (gModelCity.Load("meshes/city/4/city.obj") == false)
	{
		ConsoleWriteErr("Failed to load 3D object");
	}
	if (gModelHero.Load("meshes/duck/duck_triangulate.obj") == false)
	{
		ConsoleWriteErr("Failed to load 3D object");
	}
	if (gModelKnot.Load("meshes/phong_test/knot.obj") == false)
	{
		ConsoleWriteErr("Failed to load 3D object");
	}
	gMat4City = glm::mat4(1);

	gMat4Hero = glm::mat4(1);
	gMat4Hero = glm::translate(gMat4Hero,glm::vec3(0,10000,0));

	gMat4Sky = glm::mat4(1);
	gMat4Sky = glm::scale(gMat4Sky,glm::vec3(20,20,20));

	gCollisionCity = gPhysics.AddStaticConcaveObjetToPhysic(&gModelCity,gMat4City);
	gCollisionHero = gPhysics.AddDynamicConvexHullToPhysic (&gModelHero,gMat4Hero,glm::vec3(0,0,0),100.0f,0.8f);


//	if (gFont.Load("images/ascii-font.png") == false)
//	{
//		ConsoleWriteErr("Failed to load font image");
//	}

	// Set OpenGL viewport
	int width, height;
	glfwGetFramebufferSize(pWindow, &width, &height);
	glViewport(0, 0, width, height);

	// Enable Z-buffer
	glEnable(GL_DEPTH_TEST);

	// Disable back face culling
	glDisable(GL_CULL_FACE);

	// Setup caméra
	gCamera.SetPosition(0,0,30);
	gCamera.SetSize(width, height);
	gCamera.SetViewAngle(50);
	gCamera.SetClipping(10, 7000*100);

	// Setup cube d'environnement
	InitializeOpenGLView(pWindow);


	// Loop until the user closes the window
	float lastTimeInSecond = (float)glfwGetTime();
	while (!glfwWindowShouldClose(pWindow))
	{
		// Update the game (logic/physic/etc...)
		float currTimeInSecond  = (float)glfwGetTime();
		float deltaTimeInSecond = currTimeInSecond - lastTimeInSecond;		// Duration of the last frame, useful to update the game physic.
		lastTimeInSecond = currTimeInSecond;
		UpdateGameLogic(deltaTimeInSecond);

		// Render here
		DrawGame(fabs(sin(lastTimeInSecond*2)));
//		DrawGame(lastTimeInSecond);

		// Swap front and back buffers
		glfwSwapBuffers(pWindow);

		// Poll for and process events -> Appel automatique si besoin des fonctions callback_xxx() définies plus haut.
		glfwPollEvents();
	}

	gPhysics.Terminate();
	glfwTerminate();
	return 0;
}

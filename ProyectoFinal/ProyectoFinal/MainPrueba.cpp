#include <iostream>
#include <cmath>

// GLEW
#include <GL/glew.h>

// GLFW
#include <GLFW/glfw3.h>

// Other Libs
#include "stb_image.h"

// GLM Mathematics
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

//Load Models
#include "SOIL2/SOIL2.h"


// Other includes
#include "Shader.h"
#include "Camera.h"
#include "Model.h"
#include "Texture.h"
#include "modelAnim.h"

// Function prototypes
void KeyCallback(GLFWwindow *window, int key, int scancode, int action, int mode);
void MouseCallback(GLFWwindow *window, double xPos, double yPos);
void DoMovement();
void animacion();

// Window dimensions
const GLuint WIDTH = 800, HEIGHT = 600;
int SCREEN_WIDTH, SCREEN_HEIGHT;

glm::vec3 cuboPosicion(0.0f, 0.0f, 0.0f);

// Camera
Camera  camera(glm::vec3(0.0f, 0.0f, 0.0f));
GLfloat lastX = WIDTH / 2.0;
GLfloat lastY = HEIGHT / 2.0;
bool keys[1024];
bool firstMouse = true;
float range = 0.0f;
float rot = 0.0f;
float movCamera = 0.0f;

// Light attributes
glm::vec3 lightPos(0.0f, 0.0f, 0.0f);
glm::vec3 PosIni(-95.0f, 1.0f, -45.0f);
glm::vec3 lightDirection(0.0f, -1.0f, -1.0f);


bool active;


// Deltatime
GLfloat deltaTime = 0.0f;	// Time between current frame and last frame
GLfloat lastFrame = 0.0f;  	// Time of last frame

//Door animation
bool door = false;
float rotDoor = 0.0f;

//Car animation
float movCarX = 0.0f;
float movCarZ = 0.0f;
float rotCar = 0.0f;
float rotWheel = 0.0f;
float carSpeed = 0.01f;
glm::vec3 iniPosCar(-25.8f, -4.85f, 31.8f);
bool circuit = false;
bool rout1 = true;
bool rout2 = false;
bool rout3 = false;
bool rout4 = false;

//Swan animation
float movSwanX = 0.0f;
float movSwanZ = 0.0f;
float rotSwan = 0.0f;
float rotSwanHead = 0.0f;
bool swanHeadRotPositive = true;
float swanSpeed = 0.005;
glm::vec3 iniPosSwan(-3.13f, -2.96f , 8.12f);
bool circuit1 = false;
bool rout5 = true;
bool rout6 = false;
bool rout7 = false;
bool rout8 = false;

// Dino skeleton animation
float rotDinoArms = 0;
float rotDinoLeg = 0;
float rotDinoMouth = 0;
float rotDinoTail = 0;
float dinoSpeed = 1.5f;
bool dinoAni = false;
bool dinoArmRotPositive = true;
bool dinoLegRotPositive = true;
bool dinoTailRotPositive = false;
bool dinoMouthRotPositive = true;

//Large dino animation
float movDinoLargeZ = 0;
float rotDinoLarge = 0;
float rotDinoLargeLeg0 = 0;
float rotDinoLargeLeg1 = 0;
float rotDinoLargeHead = 0;
float rotDinoLargeTail = 0;
bool dinoLargeLeg0RotPositive = true;
bool dinoLargeLeg1RotPositive = true;
bool dinoLargeHeadRotPositive = true;
bool dinoLargeTailRotPositive = true;
float dinoLargeAni = false;
float dinoLargeSpeed = 0.007;
bool rout9 = true;
bool rout10 = false;
glm::vec3 iniPosDinoLarge(-27.0, -5.3, -10.0);



// Keyframes
float posX =PosIni.x, posY = PosIni.y, posZ = PosIni.z, rotRodIzq = 0;

#define MAX_FRAMES 9
int i_max_steps = 190;
int i_curr_steps = 0;
typedef struct _frame
{
	//Variables para GUARDAR Key Frames
	float posX;		//Variable para PosicionX
	float posY;		//Variable para PosicionY
	float posZ;		//Variable para PosicionZ
	float incX;		//Variable para IncrementoX
	float incY;		//Variable para IncrementoY
	float incZ;		//Variable para IncrementoZ
	float rotRodIzq;
	float rotInc;

}FRAME;

FRAME KeyFrame[MAX_FRAMES];
int FrameIndex = 0;			//introducir datos
bool play = false;
int playIndex = 0;

// Positions of the point lights
glm::vec3 Light1 = glm::vec3(0);
glm::vec3 Light2 = glm::vec3(0);
glm::vec3 Light3 = glm::vec3(0);
glm::vec3 Light4 = glm::vec3(0);

glm::vec3 pointLightPositions[] = {
	glm::vec3(1.949998, -2.75, -10.910154),
	glm::vec3(-1.949998, -2.75, -10.910154),
	glm::vec3(-1.919998, -2.75, -7.100067),
	glm::vec3(+1.919998, -2.75, -7.100067)
};

// Spot lights positions
glm::vec3 spotLigth0(-9.39f,1.38f,-7.76f);
glm::vec3 spotLigth1(-6.7f, 1.38f, -13.0f);
glm::vec3 spotLigth2(0.0f, 1.38f, -16.0f);
glm::vec3 spotLigth3(6.7f, 1.38f, -13.0f);
glm::vec3 spotLigth4(9.39f, 1.38f, -7.76f);

glm::vec3 LightP1;




void saveFrame(void)
{

	printf("posx %f\n", posX);
	
	KeyFrame[FrameIndex].posX = posX;
	KeyFrame[FrameIndex].posY = posY;
	KeyFrame[FrameIndex].posZ = posZ;
	
	KeyFrame[FrameIndex].rotRodIzq = rotRodIzq;
	

	FrameIndex++;
}

void resetElements(void)
{
	posX = KeyFrame[0].posX;
	posY = KeyFrame[0].posY;
	posZ = KeyFrame[0].posZ;

	rotRodIzq = KeyFrame[0].rotRodIzq;

}

void interpolation(void)
{

	KeyFrame[playIndex].incX = (KeyFrame[playIndex + 1].posX - KeyFrame[playIndex].posX) / i_max_steps;
	KeyFrame[playIndex].incY = (KeyFrame[playIndex + 1].posY - KeyFrame[playIndex].posY) / i_max_steps;
	KeyFrame[playIndex].incZ = (KeyFrame[playIndex + 1].posZ - KeyFrame[playIndex].posZ) / i_max_steps;
	
	KeyFrame[playIndex].rotInc = (KeyFrame[playIndex + 1].rotRodIzq - KeyFrame[playIndex].rotRodIzq) / i_max_steps;

}




int main()
{
	// Init GLFW
	glfwInit();




	// Set all the required options for GLFW
	/*(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);*/

	// Create a GLFWwindow object that we can use for GLFW's functions
	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Proyecto GNRA", nullptr, nullptr);

	if (nullptr == window)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();

		return EXIT_FAILURE;
	}

	glfwMakeContextCurrent(window);

	glfwGetFramebufferSize(window, &SCREEN_WIDTH, &SCREEN_HEIGHT);

	// Set the required callback functions
	glfwSetKeyCallback(window, KeyCallback);
	glfwSetCursorPosCallback(window, MouseCallback);

	// GLFW Options
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// Set this to true so GLEW knows to use a modern approach to retrieving function pointers and extensions
	glewExperimental = GL_TRUE;
	// Initialize GLEW to setup the OpenGL Function pointers
	if (GLEW_OK != glewInit())
	{
		std::cout << "Failed to initialize GLEW" << std::endl;
		return EXIT_FAILURE;
	}

	// Define the viewport dimensions
	glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

	// OpenGL options
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	Shader lightingShader("Shaders/lighting.vs", "Shaders/lighting.frag");
	Shader lampShader("Shaders/lamp.vs", "Shaders/lamp.frag");
	//Shader SkyBoxshader("Shaders/SkyBox.vs", "Shaders/SkyBox.frag");
	Shader animShader("Shaders/anim.vs", "Shaders/anim.frag");
	Shader animAguaShader("Shaders/animAgua.vs", "Shaders/animAgua.frag");
	
	Model E_MarmolBlanco((char*)"Models/RecibidorModular/MarmolBlanco.obj");
	Model E_MarmolBeige((char*)"Models/RecibidorModular/MarmolBeige.obj");
	Model E_Madera((char*)"Models/RecibidorModular/Madera.obj");
	Model E_Paja((char*)"Models/RecibidorModular/Paja.obj");
	Model E_Vidrio((char*)"Models/RecibidorModular/Vidrio.obj");
	Model E_Arbustos((char*)"Models/RecibidorModular/Arbustos.obj");
	Model E_AguaEstatica((char*)"Models/RecibidorModular/AguaEstatica.obj");
	Model E_Agua((char*)"Models/RecibidorModular/Agua.obj");
	Model E_Concreto((char*)"Models/RecibidorModular/Concreto.obj");
	Model E_PuertaIzq((char*)"Models/RecibidorModular/PuertaIzq.obj");
	Model E_PuertaDer((char*)"Models/RecibidorModular/PuertaDer.obj");

	Model O_Andamios((char*)"Models/ObjetosEscenario/Andamios.obj");
	Model O_Cajas((char*)"Models/ObjetosEscenario/Cajas.obj");
	Model O_Unifilas((char*)"Models/ObjetosEscenario/Unifilas.obj");
	Model O_Cuadros((char*)"Models/ObjetosEscenario/Cuadros.obj");
	Model O_Cartel((char*)"Models/ObjetosEscenario/Cartel.obj");
	Model O_Macetas((char*)"Models/ObjetosEscenario/Macetas.obj");
	Model O_Pasto((char*)"Models/ObjetosEscenario/Pasto.obj");
	Model O_Pinos((char*)"Models/ObjetosEscenario/Pinos.obj");
	/*Model O_Arboles((char*)"Models/Arboles/Arboles.obj");*/
	Model O_Vallas((char*)"Models/ObjetosEscenario/Vallas.obj");
	Model O_SkyBox((char*)"Models/ObjetosEscenario/SkyBox.obj");
	

	Model Carroceria((char*)"Models/Jeep/Carroceria.obj");
	Model Llanta((char*)"Models/Jeep/Llanta.obj");
	
	Model CuerpoCisne((char*)"Models/Cisne/CuerpoCisne.obj");
	Model CabezaCisne((char*)"Models/Cisne/CabezaCisne.obj");

	Model DinoCoco((char*)"Models/DinoCoco/dinoCoco.obj");
	Model DinoCola((char*)"Models/DinoCoco/dinoCola.obj");
	Model DinoPata((char*)"Models/DinoCoco/dinoPata.obj");
	Model DinoBrazoIzq((char*)"Models/DinoCoco/dinoBrazoIzq.obj");
	Model DinoBrazoDer((char*)"Models/DinoCoco/dinoBrazoDer.obj");
	Model DinoMandibula((char*)"Models/DinoCoco/dinoMandibula.obj");

	Model DinoLargoCuerpo((char*)"Models/DinoLargo/DinoLargoCuerpo.obj");
	Model DinoLargoCabeza((char*)"Models/DinoLargo/DinoLargoCabeza.obj");
	Model DinoLargoCola((char*)"Models/DinoLargo/DinoLargoCola.obj");
	Model DinoLargoPiernaIzq((char*)"Models/DinoLargo/DinoLargoPiernaIzq.obj");
	Model DinoLargoPiernaDer((char*)"Models/DinoLargo/DinoLargoPiernaDer.obj");
	Model DinoLargoMusloIzq((char*)"Models/DinoLargo/DinoLargoMusloIzq.obj");
	Model DinoLargoMusloDer((char*)"Models/DinoLargo/DinoLargoMusloDer.obj");
	


	Model Cubo((char*)"Models/Cubo/Cubo.obj");

	//
	//for(int i=0; i<MAX_FRAMES; i++)
	//{
	//	KeyFrame[i].posX = 0;
	//	KeyFrame[i].incX = 0;
	//	KeyFrame[i].incY = 0;
	//	KeyFrame[i].incZ = 0;
	//	KeyFrame[i].rotRodIzq = 0;
	//	KeyFrame[i].rotInc = 0;
	//}



	// Set up vertex data (and buffer(s)) and attribute pointers
	GLfloat vertices[] =
	{
		// Positions            // Normals              // Texture Coords
		-0.5f, -0.5f, -0.5f,    0.0f,  0.0f, -1.0f,     0.0f,  0.0f,
		0.5f, -0.5f, -0.5f,     0.0f,  0.0f, -1.0f,     1.0f,  0.0f,
		0.5f,  0.5f, -0.5f,     0.0f,  0.0f, -1.0f,     1.0f,  1.0f,
		0.5f,  0.5f, -0.5f,     0.0f,  0.0f, -1.0f,     1.0f,  1.0f,
		-0.5f,  0.5f, -0.5f,    0.0f,  0.0f, -1.0f,     0.0f,  1.0f,
		-0.5f, -0.5f, -0.5f,    0.0f,  0.0f, -1.0f,     0.0f,  0.0f,

		-0.5f, -0.5f,  0.5f,    0.0f,  0.0f,  1.0f,     0.0f,  0.0f,
		0.5f, -0.5f,  0.5f,     0.0f,  0.0f,  1.0f,     1.0f,  0.0f,
		0.5f,  0.5f,  0.5f,     0.0f,  0.0f,  1.0f,     1.0f,  1.0f,
		0.5f,  0.5f,  0.5f,     0.0f,  0.0f,  1.0f,  	1.0f,  1.0f,
		-0.5f,  0.5f,  0.5f,    0.0f,  0.0f,  1.0f,     0.0f,  1.0f,
		-0.5f, -0.5f,  0.5f,    0.0f,  0.0f,  1.0f,     0.0f,  0.0f,

		-0.5f,  0.5f,  0.5f,    -1.0f,  0.0f,  0.0f,    1.0f,  0.0f,
		-0.5f,  0.5f, -0.5f,    -1.0f,  0.0f,  0.0f,    1.0f,  1.0f,
		-0.5f, -0.5f, -0.5f,    -1.0f,  0.0f,  0.0f,    0.0f,  1.0f,
		-0.5f, -0.5f, -0.5f,    -1.0f,  0.0f,  0.0f,    0.0f,  1.0f,
		-0.5f, -0.5f,  0.5f,    -1.0f,  0.0f,  0.0f,    0.0f,  0.0f,
		-0.5f,  0.5f,  0.5f,    -1.0f,  0.0f,  0.0f,    1.0f,  0.0f,

		0.5f,  0.5f,  0.5f,     1.0f,  0.0f,  0.0f,     1.0f,  0.0f,
		0.5f,  0.5f, -0.5f,     1.0f,  0.0f,  0.0f,     1.0f,  1.0f,
		0.5f, -0.5f, -0.5f,     1.0f,  0.0f,  0.0f,     0.0f,  1.0f,
		0.5f, -0.5f, -0.5f,     1.0f,  0.0f,  0.0f,     0.0f,  1.0f,
		0.5f, -0.5f,  0.5f,     1.0f,  0.0f,  0.0f,     0.0f,  0.0f,
		0.5f,  0.5f,  0.5f,     1.0f,  0.0f,  0.0f,     1.0f,  0.0f,

		-0.5f, -0.5f, -0.5f,    0.0f, -1.0f,  0.0f,     0.0f,  1.0f,
		0.5f, -0.5f, -0.5f,     0.0f, -1.0f,  0.0f,     1.0f,  1.0f,
		0.5f, -0.5f,  0.5f,     0.0f, -1.0f,  0.0f,     1.0f,  0.0f,
		0.5f, -0.5f,  0.5f,     0.0f, -1.0f,  0.0f,     1.0f,  0.0f,
		-0.5f, -0.5f,  0.5f,    0.0f, -1.0f,  0.0f,     0.0f,  0.0f,
		-0.5f, -0.5f, -0.5f,    0.0f, -1.0f,  0.0f,     0.0f,  1.0f,

		-0.5f,  0.5f, -0.5f,    0.0f,  1.0f,  0.0f,     0.0f,  1.0f,
		0.5f,  0.5f, -0.5f,     0.0f,  1.0f,  0.0f,     1.0f,  1.0f,
		0.5f,  0.5f,  0.5f,     0.0f,  1.0f,  0.0f,     1.0f,  0.0f,
		0.5f,  0.5f,  0.5f,     0.0f,  1.0f,  0.0f,     1.0f,  0.0f,
		-0.5f,  0.5f,  0.5f,    0.0f,  1.0f,  0.0f,     0.0f,  0.0f,
		-0.5f,  0.5f, -0.5f,    0.0f,  1.0f,  0.0f,     0.0f,  1.0f
	};


	GLfloat skyboxVertices[] = {
		// Positions
		-1.0f,  1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		-1.0f,  1.0f, -1.0f,
		1.0f,  1.0f, -1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		1.0f, -1.0f,  1.0f
	};


	GLuint indices[] =
	{  // Note that we start from 0!
		0,1,2,3,
		4,5,6,7,
		8,9,10,11,
		12,13,14,15,
		16,17,18,19,
		20,21,22,23,
		24,25,26,27,
		28,29,30,31,
		32,33,34,35
	};

	// Positions all containers
	glm::vec3 cubePositions[] = {
		glm::vec3(0.0f,  0.0f,  0.0f),
		glm::vec3(2.0f,  5.0f, -15.0f),
		glm::vec3(-1.5f, -2.2f, -2.5f),
		glm::vec3(-3.8f, -2.0f, -12.3f),
		glm::vec3(2.4f, -0.4f, -3.5f),
		glm::vec3(-1.7f,  3.0f, -7.5f),
		glm::vec3(1.3f, -2.0f, -2.5f),
		glm::vec3(1.5f,  2.0f, -2.5f),
		glm::vec3(1.5f,  0.2f, -1.5f),
		glm::vec3(-1.3f,  1.0f, -1.5f)
	};


	// First, set the container's VAO (and VBO)
	GLuint VBO, VAO, EBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// Position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid *)0);
	glEnableVertexAttribArray(0);
	// Normals attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid *)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);
	// Texture Coordinate attribute
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid *)(6 * sizeof(GLfloat)));
	glEnableVertexAttribArray(2);
	glBindVertexArray(0);

	// Then, we set the light's VAO (VBO stays the same. After all, the vertices are the same for the light object (also a 3D cube))
	GLuint lightVAO;
	glGenVertexArrays(1, &lightVAO);
	glBindVertexArray(lightVAO);
	// We only need to bind to the VBO (to link it with glVertexAttribPointer), no need to fill it; the VBO's data already contains all we need.
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	// Set the vertex attributes (only position data for the lamp))
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid *)0); // Note that we skip over the other data in our buffer object (we don't need the normals/textures, only positions).
	glEnableVertexAttribArray(0);
	glBindVertexArray(0);

	glm::mat4 projection = glm::perspective(camera.GetZoom(), (GLfloat)SCREEN_WIDTH / (GLfloat)SCREEN_HEIGHT, 0.1f, 1000.0f);

	// Game loop
	while (!glfwWindowShouldClose(window))
	{

		// Calculate deltatime of current frame
		GLfloat currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// Check if any events have been activiated (key pressed, mouse moved etc.) and call corresponding response functions
		glfwPollEvents();
		DoMovement();
		animacion();


		// Clear the colorbuffer
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


		// Use cooresponding shader when setting uniforms/drawing objects
		lightingShader.Use();
		GLint viewPosLoc = glGetUniformLocation(lightingShader.Program, "viewPos");
		glUniform3f(viewPosLoc, camera.GetPosition().x, camera.GetPosition().y, camera.GetPosition().z);
		// Set material properties
		glUniform1f(glGetUniformLocation(lightingShader.Program, "material.shininess"), 32.0f);
		// == ==========================
		// Here we set all the uniforms for the 5/6 types of lights we have. We have to set them manually and index
		// the proper PointLight struct in the array to set each uniform variable. This can be done more code-friendly
		// by defining light types as classes and set their values in there, or by using a more efficient uniform approach
		// by using 'Uniform buffer objects', but that is something we discuss in the 'Advanced GLSL' tutorial.
		// == ==========================
		// Directional light
		glUniform3f(glGetUniformLocation(lightingShader.Program, "dirLight.direction"), -0.2f, -1.0f, -0.3f);
		



		glm::vec3 lightColor;
		// Point light 1
		float time = glfwGetTime();
		float sinTime = sin(time);


		lightColor.x = abs(sinTime * Light1.x);
		lightColor.y = abs(sinTime * Light1.y);
		lightColor.z = sin(time * Light1.z);


		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[0].position"), pointLightPositions[0].x, pointLightPositions[0].y, pointLightPositions[0].z);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[0].ambient"), lightColor.x, lightColor.y, lightColor.z);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[0].diffuse"), lightColor.x, lightColor.y, lightColor.z);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[0].specular"), 1.0f, 1.0f, 0.0f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[0].constant"), 1.0f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[0].linear"), 0.045f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[0].quadratic"), 0.075f);


		// Point light 2

		lightColor.x = abs(sinTime * Light2.x);
		lightColor.y = abs(sinTime * Light2.y);
		lightColor.z = sin(time * Light2.z);

		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[1].position"), pointLightPositions[1].x, pointLightPositions[1].y, pointLightPositions[1].z);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[1].ambient"), lightColor.x, lightColor.y, lightColor.z);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[1].diffuse"), lightColor.x, lightColor.y, lightColor.z);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[1].specular"), 1.0f, 1.0f, 1.0f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[1].constant"), 1.0f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[1].linear"), 0.045f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[1].quadratic"), 0.075f);

		// Point light 3

		lightColor.x = abs(sinTime * Light3.x);
		lightColor.y = abs(sinTime * Light3.y);
		lightColor.z = sin(time * Light3.z);

		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[2].position"), pointLightPositions[2].x, pointLightPositions[2].y, pointLightPositions[2].z);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[2].ambient"), lightColor.x, lightColor.y, lightColor.z);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[2].diffuse"), lightColor.x, lightColor.y, lightColor.z);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[2].specular"), 1.0f, 1.0f, 1.0f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[2].constant"), 1.0f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[2].linear"), 0.045f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[2].quadratic"), 0.075f);

		// Point light 4

		lightColor.x = abs(sinTime * Light4.x);
		lightColor.y = abs(sinTime * Light4.y);
		lightColor.z = sin(time * Light4.z);

		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[3].position"), pointLightPositions[3].x, pointLightPositions[3].y, pointLightPositions[3].z);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[3].ambient"), lightColor.x, lightColor.y, lightColor.z);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[3].diffuse"), lightColor.x, lightColor.y, lightColor.z);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[3].specular"), 1.0f, 1.0f, 1.0f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[3].constant"), 1.0f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[3].linear"), 0.045f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[3].quadratic"), 0.075f);

		// SpotLight
		glUniform3f(glGetUniformLocation(lightingShader.Program, "spotLight[0].position"), spotLigth0.x, spotLigth0.y, spotLigth0.z);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "spotLight[0].direction"), 0.0f, -1.0f, 0.0f);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "spotLight[0].ambient"), 0.9f, 0.9f, 0.0f);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "spotLight[0].diffuse"), 0.9f, 0.9f, 0.0f);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "spotLight[0].specular"), 0.7f, 0.7f, 0.7f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "spotLight[0].constant"), 1.0f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "spotLight[0].linear"), 0.35f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "spotLight[0].quadratic"), 0.44f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "spotLight[0].cutOff"), glm::cos(glm::radians(22.0f)));
		glUniform1f(glGetUniformLocation(lightingShader.Program, "spotLight[0].outerCutOff"), glm::cos(glm::radians(25.0f)));

		// SpotLight1
		glUniform3f(glGetUniformLocation(lightingShader.Program, "spotLight[1].position"), spotLigth1.x, spotLigth1.y, spotLigth1.z);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "spotLight[1].direction"), 0.0f, -1.0f, 0.0f);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "spotLight[1].ambient"), 0.9f, 0.9f, 0.0f);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "spotLight[1].diffuse"), 0.9f, 0.9f, 0.0f);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "spotLight[1].specular"), 0.7f, 0.7f, 0.7f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "spotLight[1].constant"), 1.0f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "spotLight[1].linear"), 0.35f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "spotLight[1].quadratic"), 0.44f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "spotLight[1].cutOff"), glm::cos(glm::radians(22.0f)));
		glUniform1f(glGetUniformLocation(lightingShader.Program, "spotLight[1].outerCutOff"), glm::cos(glm::radians(25.0f)));

		// SpotLight2
		glUniform3f(glGetUniformLocation(lightingShader.Program, "spotLight[2].position"), spotLigth2.x, spotLigth2.y, spotLigth2.z);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "spotLight[2].direction"), 0.0f, -1.0f, 0.0f);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "spotLight[2].ambient"), 0.9f, 0.9f, 0.0f);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "spotLight[2].diffuse"), 0.9f, 0.9f, 0.0f);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "spotLight[2].specular"), 0.7f, 0.7f, 0.7f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "spotLight[2].constant"), 1.0f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "spotLight[2].linear"), 0.35f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "spotLight[2].quadratic"), 0.44f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "spotLight[2].cutOff"), glm::cos(glm::radians(22.0f)));
		glUniform1f(glGetUniformLocation(lightingShader.Program, "spotLight[2].outerCutOff"), glm::cos(glm::radians(25.0f)));

		// SpotLight3
		glUniform3f(glGetUniformLocation(lightingShader.Program, "spotLight[3].position"), spotLigth3.x, spotLigth3.y, spotLigth3.z);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "spotLight[3].direction"), 0.0f, -1.0f, 0.0f);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "spotLight[3].ambient"), 0.9f, 0.9f, 0.0f);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "spotLight[3].diffuse"), 0.9f, 0.9f, 0.0f);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "spotLight[3].specular"), 0.7f, 0.7f, 0.7f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "spotLight[3].constant"), 1.0f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "spotLight[3].linear"), 0.35f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "spotLight[3].quadratic"), 0.44f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "spotLight[3].cutOff"), glm::cos(glm::radians(22.0f)));
		glUniform1f(glGetUniformLocation(lightingShader.Program, "spotLight[3].outerCutOff"), glm::cos(glm::radians(25.0f)));

		// SpotLight4
		glUniform3f(glGetUniformLocation(lightingShader.Program, "spotLight[4].position"), spotLigth4.x, spotLigth4.y, spotLigth4.z);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "spotLight[4].direction"), 0.0f, -1.0f, 0.0f);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "spotLight[4].ambient"), 0.9f, 0.9f, 0.0f);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "spotLight[4].diffuse"), 0.9f, 0.9f, 0.0f);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "spotLight[4].specular"), 0.7f, 0.7f, 0.7f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "spotLight[4].constant"), 1.0f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "spotLight[4].linear"), 0.35f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "spotLight[4].quadratic"), 0.44f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "spotLight[4].cutOff"), glm::cos(glm::radians(22.0f)));
		glUniform1f(glGetUniformLocation(lightingShader.Program, "spotLight[4].outerCutOff"), glm::cos(glm::radians(25.0f)));

		

		// Create camera transformations
		glm::mat4 view;
		view = camera.GetViewMatrix();


		// Get the uniform locations
		GLint modelLoc = glGetUniformLocation(lightingShader.Program, "model");
		GLint viewLoc = glGetUniformLocation(lightingShader.Program, "view");
		GLint projLoc = glGetUniformLocation(lightingShader.Program, "projection");

		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

		glUniform3f(glGetUniformLocation(lightingShader.Program, "dirLight.ambient"), 0.2f, 0.2f, 0.2f);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "dirLight.diffuse"), 0.2f, 0.3f, 0.2f);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "dirLight.specular"), 0.7f, 0.7f, 0.7f);
		glm::mat4 model(1);
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.001f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		E_MarmolBlanco.Draw(lightingShader);

		model = glm::mat4(1);
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.001f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		E_MarmolBeige.Draw(lightingShader);

		model = glm::mat4(1);
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.001f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		E_Madera.Draw(lightingShader);

		model = glm::mat4(1);
		model = glm::translate(model, cuboPosicion);
		model = glm::scale(model, glm::vec3(0.1f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		Cubo.Draw(lightingShader);

		model = glm::mat4(1);
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.001f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		E_Paja.Draw(lightingShader);


		model = glm::mat4(1);
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.001f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		E_Arbustos.Draw(lightingShader);

		model = glm::mat4(1);
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.001f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		E_AguaEstatica.Draw(lightingShader);

		glUniform3f(glGetUniformLocation(lightingShader.Program, "dirLight.specular"), 0.2f, 0.2f, 0.2f);
		model = glm::mat4(1);
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.001f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		E_Concreto.Draw(lightingShader);

		glUniform3f(glGetUniformLocation(lightingShader.Program, "dirLight.specular"), 0.2f, 0.2f, 0.2f);
		model = glm::mat4(1);
		model = glm::translate(model, glm::vec3(-1.0f, -2.75f, 4.9f));
		model = glm::scale(model, glm::vec3(0.001f));
		model = glm::rotate(model, glm::radians(rotDoor), glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		E_PuertaIzq.Draw(lightingShader);

		glUniform3f(glGetUniformLocation(lightingShader.Program, "dirLight.specular"), 0.2f, 0.2f, 0.2f);
		model = glm::mat4(1);
		model = glm::translate(model, glm::vec3(1.0f, -2.75f, 4.9f));
		model = glm::scale(model, glm::vec3(0.001f));
		model = glm::rotate(model, glm::radians(-rotDoor), glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		E_PuertaDer.Draw(lightingShader);

		glUniform3f(glGetUniformLocation(lightingShader.Program, "dirLight.specular"), 0.7f, 0.7f, 0.7f);
		model = glm::mat4(1);
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.001f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		O_Andamios.Draw(lightingShader);

		model = glm::mat4(1);
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.001f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		O_Cajas.Draw(lightingShader);

		model = glm::mat4(1);
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.001f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		O_Unifilas.Draw(lightingShader);

		model = glm::mat4(1);
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.001f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		O_Cuadros.Draw(lightingShader);


		model = glm::mat4(1);
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.001f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		O_Cartel.Draw(lightingShader);


		model = glm::mat4(1);
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.001f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		O_Macetas.Draw(lightingShader);

		model = glm::mat4(1);
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.001f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		O_Pasto.Draw(lightingShader);

		model = glm::mat4(1);
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.001f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		O_Vallas.Draw(lightingShader);

		model = glm::mat4(1);
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.001f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		O_SkyBox.Draw(lightingShader);

		glm::mat4 tmp = glm::mat4(1.0f);

		model = glm::mat4(1);
		tmp = model = glm::translate(model, iniPosCar + glm::vec3(movCarX, 0.0f, movCarZ));
		model = glm::scale(model, glm::vec3(0.1f));
		model = glm::rotate(model, glm::radians(rotCar), glm::vec3(0.0f, 1.0f, 0.0));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		Carroceria.Draw(lightingShader);
		
		model = glm::mat4(1);
		model = glm::translate(model, iniPosCar + glm::vec3(movCarX, 0, movCarZ));
		model = glm::rotate(model, glm::radians(rotCar), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::translate(model, glm::vec3(1.55f, 0.0f, 0.8f));
		model = glm::rotate(model, glm::radians(rotWheel), glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(0.1f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		Llanta.Draw(lightingShader);

		model = glm::mat4(1);
		model = glm::translate(model, iniPosCar + glm::vec3(movCarX, 0, movCarZ));
		model = glm::rotate(model, glm::radians(rotCar), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::translate(model, glm::vec3(-1.55f, 0.0f, 0.8f));
		model = glm::rotate(model, glm::radians(rotWheel), glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(0.1f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		Llanta.Draw(lightingShader);

		model = glm::mat4(1);
		model = glm::translate(model, iniPosCar + glm::vec3(movCarX, 0, movCarZ));
		model = glm::rotate(model, glm::radians(rotCar), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::translate(model, glm::vec3(1.55f, 0.0f, -1.1f));
		model = glm::rotate(model, glm::radians(rotWheel), glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.1f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		Llanta.Draw(lightingShader);

		model = glm::mat4(1);
		model = glm::translate(model, iniPosCar + glm::vec3(movCarX, 0, movCarZ));
		model = glm::rotate(model, glm::radians(rotCar), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::translate(model, glm::vec3(-1.55f, 0.0f, -1.1f));
		model = glm::rotate(model, glm::radians(rotWheel), glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.1f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		Llanta.Draw(lightingShader);

		model = glm::mat4(1);
		tmp = model = glm::translate(model, iniPosSwan + glm::vec3(movSwanX, 0.0f, movSwanZ));
		model = glm::scale(model, glm::vec3(0.008f));
		model = glm::rotate(model, glm::radians(rotSwan), glm::vec3(0.0f, 1.0f, 0.0));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		CuerpoCisne.Draw(lightingShader);

		model = glm::mat4(1);
		model = glm::translate(model, iniPosSwan + glm::vec3(movSwanX, 0, movSwanZ));
		model = glm::rotate(model, glm::radians(rotSwan), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::translate(model, glm::vec3(-0.004f, 0.257f, 0.109f));
		model = glm::rotate(model, glm::radians(rotSwanHead), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.008f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		CabezaCisne.Draw(lightingShader);


		model = glm::mat4(1);
		model = glm::translate(model, glm::vec3(0.6, -3.0, -8.2));
		model = glm::scale(model, glm::vec3(0.3f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		DinoCoco.Draw(lightingShader);

		model = glm::mat4(1);
		model = glm::translate(model, glm::vec3(0.48, 0.79, -7.81));
		model = glm::scale(model, glm::vec3(0.3f));
		model = glm::rotate(model, glm::radians(rotDinoTail), glm::vec3(1.0f, 0.0f, 1.0f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		DinoCola.Draw(lightingShader);

		model = glm::mat4(1);
		model = glm::translate(model, glm::vec3(0.13, 0.7, -7.81));
		model = glm::scale(model, glm::vec3(0.3f));
		model = glm::rotate(model, glm::radians(rotDinoLeg), glm::vec3(0.0f, 0.0f, 1.0f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		DinoPata.Draw(lightingShader);

		model = glm::mat4(1);
		model = glm::translate(model, glm::vec3(-1.17, 0.77, -8.60));
		model = glm::scale(model, glm::vec3(0.3f));
		model = glm::rotate(model, glm::radians(rotDinoArms), glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		DinoBrazoIzq.Draw(lightingShader);

		model = glm::mat4(1);
		model = glm::translate(model, glm::vec3(-0.90, 0.77, -9.05));
		model = glm::scale(model, glm::vec3(0.3f));
		model = glm::rotate(model, glm::radians(rotDinoArms), glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		DinoBrazoDer.Draw(lightingShader);

		model = glm::mat4(1);
		model = glm::translate(model, glm::vec3(-1.79, 1.26, -9.32));
		model = glm::scale(model, glm::vec3(0.3f));
		model = glm::rotate(model, glm::radians(rotDinoMouth), glm::vec3(0.0f, 0.0f, 1.0f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		DinoMandibula.Draw(lightingShader);

		model = glm::mat4(1);
		model = glm::translate(model, iniPosDinoLarge + glm::vec3(0, 0, movDinoLargeZ));
		model = glm::rotate(model, glm::radians(rotDinoLarge), glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		DinoLargoCuerpo.Draw(lightingShader);
		
		model = glm::mat4(1);
		model = glm::translate(model, iniPosDinoLarge + glm::vec3(0, 0, movDinoLargeZ));
		model = glm::rotate(model, glm::radians(rotDinoLarge), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::translate(model, glm::vec3(0.55f, 3.5f, 2.2f));
		model = glm::rotate(model, glm::radians(rotDinoLargeLeg0), glm::vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		DinoLargoPiernaIzq.Draw(lightingShader);

		model = glm::mat4(1);
		model = glm::translate(model, iniPosDinoLarge + glm::vec3(0, 0, movDinoLargeZ));
		model = glm::rotate(model, glm::radians(rotDinoLarge), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::translate(model, glm::vec3(-0.86f, 3.54f, 2.3f));
		model = glm::rotate(model, glm::radians(-rotDinoLargeLeg0), glm::vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		DinoLargoPiernaDer.Draw(lightingShader);

		model = glm::mat4(1);
		model = glm::translate(model, iniPosDinoLarge + glm::vec3(0, 0, movDinoLargeZ));
		model = glm::rotate(model, glm::radians(rotDinoLarge), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::translate(model, glm::vec3(0.49f, 4.3f, -1.8f));
		model = glm::rotate(model, glm::radians(-rotDinoLargeLeg0), glm::vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		DinoLargoMusloIzq.Draw(lightingShader);

		model = glm::mat4(1);
		model = glm::translate(model, iniPosDinoLarge + glm::vec3(0, 0, movDinoLargeZ));
		model = glm::rotate(model, glm::radians(rotDinoLarge), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::translate(model, glm::vec3(-0.67f, 4.3f, -1.8f));
		model = glm::rotate(model, glm::radians(rotDinoLargeLeg0), glm::vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		DinoLargoMusloDer.Draw(lightingShader);

		model = glm::mat4(1);
		model = glm::translate(model, iniPosDinoLarge + glm::vec3(0, 0, movDinoLargeZ));
		model = glm::rotate(model, glm::radians(rotDinoLarge), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::translate(model, glm::vec3(-0.17, 5.5, 2.2));
		model = glm::rotate(model, glm::radians(rotDinoLargeLeg0), glm::vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		DinoLargoCabeza.Draw(lightingShader);

		model = glm::mat4(1);
		model = glm::translate(model, iniPosDinoLarge + glm::vec3(0, 0, movDinoLargeZ));
		model = glm::rotate(model, glm::radians(rotDinoLarge), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::translate(model, glm::vec3(-0.23, 3.5, -3.3));
		model = glm::rotate(model, glm::radians(-rotDinoLargeLeg0), glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		DinoLargoCola.Draw(lightingShader);

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		
		glUniform1i(glGetUniformLocation(lightingShader.Program, "activaTransparencia"), 0);
		
		glUniform4f(glGetUniformLocation(lightingShader.Program, "colorAlpha"), 1.0, 1.0, 1.0, 0.35);
		model = glm::mat4(1);
		model = glm::scale(model, glm::vec3(0.001f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		E_Vidrio.Draw(lightingShader);

		glUniform4f(glGetUniformLocation(lightingShader.Program, "colorAlpha"), 1.0, 1.0, 1.0, 80.0);
		model = glm::mat4(1);
		model = glm::scale(model, glm::vec3(0.001f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		O_Pinos.Draw(lightingShader);

		/*model = glm::mat4(1);
		model = glm::scale(model, glm::vec3(0.001f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		O_Arboles.Draw(lightingShader);*/

		glDisable(GL_BLEND);
		glUniform4f(glGetUniformLocation(lightingShader.Program, "colorAlpha"), 1.0, 1.0, 1.0, 0.0); 

		glBindVertexArray(0);

		animAguaShader.Use();
		float tiempo = glfwGetTime();
		modelLoc = glGetUniformLocation(animAguaShader.Program, "model");
		viewLoc = glGetUniformLocation(animAguaShader.Program, "view");
		projLoc = glGetUniformLocation(animAguaShader.Program, "projection");
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		model = glm::mat4(1);
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.001f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		glUniform1f(glGetUniformLocation(animAguaShader.Program, "time"), tiempo);
		E_Agua.Draw(animAguaShader);
		glBindVertexArray(0);

		glDepthFunc(GL_LESS); 

		// Swap the screen buffers
		glfwSwapBuffers(window);
	}

	glDeleteVertexArrays(1, &VAO);
	glDeleteVertexArrays(1, &lightVAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
	// Terminate GLFW, clearing any resources allocated by GLFW.
	glfwTerminate();

	return 0;
}


void animacion()
{

		//Movimiento del personaje

		if (play)
		{
			if (i_curr_steps >= i_max_steps) //end of animation between frames?
			{
				playIndex++;
				if (playIndex>FrameIndex - 2)	//end of total animation?
				{
					printf("termina anim\n");
					playIndex = 0;
					play = false;
				}
				else //Next frame interpolations
				{
					i_curr_steps = 0; //Reset counter
									  //Interpolation
					interpolation();
				}
			}
			else
			{
				//Draw animation
				posX += KeyFrame[playIndex].incX;
				posY += KeyFrame[playIndex].incY;
				posZ += KeyFrame[playIndex].incZ;

				rotRodIzq += KeyFrame[playIndex].rotInc;

				i_curr_steps++;
			}

		}



		//Door movement
		if (door)
		{
			if (rotDoor < 90) {
				rotDoor += 0.15f;
			}
		}
		else
		{
			if (rotDoor > 0)
			{
				rotDoor -= 0.15f;
			}
		}
		// Car circuit
		if (circuit)
		{
			rotWheel -= 0.38;

			if (rout1)
			{
				rotCar = 0.0f;
				movCarX += carSpeed;
				
				
				if ( (iniPosCar.x + movCarX) > 25.8f)
				{
					rout1 = false;
					rout2 = true;
				}
			}
			if (rout2)
			{
				rotCar = 90;
				movCarZ -= carSpeed;
				if ( (iniPosCar.z + movCarZ) < 19.0f)
				{
					rout2 = false;
					rout3 = true;
				}
			}

			if (rout3)
			{
				rotCar = 180;
				movCarX -= carSpeed;
				if ((iniPosCar.x + movCarX ) < -25.8)
				{
					rout3 = false;
					rout4 = true;
				}
			}
			if (rout4)
			{
				rotCar = 270;
				movCarZ += carSpeed;
				if ((iniPosCar.z + movCarZ) > 31.8)
				{
					rout4 = false;
					rout1 = true;
				}
			}


		}
		//Swan circuit
		if (circuit1)
		{
			

			if (swanHeadRotPositive)
			{
				rotSwanHead += 0.38;
				if( rotSwanHead > 22.5f )
					swanHeadRotPositive = false;
			}
			else
			{
				rotSwanHead -= 0.38;
				if (rotSwanHead < -45.0f)
					swanHeadRotPositive = true;
			}

			if (rout5)
			{
				rotSwan = 0.0f;
				movSwanZ += swanSpeed;


				if ((iniPosSwan.z + movSwanZ) > 10.68f)
				{
					rout5 = false;
					rout6 = true;
				}
			}
			if (rout6)
			{
				rotSwan = -90;
				movSwanX -= swanSpeed;
				if ((iniPosSwan.x + movSwanX) < -6.54f)
				{
					rout6 = false;
					rout7 = true;
				}
			}

			if (rout7)
			{
				rotSwan = -180;
				movSwanZ -= swanSpeed;
				if ((iniPosSwan.z + movSwanZ) < 8.12f)
				{
					rout7 = false;
					rout8 = true;
				}
			}

			if (rout8)
			{
				rotSwan = 90;
				movSwanX += swanSpeed;
				if ((iniPosSwan.x + movSwanX) > -3.13f )
				{
					rout8 = false;
					rout5 = true;
				}
			}


		}
		//dinosaur animation 
		if (dinoAni)
		{
			if (dinoArmRotPositive)
			{
				rotDinoArms += dinoSpeed;
				if (rotDinoArms > 45.0f)
					dinoArmRotPositive = false;
			}
			else
			{
				rotDinoArms -= dinoSpeed;
				if (rotDinoArms < -45.0f)
					dinoArmRotPositive = true;
			}

			if (dinoLegRotPositive)
			{
				rotDinoLeg += dinoSpeed;
				if (rotDinoLeg > 45.0f)
					dinoLegRotPositive = false;
			}
			else
			{
				rotDinoLeg -= dinoSpeed;
				if (rotDinoLeg < -45.0f)
					dinoLegRotPositive = true;
			}

			if (dinoTailRotPositive)
			{
				rotDinoTail += dinoSpeed;
				if (rotDinoTail > 45.0f)
					dinoTailRotPositive = false;
			}
			else
			{
				rotDinoTail -= dinoSpeed;
				if (rotDinoTail < -45.0f)
					dinoTailRotPositive = true;
			}

			if (dinoMouthRotPositive)
			{
				rotDinoMouth += dinoSpeed;
				if (rotDinoMouth > 25.0f)
					dinoMouthRotPositive = false;
			}
			else
			{
				rotDinoMouth -= dinoSpeed;
				if (rotDinoMouth < 0.0f)
					dinoMouthRotPositive = true;
			}
		}
		//pointLigths
		if (dinoAni)
		{
			Light1 = glm::vec3(1.0f, 0.0f, 0.0f);
			Light2 = glm::vec3(0.0f, 1.0f, 0.0f);
			Light3 = glm::vec3(0.0f, 0.0f, 1.0f);
			Light4 = glm::vec3(1.0f, 0.0f, 1.0f);
		}
		else
		{
			Light1 = glm::vec3(0);
			Light2 = glm::vec3(0);
			Light3 = glm::vec3(0);
			Light4 = glm::vec3(0);
		}
		//large dinosaur animation 
		if (dinoLargeAni)
		{
			rotDinoLargeHead -= 0.3;
			rotDinoLargeTail -= 0.3;
			rotDinoLargeLeg1 += 0.3;

			if (dinoLargeLeg0RotPositive)
			{
				rotDinoLargeLeg0 -= 0.15;
				if (rotDinoLargeLeg0 < -20.0f)
					dinoLargeLeg0RotPositive = false;
			}
			else
			{
				rotDinoLargeLeg0 += 0.15;
				if (rotDinoLargeLeg0 > 20.0f)
					dinoLargeLeg0RotPositive = true;
			}

			if (rout9)
			{
				rotDinoLarge = 0.0f;
				movDinoLargeZ += dinoLargeSpeed;


				if ((iniPosDinoLarge.z + movDinoLargeZ) > 15.0f)
				{
					rout9 = false;
					rout10 = true;
				}
			}
			if (rout10)
			{
				rotDinoLarge = 180;
				movDinoLargeZ -= dinoLargeSpeed;
				if ((iniPosDinoLarge.z + movDinoLargeZ) < -10.0f)
				{
					rout10 = false;
					rout9 = true;
				}
			}
		}

	}


// Is called whenever a key is pressed/released via GLFW
void KeyCallback(GLFWwindow *window, int key, int scancode, int action, int mode)
{
	if (keys[GLFW_KEY_L])
	{
		if (play == false && (FrameIndex > 1))
		{

			resetElements();
			//First Interpolation				
			interpolation();

			play = true;
			playIndex = 0;
			i_curr_steps = 0;
		}
		else
		{
			play = false;
		}

	}

	if (keys[GLFW_KEY_K])
	{
		if (FrameIndex<MAX_FRAMES)
		{
			saveFrame();
		}

		rot =-25.0f;//Variable que maneja el giro de la camara

	}


	if (GLFW_KEY_ESCAPE == key && GLFW_PRESS == action)
	{
		glfwSetWindowShouldClose(window, GL_TRUE);
	}

	if (key >= 0 && key < 1024)
	{
		if (action == GLFW_PRESS)
		{
			keys[key] = true;
		}
		else if (action == GLFW_RELEASE)
		{
			keys[key] = false;
		}
	}

	if (keys[GLFW_KEY_SPACE])
	{
		active = !active;
		if (active)
			LightP1 = glm::vec3(1.0f, 0.0f, 0.0f);
		else
			LightP1 = glm::vec3(0.0f, 0.0f, 0.0f);
	}

	if (keys[GLFW_KEY_X])
	{
		if (!door)
		{
			door = true;
		}
		else
		{
			door = false;
		}
	}

	if (keys[GLFW_KEY_C])
	{
		if (!circuit)
		{
			circuit = true;
		}
		else
		{
			circuit = false;
		}
	}

	if (keys[GLFW_KEY_V])
	{
		if (!circuit1)
		{
			circuit1 = true;
		}
		else
		{
			circuit1 = false;
		}
	}

	if (keys[GLFW_KEY_B])
	{
		if (!dinoAni)
		{
			dinoAni = true;
		}
		else
		{
			dinoAni = false;
		}
	}
	
	if (keys[GLFW_KEY_N])
	{
		if (!dinoLargeAni)
		{
			dinoLargeAni = true;
		}
		else
		{
			dinoLargeAni = false;
		}
	}

	if (keys[GLFW_KEY_Z])
	{
		printf("\n%f, %f, %f",cuboPosicion.x, cuboPosicion.y, cuboPosicion.z);
	}
}

void MouseCallback(GLFWwindow *window, double xPos, double yPos)
{

	if (firstMouse)
	{
		lastX = xPos;
		lastY = yPos;
		firstMouse = false;
	}

	GLfloat xOffset = xPos - lastX;
	GLfloat yOffset = lastY - yPos;  // Reversed since y-coordinates go from bottom to left

	lastX = xPos;
	lastY = yPos;

	camera.ProcessMouseMovement(xOffset, yOffset);
}

// Moves/alters the camera positions based on user input
void DoMovement()
{

	if (keys[GLFW_KEY_W] || keys[GLFW_KEY_UP])
	{
		camera.ProcessKeyboard(FORWARD, deltaTime);

	}

	if (keys[GLFW_KEY_S] || keys[GLFW_KEY_DOWN])
	{
		camera.ProcessKeyboard(BACKWARD, deltaTime);


	}

	if (keys[GLFW_KEY_A] || keys[GLFW_KEY_LEFT])
	{
		camera.ProcessKeyboard(LEFT, deltaTime);


	}

	if (keys[GLFW_KEY_D] || keys[GLFW_KEY_RIGHT])
	{
		camera.ProcessKeyboard(RIGHT, deltaTime);


	}

	

	////Mov Personaje
	//if (keys[GLFW_KEY_H])
	//{
	//	posZ += 1;
	//}

	//if (keys[GLFW_KEY_Y])
	//{
	//	posZ -= 1;
	//}

	//if (keys[GLFW_KEY_G])
	//{
	//	posX -= 1;
	//}

	//if (keys[GLFW_KEY_J])
	//{
	//	posX += 1;
	//}


	if (keys[GLFW_KEY_T])
	{
		cuboPosicion.x += 0.01f;
	}
	if (keys[GLFW_KEY_G])
	{
		cuboPosicion.x -= 0.01f;
	}

	if (keys[GLFW_KEY_Y])
	{
		cuboPosicion.y += 0.01f;
	}

	if (keys[GLFW_KEY_H])
	{
		cuboPosicion.y -= 0.01f;
	}
	if (keys[GLFW_KEY_U])
	{
		cuboPosicion.z -= 0.01f;
	}
	if (keys[GLFW_KEY_J])
	{
		cuboPosicion.z += 0.01f;
	}

	// Camera controls
	if (keys[GLFW_KEY_W] || keys[GLFW_KEY_UP])
	{
		camera.ProcessKeyboard(FORWARD, deltaTime);

	}

	if (keys[GLFW_KEY_S] || keys[GLFW_KEY_DOWN])
	{
		camera.ProcessKeyboard(BACKWARD, deltaTime);


	}

	if (keys[GLFW_KEY_A] || keys[GLFW_KEY_LEFT])
	{
		camera.ProcessKeyboard(LEFT, deltaTime);


	}

	if (keys[GLFW_KEY_D] || keys[GLFW_KEY_RIGHT])
	{
		camera.ProcessKeyboard(RIGHT, deltaTime);
	}






}
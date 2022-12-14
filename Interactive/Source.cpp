// 3D Graphics and Animation - Main Template
// This uses Visual Studio Code - https://code.visualstudio.com/docs/cpp/introvideos-cpp
// Two versions available -  Win64 and Apple MacOS - please see notes
// Last changed August 2022

//#pragma comment(linker, "/NODEFAULTLIB:MSVCRT")

//#define STB_IMAGE_IMPLEMENTATION

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
using namespace std;

#define GL_SILENCE_DEPRECATION
#include <glad/glad.h> // Extend OpenGL Specfication to version 4.5 for WIN64 and versions 4.1 for Apple (note: two different files).

#include <imgui/imgui.h>			  // Load GUI library - Dear ImGui - https://github.com/ocornut/imgui
#include <imgui/imgui_impl_glfw.h>	  // Platform ImGui using GLFW
#include <imgui/imgui_impl_opengl3.h> // Platform new OpenGL - aka better than 3.3 core version.

#include <GLFW/glfw3.h> // Add GLFW - library to launch a window and callback mouse and keyboard - https://www.glfw.org

#define GLM_ENABLE_EXPERIMENTAL	 // Enable all functions include non LTS
#include <glm/glm.hpp>			 // Add helper maths library - GLM 0.9.9.9 - https://github.com/g-truc/glm - for example variables vec3, mat and operators.
#include <glm/gtx/transform.hpp> // Help us with transforms
using namespace glm;
 
//#include <tinygltf/tiny_gltf.h> // Model loading library - tiny gltf - https://github.com/syoyo/tinygltf
//#include "src/stb_image.hpp" // Add library to load images for textures

//#include "src/Mesh.hpp" // Simplest mesh holder and OBJ loader - can update more - from https://github.com/BennyQBD/ModernOpenGLTutorial

#include "src/Pipeline.hpp"		// Setup pipeline and load shaders.
#include "src/Content.hpp"		// Setup content loader and drawing functions - https://github.com/KhronosGroup/glTF - https://github.com/syoyo/tinygltf 
#include "src/Debugger.hpp"		// Setup debugger functions.



// Main fuctions
void startup();
void update();
void render();
void ui();
void endProgram();
//void scroll_callback(); was doing this for mouse scroll but found the callback function at the bottom of the file!

// HELPER FUNCTIONS OPENGL
void hintsGLFW();
//string readShader(string name);
//void checkErrorShader(GLuint shader);
inline void errorCallbackGLFW(int error, const char *description){cout << "Error GLFW: " << description << "\n";};
void debugGL();

void APIENTRY openGLDebugCallback(GLenum source,
								  GLenum type,
								  GLuint id,
								  GLenum severity,
								  GLsizei length,
								  const GLchar *message,
								  const GLvoid *userParam);
GLenum glCheckError_(const char *file, int line);
#define glCheckError() glCheckError_(__FILE__, __LINE__)

// Setup all the message loop callbacks - do this before Dear ImGui
// Callback functions for the window and interface devices
void onResizeCallback(GLFWwindow *window, int w, int h);
void onKeyCallback(GLFWwindow *window, int key, int scancode, int action, int mods);
void onMouseButtonCallback(GLFWwindow *window, int button, int action, int mods);
void onMouseMoveCallback(GLFWwindow *window, double x, double y);
void onMouseWheelCallback(GLFWwindow *window, double xoffset, double yoffset);

//creating a struct to store content, model position and rotation, scale and a modelMatrix for each model individually (editable further in scene)
struct renderObject {
	Content c;
	vec3 modRotation;
	vec3 modPosition;
	mat4 modelMatrix;
	vec3 scale;
};


// VARIABLES
GLFWwindow *window; 								// Keep track of the window
auto windowWidth = 1024;							// Window width (updated to run 1024 by Brendan)			
auto windowHeight = 700;							// Window height (updated to run 700 by Brendan)
auto running(true);							  		// Are we still running our main loop
mat4 projMatrix;							 		// Our Projection Matrix
vec3 cameraPosition = vec3(0.0f, 0.0f, 5.0f);		// Where is our camera
vec3 cameraFront = vec3(0.0f, 0.0f, -1.0f);			// Camera front vector
vec3 cameraUp = vec3(0.0f, 1.0f, 0.0f);				// Camera up vector
auto aspect = (float)windowWidth / (float)windowHeight;	// Window aspect ration
auto fovy = 95.0f;									// Field of view (y axis), updated to 95 here for nicer view by Brendan
bool keyStatus[1024];								// Track key strokes
auto currentTime = 0.0f;							// Framerate
auto deltaTime = 0.0f;								// time passed
auto lastTime = 0.0f;								// Used to calculate Frame rate

Pipeline pipeline;									// Add one pipeline plus some shaders.									// Add one content loader (+drawing).

//OLD CODE BEFORE I ADDED ARRAY LIST FOR OBJECTS
//renderObject raft;										
//renderObject rocks;										
//renderObject lifebuoy;									
//renderObject campFireSeats;
//renderObject campFire;
//renderObject trees;
//renderObject beachFull;

renderObject renderObjectsList[7]; //creating an array for render objects from blender project

unsigned int selectedObjIdx = 0; //decalring an index for the array

Debugger debugger;									// Add one debugger to use for callbacks ( Win64 - openGLDebugCallback() ) or manual calls ( Apple - glCheckError() ) 

vec3 modelPosition;									// Model position
vec3 modelRotation;									// Model rotation

//adding in variables for lighting equation 
vec3 ia = vec3(1.0f, 1.0f, 1.0f);			// Ambient colour
float ka = 1.0f;									// Ambient constant
vec3 id = vec3(0.9f, 0.9f, 0.9f);			// diffuse colour
float kd = 1.0f;									// Diffuse constant
vec3 is = vec3(0.6f, 0.8f, 0.6f);			// specular colour
float ks = 1.0f;									// specular constant
float shininess;							// shininess constant

vec3 lightPos = vec3(-3.0f, 4.0f, 6.0f);	//setting light position


int main()
{
	cout << endl << "===" << endl << "3D Graphics and Animation - Running..." << endl;

	if (!glfwInit()) // Check if GLFW is working, if not then stop all
	{
		cout << "Could not initialise GLFW...";
		return -1;
	} 

	glfwSetErrorCallback(errorCallbackGLFW); // Setup a function callback to catch and display all GLFW errors.
	hintsGLFW();							 // Setup glfw with various hints.

	const GLFWvidmode *mode = glfwGetVideoMode(glfwGetPrimaryMonitor()); // Grab reference to monitor
	// windowWidth = mode->width; windowHeight = mode->height; //fullscreen
	// window = glfwCreateWindow(windowWidth, windowHeight, title.c_str(), glfwGetPrimaryMonitor(), NULL); // fullscreen

	// Create our Window
	const auto windowTitle = "Stranded Interactive Version - Brendan Reid (openGL) "s;
	window = glfwCreateWindow(windowWidth, windowHeight, windowTitle.c_str(), NULL, NULL);
	if (!window) // Test if Window or OpenGL context creation failed
	{
		cout << "Could not initialise GLFW...";
		glfwTerminate();
		return -1;
	} 

	glfwSetWindowPos(window, 10, 10); // Place it in top corner for easy debugging.
	glfwMakeContextCurrent(window);	  // making the OpenGL context current

	// GLAD: Load OpenGL function pointers - aka update specs to newest versions - plus test for errors.
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD...";
		glfwMakeContextCurrent(NULL);
		glfwTerminate();
		return -1;
	}

	glfwSetWindowSizeCallback(window, onResizeCallback);	   // Set callback for resize
	glfwSetKeyCallback(window, onKeyCallback);				   // Set Callback for keys
	glfwSetMouseButtonCallback(window, onMouseButtonCallback); // Set callback for mouse click
	glfwSetCursorPosCallback(window, onMouseMoveCallback);	   // Set callback for mouse move
	glfwSetScrollCallback(window, onMouseWheelCallback);	   // Set callback for mouse wheel.
	// glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);	// Set mouse cursor Fullscreen
	// glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);	// Set mouse cursor FPS fullscreen.

	// Setup Dear ImGui and add context	-	https://blog.conan.io/2019/06/26/An-introduction-to-the-Dear-ImGui-library.html
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO &io = ImGui::GetIO(); //(void)io;
								  // io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	// io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
	ImGui::StyleColorsLight(); // ImGui::StyleColorsDark(); 		// Setup Dear ImGui style

	// Setup Platform/Renderer ImGui backends
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	const auto glsl_version = "#version 410";
	ImGui_ImplOpenGL3_Init(glsl_version);

	#if defined(__WIN64__)
		debugGL(); // Setup callback to catch openGL errors.	V4.2 or newer
	#elif(__APPLE__)
		glCheckError(); // Old way of checking for errors. Newest not implemented by Apple. Manually call function anywhere in code to check for errors.
	#endif

	glfwSwapInterval(1);			 // Ony render when synced (V SYNC) - https://www.tomsguide.com/features/what-is-vsync-and-should-you-turn-it-on-or-off
	glfwWindowHint(GLFW_SAMPLES, 2); // Multisampling - covered in lectures - https://www.khronos.org/opengl/wiki/Multisampling

	startup(); // Setup all necessary information for startup (aka. load texture, shaders, models, etc).

	cout << endl << "Starting main loop and rendering..." << endl;	

	do{											 // run until the window is closed
		auto currentTime = (float)glfwGetTime(); // retrieve timelapse
		deltaTime = currentTime - lastTime;		 // Calculate delta time
		lastTime = currentTime;					 // Save for next frame calculations.

		glfwPollEvents(); 						// poll callbacks

		update(); 								// update (physics, animation, structures, etc)
		render(); 								// call render function.
		ui();									// call function to render ui.

		#if defined(__APPLE__)
			glCheckError();				// Manually checking for errors for MacOS, Windows has a callback.
		#endif

		glfwSwapBuffers(window); 		// swap buffers (avoid flickering and tearing)

		running &= (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_RELEASE); // exit if escape key pressed
		running &= (glfwWindowShouldClose(window) != GL_TRUE);
	} while (running);

	endProgram(); // Close and clean everything up...

	// cout << "\nPress any key to continue...\n";
	// cin.ignore(); cin.get(); // delay closing console to read debugging errors.

	return 0;
}


void hintsGLFW(){
	
	auto majorVersion = 3; auto minorVersion = 3; // define OpenGL version - at least 3.3 for bare basic NEW OpenGL

	#if defined(__WIN64__)	
		majorVersion = 4; minorVersion = 5;					// Recommended for Windows 4.5, but latest is 4.6 (not all drivers support 4.6 version).
		glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE); // Create context in debug mode - for debug message callback
	#elif(__APPLE__)
		majorVersion = 4; minorVersion = 1; 				// Max support for OpenGL in MacOS
	#endif

	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); 	// https://www.glfw.org/docs/3.3/window_guide.html
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, majorVersion); 
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, minorVersion);
}

void endProgram()
{
	// Clean ImGui
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	glfwMakeContextCurrent(NULL); 	// destroys window handler
	glfwTerminate();				// destroys all windows and releases resources.
}

void startup()
{
	// Output some debugging information
	cout << "VENDOR: " << (char *)glGetString(GL_VENDOR) << endl;
	cout << "VERSION: " << (char *)glGetString(GL_VERSION) << endl;
	cout << "RENDERER: " << (char *)glGetString(GL_RENDERER) << endl;	

	cout << endl << "Loading content..." << endl;
	
	//Loading each exported GLTF object from blender project
	renderObjectsList[0].c.LoadGLTF("assets/raft.gltf");						
	renderObjectsList[1].c.LoadGLTF("assets/rocks.gltf");
	renderObjectsList[2].c.LoadGLTF("assets/lifebuoy.gltf");
	renderObjectsList[3].c.LoadGLTF("assets/campfireseat.gltf");
	renderObjectsList[4].c.LoadGLTF("assets/campfire.gltf");
	renderObjectsList[5].c.LoadGLTF("assets/trees.gltf");
	renderObjectsList[6].c.LoadGLTF("assets/beach.gltf");

	pipeline.CreatePipeline();
	pipeline.LoadShaders("shaders/vs_model.glsl", "shaders/fs_model.glsl");

	//Start position, rotation and scale for raft
	renderObjectsList[0].modPosition = glm::vec3(0.0f, 0.0f, 0.0f);
	renderObjectsList[0].modRotation = glm::vec3(0.0f, 0.0f, 0.0f); 
	renderObjectsList[0].scale = glm::vec3(1.0f, 1.0f, 1.0f);

	//Start position, rotation and scale for rocks
	renderObjectsList[1].modPosition = glm::vec3(0.0f, 0.0f, 0.0f);
	renderObjectsList[1].modRotation = glm::vec3(0.0f, 0.0f, 0.0f);
	renderObjectsList[1].scale = glm::vec3(1.0f, 1.0f, 1.0f);

	//Start position, rotation and scale for lifebuoy
	renderObjectsList[2].modPosition = glm::vec3(0.0f, 0.0f, 0.0f);
	renderObjectsList[2].modRotation = glm::vec3(0.0f, 0.0f, 0.0f);
	renderObjectsList[2].scale = glm::vec3(1.0f, 1.0f, 1.0f);

	//Start position, rotation and scale for fire seats 
	renderObjectsList[3].modPosition = glm::vec3(0.0f, 0.0f, 0.0f);
	renderObjectsList[3].modRotation = glm::vec3(0.0f, 0.0f, 0.0f);
	renderObjectsList[3].scale = glm::vec3(1.0f, 1.0f, 1.0f);

	//Start position, rotation and scale for fire
	renderObjectsList[4].modPosition = glm::vec3(0.0f, 0.0f, 0.0f);
	renderObjectsList[4].modRotation = glm::vec3(0.0f, 0.0f, 0.0f);
	renderObjectsList[4].scale = glm::vec3(1.0f, 1.0f, 1.0f);

	//Start position, rotation and scale for trees
	renderObjectsList[5].modPosition = glm::vec3(0.0f, 0.0f, 0.0f);
	renderObjectsList[5].modRotation = glm::vec3(0.0f, 0.0f, 0.0f);
	renderObjectsList[5].scale = glm::vec3(0.05f, 0.02f, 0.01f);

	//Start position, rotation and scale for beach
	renderObjectsList[6].modPosition = glm::vec3(0.0f, 0.0f, 0.0f);
	renderObjectsList[6].modRotation = glm::vec3(0.0f, 0.0f, 0.0f);
	renderObjectsList[6].scale = glm::vec3(3.0f, 1.0f, 10.0f);

	// A few optimizations.
	glFrontFace(GL_CCW);
	glCullFace(GL_BACK);
	glEnable(GL_CULL_FACE);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	// Get the correct size in pixels - E.g. if retina display or monitor scaling
	glfwGetFramebufferSize(window, &windowWidth, &windowHeight);

	// Calculate proj_matrix for the first time.
	aspect = (float)windowWidth / (float)windowHeight;
	projMatrix = glm::perspective(glm::radians(fovy), aspect, 0.1f, 1000.0f);
}

void update()
{
	/* OLD CODE TO MOVE MODEL, UPDATED (BELOW) FOR MODEL INTERACTION, LIGHT INTERACTION ETC... 
	FIX TO DEAL WITH OBJECT (INDIVIDUALLY)
	rotation for the model (comes with the template)
	if (keyStatus[GLFW_KEY_LEFT]) modelRotation.y += 0.05f; 
	if (keyStatus[GLFW_KEY_RIGHT]) modelRotation.y -= 0.05f;
	if (keyStatus[GLFW_KEY_UP]) modelRotation.x += 0.05f;
	if (keyStatus[GLFW_KEY_DOWN]) modelRotation.x -= 0.05f;
	if (keyStatus[GLFW_KEY_Z]) modelPosition.z += 0.05f;	//updated from W to Z key by Brendan also changed from 0.10 to 0.05 to keep things relative
	if (keyStatus[GLFW_KEY_X]) modelPosition.z -= 0.05f;	//updated from S to X key by Brendan (0.10 -> 0.05 here too)
	*/

	//selectable models using tab
	if (keyStatus[GLFW_KEY_TAB]) {
		selectedObjIdx = ++selectedObjIdx % 7;
	}

	//use these keys to interact with each model (when selected using tab) and manipulate their rotation and position
	if (keyStatus[GLFW_KEY_LEFT]) {
		renderObjectsList[selectedObjIdx].modRotation.y += 0.05f;		//++ROTATION OF Y
	}
	if (keyStatus[GLFW_KEY_RIGHT]) {
		renderObjectsList[selectedObjIdx].modRotation.y -= 0.05f;		//--ROTATION OF Y
	}
	if (keyStatus[GLFW_KEY_UP]) {
		renderObjectsList[selectedObjIdx].modRotation.x += 0.05f;		//++ROTATION OF X
	}
	if (keyStatus[GLFW_KEY_DOWN]) {
		renderObjectsList[selectedObjIdx].modRotation.x -= 0.05f;		//--ROTATION OF X
	}
	if (keyStatus[GLFW_KEY_Z]) {
		renderObjectsList[selectedObjIdx].modPosition.x += 0.05f;		//++POSITION OF X
	}
	if (keyStatus[GLFW_KEY_X]) {
		renderObjectsList[selectedObjIdx].modPosition.x -= 0.05f;		//--POSITION OF X
	}
	if (keyStatus[GLFW_KEY_N]) {
		renderObjectsList[selectedObjIdx].modPosition.y += 0.05f;		//++POSITION OF Y
	}
	if (keyStatus[GLFW_KEY_M]) {
		renderObjectsList[selectedObjIdx].modPosition.y -= 0.05f;		//--POSITION OF X
	}

	/*
	* Obviously you could do things here like manipulate the scale of each object as well etc... 
	* I just didn't think it was necessary to add here as rather obvious! Have done start scales in the startup func however and is possible to scale each object
	* accordingly there. In theory, my model should scale to what my blender project does, however there is clear issues with the export process!
	*/

	//RELOAD SHADERS USING R KEY
	if (keyStatus[GLFW_KEY_R]) pipeline.ReloadShaders();

	//interactive camera (rotation) 
	//makes the camera moveable
	if (keyStatus[GLFW_KEY_A]) {			//MOVE CAM LEFT
		cameraPosition.x -= 0.05f;
	}
	if (keyStatus[GLFW_KEY_D]) {			//MOVE CAM RIGHT
		cameraPosition.x += 0.05f;
	}
	if (keyStatus[GLFW_KEY_W]) {			//MOVE CAM FORWARD
		cameraPosition.y += 0.05f;
	}
	if (keyStatus[GLFW_KEY_S]) {			//MOVE CAM OUTWARD 
		cameraPosition.y -= 0.05f;
	}
	if (keyStatus[GLFW_KEY_O]) {			//ZOOM OUT
		cameraPosition.z += 0.10f;
	}
	if (keyStatus[GLFW_KEY_P]) {			//ZOOM IN
		cameraPosition.z -= 0.10f;
	}

	//interactive keys for the lights (affect phong's lighting)
	//add or decrease the poistion on x and y of the lights
	if (keyStatus[GLFW_KEY_U]) {
		lightPos.y += 0.05f;
	}
	if (keyStatus[GLFW_KEY_J]) {
		lightPos.y += 0.05f;
	}
	if (keyStatus[GLFW_KEY_H]) {
		lightPos.x += 0.05f;
	}
	if (keyStatus[GLFW_KEY_L]) {
		lightPos.x += 0.05f;
	}

	//interactive keys for light strength (altering ambient,diffuse and specular lighting strengths)
	if (keyStatus[GLFW_KEY_0]) {			//ambient +
		ka += 0.05f;
	}
	if (keyStatus[GLFW_KEY_1]) {			//ambient -
		ka -= 0.05f;
	}
	if (keyStatus[GLFW_KEY_2]) {			//diffuse +
		kd += 0.05f;
	}
	if (keyStatus[GLFW_KEY_3]) {			//diffuse -
		kd -= 0.05f;
	}
	if (keyStatus[GLFW_KEY_4]) {			//specular +
		ks += 0.05f;
	}
	if (keyStatus[GLFW_KEY_5]) {			//specular -
		ks -= 0.05f;
	}
	// Start the Dear ImGui frame
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
}

void render()
{
	glViewport(0, 0, windowWidth, windowHeight);

	// Clear colour buffer
	glm::vec4 inchyraBlue = glm::vec4(0.345f, 0.404f, 0.408f, 1.0f);
	glm::vec4 backgroundColor = inchyraBlue;
	glClearBufferfv(GL_COLOR, 0, &backgroundColor[0]);

	// Clear deep buffer
	static const GLfloat one = 1.0f;
	glClearBufferfv(GL_DEPTH, 0, &one);

	// Enable blend
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Use our shader programs
	glUseProgram(pipeline.pipe.program);

	// Setup camera
	glm::mat4 viewMatrix = glm::lookAt(cameraPosition,				 // eye
									   cameraPosition + cameraFront, // centre
									   cameraUp);					 // up

	glUniformMatrix4fv(glGetUniformLocation(pipeline.pipe.program, "view_matrix"), 1, GL_FALSE, &viewMatrix[0][0]);	//sets these uniforms for shaders, refactoring here as repeated code in template
	glUniformMatrix4fv(glGetUniformLocation(pipeline.pipe.program, "proj_matrix"), 1, GL_FALSE, &projMatrix[0][0]);

	//adding a pipeline for lighting (rendering before draw) using phong's lighting from lab
	glUniform4f(glGetUniformLocation(pipeline.pipe.program, "viewPosition"), cameraPosition.x, cameraPosition.y, cameraPosition.z, 1.0);
	glUniform4f(glGetUniformLocation(pipeline.pipe.program, "lightPosition"), lightPos.x, lightPos.y, lightPos.z, 1.0);
	glUniform4f(glGetUniformLocation(pipeline.pipe.program, "ia"), ia.r, ia.g, ia.b, 1.0);
	glUniform1f(glGetUniformLocation(pipeline.pipe.program, "ka"), ka);
	glUniform4f(glGetUniformLocation(pipeline.pipe.program, "id"), id.r, id.g, id.b, 1.0);
	glUniform1f(glGetUniformLocation(pipeline.pipe.program, "kd"), 1.0f);
	glUniform4f(glGetUniformLocation(pipeline.pipe.program, "is"), is.r, is.g, is.b, 1.0);
	glUniform1f(glGetUniformLocation(pipeline.pipe.program, "ks"), 1.0f);
	glUniform1f(glGetUniformLocation(pipeline.pipe.program, "shininess"), 32.0f);

	//creating a loop to render through each model in objects array (raft, rocks, ... , n)
	for (int i = 0; i < 7; i++) {
		renderObjectsList[i].modelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
		renderObjectsList[i].modelMatrix = glm::rotate(renderObjectsList[i].modelMatrix, renderObjectsList[i].modRotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
		renderObjectsList[i].modelMatrix = glm::rotate(renderObjectsList[i].modelMatrix, renderObjectsList[i].modRotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
		renderObjectsList[i].modelMatrix = glm::rotate(renderObjectsList[i].modelMatrix, renderObjectsList[i].modRotation.z, glm::vec3(0.0f, 0.0f, 1.0f));
		renderObjectsList[i].modelMatrix = glm::scale (renderObjectsList[i].modelMatrix, glm::vec3(renderObjectsList[i].scale.x, renderObjectsList[i].scale.y, renderObjectsList[i].scale.z));

		glUniformMatrix4fv(glGetUniformLocation(pipeline.pipe.program, "model_matrix"), 1, GL_FALSE, &renderObjectsList[i].modelMatrix[0][0]);

		renderObjectsList[i].c.DrawModel(renderObjectsList[i].c.vaoAndEbos, renderObjectsList[i].c.model);
	}

	/* OLD CODE FOR MODEL MATRICES PER OBJECT UPDATED 06/12/22 TO BE AN ARRAY LIST (ABOVE) SAVES COMPUTATION TIME
	creating a model matrix for raft obj
	raft.modelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
	raft.modelMatrix = glm::rotate(raft.modelMatrix, raft.modRotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
	raft.modelMatrix = glm::rotate(raft.modelMatrix, raft.modRotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
	raft.modelMatrix = glm::rotate(raft.modelMatrix, raft.modRotation.z, glm::vec3(0.0f, 0.0f, 1.0f));
	raft.modelMatrix = glm::scale(raft.modelMatrix, glm::vec3(1.2f, 1.2f, 1.2f));

	glUniformMatrix4fv(glGetUniformLocation(pipeline.pipe.program, "model_matrix"), 1, GL_FALSE, &raft.modelMatrix[0][0]);
	

	Drawing loaded raft model from blender project
	raft.c.DrawModel(raft.c.vaoAndEbos, raft.c.model);

	creating a model matrix for rocks obj
	rocks.modelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
	rocks.modelMatrix = glm::rotate(rocks.modelMatrix, rocks.modRotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
	rocks.modelMatrix = glm::rotate(rocks.modelMatrix, rocks.modRotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
	rocks.modelMatrix = glm::rotate(rocks.modelMatrix, rocks.modRotation.z, glm::vec3(0.0f, 0.0f, 1.0f));
	rocks.modelMatrix = glm::scale(rocks.modelMatrix, glm::vec3(1.2f, 1.2f, 1.2f));

	glUniformMatrix4fv(glGetUniformLocation(pipeline.pipe.program, "model_matrix"), 1, GL_FALSE, &rocks.modelMatrix[0][0]);


	Drawing loaded raft model from blender project
	rocks.c.DrawModel(rocks.c.vaoAndEbos, rocks.c.model);

	creating a model matrix for lifebuoy model
	lifebuoy.modelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
	lifebuoy.modelMatrix = glm::rotate(lifebuoy.modelMatrix, lifebuoy.modRotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
	lifebuoy.modelMatrix = glm::rotate(lifebuoy.modelMatrix, lifebuoy.modRotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
	lifebuoy.modelMatrix = glm::rotate(lifebuoy.modelMatrix, lifebuoy.modRotation.z, glm::vec3(0.0f, 0.0f, 1.0f));
	lifebuoy.modelMatrix = glm::scale(lifebuoy.modelMatrix, glm::vec3(1.2f, 1.2f, 1.2f));

	glUniformMatrix4fv(glGetUniformLocation(pipeline.pipe.program, "model_matrix"), 1, GL_FALSE, &lifebuoy.modelMatrix[0][0]);

	Drawing lifebuoy model from blender project			
	lifebuoy.c.DrawModel(lifebuoy.c.vaoAndEbos, lifebuoy.c.model);

	creating a model matrix for camp fire seats model
	campFireSeats.modelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
	campFireSeats.modelMatrix = glm::rotate(campFireSeats.modelMatrix, campFireSeats.modRotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
	campFireSeats.modelMatrix = glm::rotate(campFireSeats.modelMatrix, campFireSeats.modRotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
	campFireSeats.modelMatrix = glm::rotate(campFireSeats.modelMatrix, campFireSeats.modRotation.z, glm::vec3(0.0f, 0.0f, 1.0f));
	campFireSeats.modelMatrix = glm::scale(campFireSeats.modelMatrix, glm::vec3(1.2f, 1.2f, 1.2f));

	glUniformMatrix4fv(glGetUniformLocation(pipeline.pipe.program, "model_matrix"), 1, GL_FALSE, &campFireSeats.modelMatrix[0][0]);

	Drawing camp fire seats model			
	campFireSeats.c.DrawModel(campFireSeats.c.vaoAndEbos, campFireSeats.c.model);

	creating a model matrix for camp fire model
	campFire.modelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
	campFire.modelMatrix = glm::rotate(campFire.modelMatrix, campFire.modRotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
	campFire.modelMatrix = glm::rotate(campFire.modelMatrix, campFire.modRotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
	campFire.modelMatrix = glm::rotate(campFire.modelMatrix, campFire.modRotation.z, glm::vec3(0.0f, 0.0f, 1.0f));
	campFire.modelMatrix = glm::scale(campFire.modelMatrix, glm::vec3(1.2f, 1.2f, 1.2f));

	glUniformMatrix4fv(glGetUniformLocation(pipeline.pipe.program, "model_matrix"), 1, GL_FALSE, &campFire.modelMatrix[0][0]);

	Drawing camp fire seats model 			
	campFire.c.DrawModel(campFire.c.vaoAndEbos, campFire.c.model);

	creating a model matrix for trees model
	trees.modelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
	trees.modelMatrix = glm::rotate(trees.modelMatrix, trees.modRotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
	trees.modelMatrix = glm::rotate(trees.modelMatrix, trees.modRotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
	trees.modelMatrix = glm::rotate(trees.modelMatrix, trees.modRotation.z, glm::vec3(0.0f, 0.0f, 1.0f));
	trees.modelMatrix = glm::scale(trees.modelMatrix, glm::vec3(0.05f, 0.05f, 0.05f));;

	glUniformMatrix4fv(glGetUniformLocation(pipeline.pipe.program, "model_matrix"), 1, GL_FALSE, &trees.modelMatrix[0][0]);

	Drawing camp fire seats model from blender project			
	trees.c.DrawModel(trees.c.vaoAndEbos, trees.c.model);

	//////creating a model matrix for trees model
	sand.modelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
	sand.modelMatrix = glm::rotate(sand.modelMatrix, sand.modRotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
	sand.modelMatrix = glm::rotate(sand.modelMatrix, sand.modRotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
	sand.modelMatrix = glm::rotate(sand.modelMatrix, sand.modRotation.z, glm::vec3(0.0f, 0.0f, 1.0f));
	sand.modelMatrix = glm::scale(sand.modelMatrix, glm::vec3(1.0f, 1.0f, 1.0f));

	glUniformMatrix4fv(glGetUniformLocation(pipeline.pipe.program, "model_matrix"), 1, GL_FALSE, &sand.modelMatrix[0][0]);

	creating a model matrix for trees model
	beachFull.modelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
	beachFull.modelMatrix = glm::rotate(beachFull.modelMatrix, beachFull.modRotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
	beachFull.modelMatrix = glm::rotate(beachFull.modelMatrix, beachFull.modRotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
	beachFull.modelMatrix = glm::rotate(beachFull.modelMatrix, beachFull.modRotation.z, glm::vec3(0.0f, 0.0f, 1.0f));
	beachFull.modelMatrix = glm::scale(beachFull.modelMatrix, glm::vec3(3.0f, 1.2f, 10.0f));

	glUniformMatrix4fv(glGetUniformLocation(pipeline.pipe.program, "model_matrix"), 1, GL_FALSE, &beachFull.modelMatrix[0][0]);

	Drawing camp fire seats model from blender project			
	beachFull.c.DrawModel(beachFull.c.vaoAndEbos, beachFull.c.model);
	*/

	#if defined(__APPLE__)
		glCheckError();
	#endif
}

void ui()
{
	ImGuiIO &io = ImGui::GetIO();
	ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDecoration; 
	window_flags |= ImGuiWindowFlags_AlwaysAutoResize;
	window_flags |= ImGuiWindowFlags_NoSavedSettings; 
	window_flags |= ImGuiWindowFlags_NoFocusOnAppearing; 
	window_flags |= ImGuiWindowFlags_NoNav;

	const auto PAD = 10.0f;
	const ImGuiViewport *viewport = ImGui::GetMainViewport();
	ImVec2 work_pos = viewport->WorkPos; // Use work area to avoid menu-bar/task-bar, if any!
	ImVec2 work_size = viewport->WorkSize;
	ImVec2 window_pos, window_pos_pivot;
	window_pos.x = work_pos.x + work_size.x - PAD;
	window_pos.y = work_pos.y + work_size.y - PAD;
	window_pos_pivot.x = 1.0f;
	window_pos_pivot.y = 1.0f;

	ImGui::SetNextWindowPos(window_pos, ImGuiCond_Always, window_pos_pivot);
	window_flags |= ImGuiWindowFlags_NoMove;

	ImGui::SetNextWindowBgAlpha(0.35f); // Transparent background
	bool *p_open = NULL;
	if (ImGui::Begin("Info", nullptr, window_flags)) {
		ImGui::Text("About: 3D Graphics and Animation 2022"); // ImGui::Separator();
		ImGui::Text("Performance: %.3fms/Frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		ImGui::Text("Pipeline: %s", pipeline.pipe.error?"ERROR":"OK");
		ImGui::Text("ka: %.01f", ka);	//adding in text to watch numerical value of ambient colour
		ImGui::Text("kd: %.01f", kd);	//adding in text to watch numerical value of diffuse colour
		ImGui::Text("ks: %.01f", ks);	//adding in text to watch numerical value of specular colour
	}
	ImGui::End();

	// Rendering imgui
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void onResizeCallback(GLFWwindow *window, int w, int h)
{
	windowWidth = w;
	windowHeight = h;

	// Get the correct size in pixels
	glfwGetFramebufferSize(window, &windowWidth, &windowHeight);

	if (windowWidth > 0 && windowHeight > 0)
	{ // Avoid issues when minimising window - it gives size of 0 which fails division.
		aspect = (float)w / (float)h;
		projMatrix = glm::perspective(glm::radians(fovy), aspect, 0.1f, 1000.0f);
	}
}

void onKeyCallback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
	if (action == GLFW_PRESS)
		keyStatus[key] = true;
	else if (action == GLFW_RELEASE)
		keyStatus[key] = false;

	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GLFW_TRUE);
}

void onMouseButtonCallback(GLFWwindow *window, int button, int action, int mods)
{
}

void onMouseMoveCallback(GLFWwindow *window, double x, double y)
{
	int mouseX = static_cast<int>(x);
	int mouseY = static_cast<int>(y);
}

void onMouseWheelCallback(GLFWwindow *window, double xoffset, double yoffset)
{
	int yoffsetInt = static_cast<int>(yoffset);

	fovy -= (float)yoffset;

	if (fovy < 1.0f) {				//updated this function to be standard to openGL sources provided with class work (https://learnopengl.com/Getting-Started/Camera)
		fovy = 1.0f;
	}
	if (fovy > 45.0f) {
		45.0f;
	}
}

void APIENTRY openGLDebugCallback(GLenum source,
								  GLenum type,
								  GLuint id,
								  GLenum severity,
								  GLsizei length,
								  const GLchar *message,
								  const GLvoid *userParam)  // Debugger callback for Win64 - OpenGL versions 4.2 or better.
{
	debugger.OpenGLDebugCallback(source, type, id, severity, length, message, userParam);
}

void debugGL() // Debugger function for Win64 - OpenGL versions 4.2 or better.
{
	// Enable Opengl Debug
	glEnable(GL_DEBUG_OUTPUT);
	glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
	glDebugMessageCallback((GLDEBUGPROC)openGLDebugCallback, nullptr);
	glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, NULL, true);
}

GLenum glCheckError_(const char *file, int line) // Debugger manual function for Apple - OpenGL versions 3.3 to 4.1.
{
	GLenum errorCode;
	while ((errorCode = glGetError()) != GL_NO_ERROR) debugger.GlGetError(errorCode, file, line);

	return errorCode;
}

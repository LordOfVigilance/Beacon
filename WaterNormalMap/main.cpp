#include "GL/GLEW.h"
#define GLFW_DLL
#include "GLFW/glfw3.h"
#include <stdio.h>
#include <string>
#include <fstream>

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include <vector>


#include <iostream>

#include "DreamButton.h"
#include "DreamSlider.h"
#include "DreamClickable.h"
#include "DreamContainer.h"

#include "Model.h"
#include "Player.h"
#include "Collectible.h"
#include "Sound.h"

struct {
	GLint mvp;
	GLint dmapDepth;
	GLuint texDepthMap;
	GLint texOffset;
	GLint sinValue;
	GLint lightPower;
	GLint lightPos;
	GLint lightColor;
	GLint eyePos;
	GLint lightPosCorrect;
	GLint offset;
	GLint reflectionColor;
	GLint specularColor;
	GLint diffuseColor;
	GLint var1;
	GLint var2;
	GLint var3;
	GLint var4;
	GLint var5;
	GLint var6;
} uniforms;

struct ImagePBM {
	GLint width;
	GLint height;
	GLint elements;
	GLubyte* data;
};

struct {
	glm::vec2 rotation;
	glm::vec3 translation;
} camera;

const float ROTATIONSPEED = 0.015f;
const float MOVESPEED = 2.0f;

void checkForHover(double, double);

std::vector<DreamClickable*> buttonList;

glm::vec2 texOffset(1.5, 0.0);

GLFWwindow* openGLInit(GLint, GLint, GLchar*);
void keyCallback (GLFWwindow*, int, int, int, int);
void mouseMoveCallback (GLFWwindow*, double, double);
void mouseButtonCallback (GLFWwindow*, int, int, int);
GLuint createProgram (GLchar *, GLchar *, GLchar *, GLchar *, GLchar *);
void shaderFromFileAndLink (GLenum, GLchar *, GLuint);
std::string readFile (GLchar *);
GLuint createShaderFromCode (GLenum, std::string);
ImagePBM readNetpbmFile(GLchar *);
GLuint createRTexture(GLchar *);
GLuint createRTextureClamped(GLchar *);
GLuint createRGBTexture(GLchar *);
GLuint createRGBTextureMipMapped(GLchar *);
void renderSplashScreen(GLuint, GLuint, GLFWwindow*);
void computeMatricesFromInputs(GLFWwindow * window, glm::mat4 * cameraView, Player* player);

void printFunction();
bool locked = true;
bool lockLock = false;
bool muteLock = false;
Sound sounds = Sound();
glm::vec3 currentDirection = glm::vec3(0.0f,0.0f,0.0f);
const float PI = 3.14159265f;
bool terminated = false;
DreamClickable* buttonPressed;
bool mousePressed = false;
DreamContainer* guiContainer;
bool drawButtons = true;

int main(void) {

	GLFWwindow* window = openGLInit(1280, 720, "Water Normal Mapped");

	GLuint texProgram = createProgram("tex.vert", NULL, NULL, NULL, "tex.frag");
	GLuint splashScreenTexture = createRGBTexture("textures/splashscreen.pbm");
	renderSplashScreen(texProgram, splashScreenTexture, window);
	
	GLuint program = createProgram("waterNM.vert", "waterNM.geom", "waterNM.tcs", "waterNM.tes", "waterNM.frag");
	uniforms.dmapDepth = glGetUniformLocation(program, "dmap_depth");
	uniforms.mvp = glGetUniformLocation(program, "mvp");
	uniforms.offset = glGetUniformLocation(program, "offset");
	uniforms.texDepthMap = glGetUniformLocation(program, "texDisplacement");
	uniforms.texOffset = glGetUniformLocation(program, "texOffset");
	uniforms.sinValue = glGetUniformLocation(program, "sinValue");
	uniforms.lightPower = glGetUniformLocation(program, "lightPower");
	uniforms.lightColor = glGetUniformLocation(program, "lightColor");
	uniforms.lightPos = glGetUniformLocation(program, "lightPos");
	uniforms.eyePos = glGetUniformLocation(program, "eyePos");
	uniforms.lightPosCorrect = glGetUniformLocation(program, "lightPosCorrect");
	
	uniforms.reflectionColor = glGetUniformLocation(program, "reflectionColor");
	uniforms.diffuseColor = glGetUniformLocation(program, "diffuseColor");
	uniforms.specularColor = glGetUniformLocation(program, "specularColor");
	uniforms.var1 = glGetUniformLocation(program, "var1");
	uniforms.var2 = glGetUniformLocation(program, "var2");
	uniforms.var3 = glGetUniformLocation(program, "var3");
	uniforms.var4 = glGetUniformLocation(program, "var4");
	uniforms.var5 = glGetUniformLocation(program, "var5");
	uniforms.var6 = glGetUniformLocation(program, "var6");

	GLuint simpleProgram = createProgram("simplePass.vert", NULL, NULL, NULL, "simplePass.frag");
	GLuint rainProgram = createProgram("rain.vert", "rain.geom", NULL, NULL, "rain.frag");
	GLuint causticsProgram = createProgram("plyColor.vert", "plyColor.geom", NULL, NULL, "causticsPly.frag");
	GLuint plyColorProgram = createProgram("plyColor.vert", "plyColor.geom", NULL, NULL, "plyColor.frag");
	GLuint plyUVProgram = createProgram("plyUV.vert", "plyUV.geom", NULL, NULL, "plyUV.frag");
	GLuint waveProgram = createProgram("wave.vert", "wave.geom", NULL, NULL, "wave.frag");
	GLuint buttonProgram = createProgram("button.vert", NULL, NULL, NULL, "button.frag");
	GLuint shadowProgram = createProgram("shadowDepth.vert", NULL, NULL, NULL, "shadowDepth.frag");
	GLuint reflectionProgram = createProgram("reflection.vert", NULL, NULL, NULL, "reflection.frag");


	sounds.loadSounds(1);

	int width;
	int height;
	glfwGetWindowSize(window, &width, &height);


	glfwSetCursorPos(window, width / 2, height / 2);


	glPatchParameteri(GL_PATCH_VERTICES, 4);
	glPointSize(16);

	//FrameBuffer One
	GLuint frameBufferDownSampled;
	glGenFramebuffers(1, &frameBufferDownSampled);
	glBindFramebuffer(GL_FRAMEBUFFER, frameBufferDownSampled);

	GLuint FBcolorDownSampled;
	glGenTextures(1, &FBcolorDownSampled);
	glBindTexture(GL_TEXTURE_2D, FBcolorDownSampled);
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGB8, 1280, 720);
	
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	float borderColor[] = { 1.0f, 0.0f, 1.0f, 1.0f };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, FBcolorDownSampled, 0);

	static const GLenum drawBuffersDS[] = {GL_COLOR_ATTACHMENT0};
	glDrawBuffers(1, drawBuffersDS);
	
	if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		return false;

	//Multisampled FrameBuffer
	GLuint frameBufferName;
	glGenFramebuffers(1, &frameBufferName);
	glBindFramebuffer(GL_FRAMEBUFFER, frameBufferName);
	
	glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	glTexParameterfv(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_BORDER_COLOR, &glm::vec4(1.0, 0.0, 1.0, 1.0)[0]);

	GLuint frameBufferColorTextureName;
	glGenTextures(1, &frameBufferColorTextureName);
	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, frameBufferColorTextureName);
	glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, 4, GL_RGB8, 1280, 720, GL_TRUE);
	
	glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	glTexParameterfv(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_BORDER_COLOR, &glm::vec4(1.0, 0.0, 1.0, 1.0)[0]);

	GLuint frameBufferDepthTextureName;
	glGenTextures(1, &frameBufferDepthTextureName);
	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, frameBufferDepthTextureName);
	glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, 4, GL_DEPTH_COMPONENT32F, 1280, 720, GL_TRUE);//GL_DEPTH_COMPONENT16

	glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
	glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, frameBufferColorTextureName, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D_MULTISAMPLE, frameBufferDepthTextureName, 0);

	static const GLenum drawBuffers[] = {GL_COLOR_ATTACHMENT0};
	glDrawBuffers(1, drawBuffers);
	
	if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		return false;
 
	//FrameBuffer Two
	GLuint FBreflection;
	glGenFramebuffers(1, &FBreflection);
	glBindFramebuffer(GL_FRAMEBUFFER, FBreflection);

	GLuint FBreflectionDepth;
	glGenTextures(1, &FBreflectionDepth);
	glBindTexture(GL_TEXTURE_2D, FBreflectionDepth);
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_DEPTH_COMPONENT32, 1280, 720);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, FBreflectionDepth, 0);
	glDrawBuffer(GL_NONE);
	
	if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		return false;

	////////////////////////////
	//Shadow Buffer
	////////////////////////////
	const int DEPTH_MAP_WIDTH = 1024;
	const int DEPTH_MAP_HEIGHT = 1024;

	GLuint depthMapFBO;
	glGenFramebuffers(1, &depthMapFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);

	GLuint depthMap;
	glGenTextures(1, &depthMap);
	glBindTexture(GL_TEXTURE_2D, depthMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, DEPTH_MAP_WIDTH, DEPTH_MAP_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	float* depth = new float(0.0f);
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, depth);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);

	if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		return false;

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	////////////////////////////
	//End of Shadow Framebuffer
	///////////////////////////

	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	GLuint waterTexture = createRTexture("Textures/Water.pbm");
	GLuint waterNormalMap = createRGBTexture("Textures/Water_NRM.pbm");
	GLuint fiberTexture = createRTexture("Textures/colorMap.pbm");
	
	GLuint worldDepthMap = createRTextureClamped("Textures/worldDepthFull.pbm");
	GLuint worldDepthMapFoam = createRTextureClamped("Textures/worldDepthFoam.pbm");
	GLuint waterFoam = createRTexture("Textures/foamDark.pbm");
	GLuint marbleTexture = createRGBTexture("Textures/marble.pbm");
	GLuint waveTexture = createRGBTextureMipMapped("Textures/waveTexture.pbm");

	GLuint waterCaustic[30];
	for(int i=0; i<30; i++) {
		GLchar textureFileName[100];
		sprintf(textureFileName, "Textures/causticsAnimation/lessblur30/CausticsRender_%03d.pbm", i + 1);

		waterCaustic[i] = createRTexture(textureFileName);
	}

	Model land("Models/Land.obj");
	Model lighthouse("Models/Lighthouse.obj");	
	Model monkey("Models/Monkey.obj");
	Model ship("Models/Ship.obj");
	Model room("Models/room_thickwalls2.obj");
	
	Model plyPlane("Models/plane.ply", PLYMALLOC);
	Model plyCube("Models/cube.ply", PLYMALLOC);
	Model plyWave("Models/wave.ply", PLYMALLOC);
	Model plyWorld("Models/WorldColor.ply", PLYMALLOC);
	Model plyMarble("Models/marble2.ply", PLYUVMALLOC);
	
	//Model cube("Models/cube.dae", COLLADAE);
	//Model land("Models/Land.dae", COLLADAE);
	//Model lighthouse("Models/Lighthouse.dae", COLLADAE);
	//Model monkey("Models/Monkey.dae", COLLADAE);
	//Model ship("Models/Ship.dae", COLLADAE);

	camera.rotation = glm::vec2();
	
    Player player = Player(glm::vec3(0, -0.1f, 0), glm::vec2(0.0f, 0.0f), Model("Models/wave.ply", PLYMALLOC), &sounds);
	camera.translation = player.getPosition() + glm::vec3(0.0f, 2.0f, 4.0f);
    
    
	glm::vec4 lightPos(520.0, 120.0, 580.0, 1.0);
	glm::vec3 lightColor(1.0, 1.0, 1.0);
	GLfloat lightPower(1.0);

	glm::vec3 eye(0.0, 2.5, 1.0);
	glm::vec3 center(0.0, 2.5, 0.0);
	glm::vec3 up(0.0, -1.0, 0.0);
	glm::mat4 viewMatrix = glm::lookAt(eye, center, up);

	glm::vec3 eyeReflection(0.0, -2.5, 1.0);
	glm::vec3 centerReflection(0.0, -2.5, 0.0);
	glm::vec3 upReflection(0.0, 1.0, 0.0);
	glm::mat4 reflectionViewMatrix = glm::scale(glm::lookAt(eyeReflection, centerReflection, upReflection), glm::vec3(-1.0, 1.0, 1.0));
	
	glm::mat4 perspectiveMatrix = glm::perspective(30.0f, 16.0f/9.0f, 0.1f, 3000.0f);

	glm::mat4 modelMatrixLight(1.0f);
	glm::mat4 modelMatrixWater(1.0f);

	land.scale(glm::vec3(17.0, 17.0, 17.0));
	land.translate(glm::vec3(0.0, -0.4, 0.0));

	lighthouse.scale(glm::vec3(1.0, 1.0, 1.0));
	lighthouse.translate(glm::vec3(0.0, 0.0, 0.0));

	ship.translate(glm::vec3(70.0, 0.0, 25.0));
	ship.rotate(90.0f, glm::vec3(0.0, 1.0, 0.0));
	
	room.scale(glm::vec3(1.0, 1.0, 1.0));
	room.translate(glm::vec3(0.0, 0.0, 0.0));

	plyMarble.translate(glm::vec3(0.0f, 1.0f, -10.0f));

	//cube.translate(glm::vec3(0.0, 1.0, 0.0));

	glm::mat4 mvpWater = perspectiveMatrix * viewMatrix * modelMatrixWater;
	glm::mat4 mvpLight = perspectiveMatrix * viewMatrix * modelMatrixLight;	
	
	glm::vec3 reflectionColor(0.22f, 0.3f, 0.36f);
	glm::vec3 specularColor(0.68f, 0.33f, 0.11f);
	glm::vec3 diffuseColor(0.25f, 0.32f, 0.43f);
	glm::vec3 skyColor(0.78f, 0.91f, 1.0f);

	GLfloat var1 = 1.4f;
	GLfloat var2 = 2.4f;
	GLfloat var3 = 675.0f;
	GLfloat var4 = 340.0f;
	GLfloat var5 = 580.0f;
	GLfloat var6 = 260.0f;
	GLfloat bias = 0.003f;
	GLfloat shadowPos[3] = {10.0f, 10.0f, 3.6f};
	GLfloat shadowSliderColor[4] = {0.0f, 0.0f, 1.0f, 0.3f};
	GLfloat sphereControls[2] = {1.0f, 1.0f};
	GLfloat sphereControlsSliderColor[4] = {1.0f, 0.0f, 1.0f, 0.3f};
	GLfloat spherePosition[3] = {3.8f, 0.0f, 0.0f};
	GLfloat spherePositionSliderColor[4] = {1.0f, 0.0f, 0.0f, 0.3f};

	GLfloat textureSelect = 0.0;

	GLfloat dmapDepth = -0.4f;

	int windowWidth, windowHeight;
	glfwGetWindowSize(window, &windowWidth, &windowHeight);

	float containerOffset[2] = {0.0f/windowWidth, 200.0f/windowHeight};
	float containerSize[2] = {100.0f/windowWidth, 200.0f/windowHeight};
	float containerColor[4] = {0.0, 0.0, 0.0, 0.5};
	guiContainer = new DreamContainer(containerOffset, containerSize, containerColor);
	guiContainer->addComponent(printFunction);
	guiContainer->addComponent(&reflectionColor[0], 3, 0.0f, 1.0f);
	guiContainer->addComponent(&specularColor[0], 3, 0.0f, 1.0f);
	guiContainer->addComponent(&diffuseColor[0], 3, 0.0f, 1.0f);
	guiContainer->addComponent(&skyColor[0], 3, 0.0f, 1.0f, shadowSliderColor);
	guiContainer->addComponent(&var1, 0.0f, 10.0f);
	guiContainer->addComponent(&var2, 0.0f, 120.0f);
	guiContainer->addComponent(&var3, 0.0f, 675.0f);
	guiContainer->addComponent(&var4, 0.1f, 1000.0f);
	guiContainer->addComponent(&var5, 0.1f, 1000.0f);
	guiContainer->addComponent(&var6, 0.1f, 1000.0f);
	guiContainer->addComponent(&bias, -0.1f, 0.1f);
	guiContainer->addComponent(&shadowPos[0], 3, -30.0f, 30.0f, shadowSliderColor);
	guiContainer->addComponent(&sphereControls[0], 2, -20.0f, 20.0f, sphereControlsSliderColor);
	guiContainer->addComponent(&spherePosition[0], 3, -10.0f, 10.0f, spherePositionSliderColor);
	//guiContainer->addComponent(&modelMatrixLight[3][0], -1000.0f, 1000.0f);
	//guiContainer->addComponent(&modelMatrixLight[3][1], -1000.0f, 1000.0f);
	//guiContainer->addComponent(&modelMatrixLight[3][2], -1000.0f, 1000.0f);
	guiContainer->addComponent(&textureSelect, 0.0f, 3.0f);
	guiContainer->addComponent(&dmapDepth, -10.0f, 10.0f);
	
	float startTime = (float) glfwGetTime();

	glm::vec4 clearColorValue = glm::vec4(skyColor.r, skyColor.g, skyColor.b, 1.0);
	float* clearDepthValue = new float(1.0f);
	int animationFrame = 0;
	float lastFrameTime, time;
	lastFrameTime = time = startTime;

	GLfloat currentTime;
	do {
		currentTime = (GLfloat) glfwGetTime();
	} while(currentTime - startTime < 0.5f);

	while(!glfwWindowShouldClose(window) && !terminated) {
		clearColorValue = glm::vec4(skyColor.r, skyColor.g, skyColor.b, 1.0);
		glfwPollEvents();
        computeMatricesFromInputs(window, &viewMatrix, &player);

		time = (float) glfwGetTime();
		texOffset.x = -time/20.0f;
		texOffset.y = time/40.0f;

		float sinValue = (sin(time)/4.0f + 1.25f);

		/////////////////
		//Camera Controls
		//viewMatrix = glm::rotate(viewMatrix, camera.rotation.x, glm::vec3(0.0, 1.0, 0.0));
		//viewMatrix = glm::translate(viewMatrix, camera.translation);
		reflectionViewMatrix = glm::rotate(reflectionViewMatrix, camera.rotation.x, glm::vec3(0.0, 1.0, 0.0));

		glm::vec3 cameraReflection (camera.translation.x, -camera.translation.y, camera.translation.z);
		reflectionViewMatrix = glm::translate(reflectionViewMatrix, cameraReflection);
		mvpLight = perspectiveMatrix*viewMatrix*modelMatrixLight;
		mvpWater = perspectiveMatrix*viewMatrix*modelMatrixWater;


		//End of Camera Controls
		////////////////

		glm::vec4 mvpLightPos = lightPos*mvpLight;
		glm::vec4 mvpEyePos = glm::vec4(eye, 1.0)*mvpLight;
		glm::vec4 mvpLightPosCorrect = mvpLight*lightPos;
		
		//Draw the depths to the Reflection Buffer
		glBindFramebuffer(GL_FRAMEBUFFER, FBreflection);
		glViewport(0, 0, 1280, 720);
		glClearBufferfv(GL_DEPTH, 0, clearDepthValue);

		glEnable(GL_CLIP_DISTANCE0);
		glUseProgram(reflectionProgram);

		//Land
		land.setVP(perspectiveMatrix*reflectionViewMatrix);
		glUniformMatrix4fv(1, 1, GL_FALSE, &land.getMatrix()[0][0]);
		//land.render();

		//Lighthouse
		lighthouse.setVP(perspectiveMatrix*reflectionViewMatrix);
		glUniformMatrix4fv(1, 1, GL_FALSE, &lighthouse.getMatrix()[0][0]);
		lighthouse.render();
		
		//Monkey
		monkey.setVP(perspectiveMatrix*reflectionViewMatrix);
		glUniformMatrix4fv(1, 1, GL_FALSE, &monkey.getMatrix()[0][0]);
		//monkey.render();
		
		//Ship
		ship.setVP(perspectiveMatrix*reflectionViewMatrix);
		glUniformMatrix4fv(1, 1, GL_FALSE, &ship.getMatrix()[0][0]);
		ship.render();

		//Room
		room.setVP(perspectiveMatrix*reflectionViewMatrix);
		glUniformMatrix4fv(1, 1, GL_FALSE, &room.getMatrix()[0][0]);
		//room.render();
		
		//PLY World
		plyWorld.setVP(perspectiveMatrix*reflectionViewMatrix);
		glUniformMatrix4fv(1, 1, GL_FALSE, &plyWorld.getMatrix()[0][0]);
		plyWorld.renderPLY();

		glDisable(GL_CLIP_DISTANCE0);

		///////////////////////////
		//Draw to the Shadow Buffer
		///////////////////////////
		glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
		glViewport(0, 0, DEPTH_MAP_WIDTH, DEPTH_MAP_HEIGHT);
		glClearBufferfv(GL_DEPTH, 0, clearDepthValue);
		glUseProgram(shadowProgram);

		glm::vec3 lightInvDir = glm::vec3(lightPos[0], lightPos[1], lightPos[2]);

		//Compute the MVP matrix from the light's point of view
		glm::mat4 depthProjectionMatrix = glm::ortho<float>(-100, 100, -100, 100, -100, 200);
		glm::mat4 depthViewMatrix = glm::lookAt(lightInvDir, glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
		glm::mat4 depthVP = depthProjectionMatrix * depthViewMatrix;

		glm::mat4 depthBiasMatrix(0.5, 0.0, 0.0, 0.0,
								  0.0, 0.5, 0.0, 0.0,
								  0.0, 0.0, 0.5, 0.0,
								  0.5, 0.5, 0.5, 1.0);
		glm::mat4 depthBiasMVP = depthBiasMatrix*depthVP;
		
		//Land
		land.setVP(depthVP);
		//land.render();

		//Lighthouse
		lighthouse.setVP(depthVP);
		lighthouse.render();
		
		//Monkey
		monkey.setVP(depthVP);
		//monkey.render();
		
		//Ship
		ship.setVP(depthVP);
		ship.render();
		
		//Room
		room.setVP(depthVP);
		//room.render();
		
		//PLY World
		plyWorld.setVP(depthVP);
		plyWorld.renderPLY();
		
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		////////////////////////
		//End of Shadow Buffer
		////////////////////////

		/////////////////////////
		//Draw to FrameBuffer
		/////////////////////////

		glBindFramebuffer(GL_FRAMEBUFFER, frameBufferName);

		glViewport(0, 0, 1280, 720);
		glClearBufferfv(GL_COLOR, 0, &clearColorValue[0]);
		glClearBufferfv(GL_DEPTH, 0, clearDepthValue);

		//Set rainProgram
		glUseProgram (rainProgram);
		glUniform3fv(5, 1, &mvpLightPosCorrect[0]);
		glUniform3fv(6, 1, &lightColor[0]);
		glUniform1f(7, lightPower);

		glUniform1f(10, bias);
		
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, depthMap);

		//Cube
		//cube.setVP(perspectiveMatrix*viewMatrix);
		//cube.render();

		//Land
		land.setVP(perspectiveMatrix*viewMatrix);
		depthBiasMVP = depthBiasMatrix*depthProjectionMatrix*depthViewMatrix*land.getMatrix();
		glUniformMatrix4fv(3, 1, GL_FALSE, &depthBiasMVP[0][0]);
		//land.render();

		//Lighthouse
		lighthouse.setVP(perspectiveMatrix*viewMatrix);
		depthBiasMVP = depthBiasMatrix*depthProjectionMatrix*depthViewMatrix*lighthouse.getMatrix();
		glUniformMatrix4fv(3, 1, GL_FALSE, &depthBiasMVP[0][0]);
		//lighthouse.render();
		
		//Monkey
		monkey.setVP(perspectiveMatrix*viewMatrix);
		depthBiasMVP = depthBiasMatrix*depthProjectionMatrix*depthViewMatrix*monkey.getMatrix();
		glUniformMatrix4fv(3, 1, GL_FALSE, &depthBiasMVP[0][0]);
		//monkey.render();
		
		//Ship
		ship.setVP(perspectiveMatrix*viewMatrix);
		depthBiasMVP = depthBiasMatrix*depthProjectionMatrix*depthViewMatrix*ship.getMatrix();
		glUniformMatrix4fv(3, 1, GL_FALSE, &depthBiasMVP[0][0]);
		ship.render();

		//Room
		room.setVP(perspectiveMatrix*viewMatrix);
		depthBiasMVP = depthBiasMatrix*depthProjectionMatrix*depthViewMatrix*room.getMatrix();
		glUniformMatrix4fv(3, 1, GL_FALSE, &depthBiasMVP[0][0]);
		//room.render();

		/////Ply Files

		glUseProgram(plyColorProgram);
		glUniform3fv(5, 1, &lightPos[0]);
		glUniform3fv(6, 1, &lightColor[0]);
		glUniform1f(7, lightPower);
		
		glUniformMatrix4fv(9, 1, GL_FALSE, &viewMatrix[0][0]);
		glUniform1f(10, bias);
		
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, depthMap);
		
		//PLY Cube
		plyCube.setVP(perspectiveMatrix*viewMatrix);
		depthBiasMVP = depthBiasMatrix*depthProjectionMatrix*depthViewMatrix*plyCube.getMatrix();
		glUniformMatrix4fv(3, 1, GL_FALSE, &depthBiasMVP[0][0]);
		glUniformMatrix4fv(8, 1, GL_FALSE, &plyCube.getMatrix()[0][0]);
		//plyCube.renderPLY();
		
		//PLY Plane
		plyPlane.setVP(perspectiveMatrix*viewMatrix);
		depthBiasMVP = depthBiasMatrix*depthProjectionMatrix*depthViewMatrix*plyPlane.getMatrix();
		glUniformMatrix4fv(3, 1, GL_FALSE, &depthBiasMVP[0][0]);
		glUniformMatrix4fv(8, 1, GL_FALSE, &plyPlane.getMatrix()[0][0]);
		//plyPlane.renderPLY();


		////////////Render World

		glUseProgram(causticsProgram);
		glUniform3fv(5, 1, &lightPos[0]);
		glUniform3fv(6, 1, &lightColor[0]);
		glUniform1f(7, lightPower);
		
		glUniformMatrix4fv(9, 1, GL_FALSE, &viewMatrix[0][0]);
		glUniform1f(10, bias);
		glUniform2fv(11, 1, &texOffset[0]);
		glUniform3fv(12, 1, &lightPos[0]);
		
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, depthMap);
		
		if((time - lastFrameTime) > 1.0f/15.0f) {
			animationFrame++;
			lastFrameTime = time;
		}

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, waterCaustic[animationFrame%30]);
		
		//PLY World
		plyWorld.setVP(perspectiveMatrix*viewMatrix);
		depthBiasMVP = depthBiasMatrix*depthProjectionMatrix*depthViewMatrix*plyWorld.getMatrix();
		glUniformMatrix4fv(3, 1, GL_FALSE, &depthBiasMVP[0][0]);
		glUniformMatrix4fv(8, 1, GL_FALSE, &plyWorld.getMatrix()[0][0]);
		plyWorld.renderPLY();



		/////Ply Files

		//Water
		glDepthMask(GL_FALSE);
		glUseProgram(program);

		glBindFramebuffer(GL_READ_FRAMEBUFFER, frameBufferName);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, frameBufferDownSampled);
		glBlitFramebuffer(0, 0, 1280, 720, 0, 0, 1280, 720, GL_COLOR_BUFFER_BIT, GL_NEAREST);
		
		glBindFramebuffer(GL_FRAMEBUFFER, frameBufferName);
		
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, waterTexture);
		
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, FBcolorDownSampled);
		
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, frameBufferDepthTextureName);
		
		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, FBreflectionDepth);
		
		glActiveTexture(GL_TEXTURE4);
		glBindTexture(GL_TEXTURE_2D, waterNormalMap);
		
		glActiveTexture(GL_TEXTURE5);
		glBindTexture(GL_TEXTURE_2D, worldDepthMap);

		glActiveTexture(GL_TEXTURE6);
		glBindTexture(GL_TEXTURE_2D, waterFoam);
		
		glActiveTexture(GL_TEXTURE7);
		glBindTexture(GL_TEXTURE_2D, worldDepthMapFoam);

		glBindVertexArray(vao);

		glUniform1f(uniforms.dmapDepth, dmapDepth);
		glUniform1f(uniforms.sinValue, sinValue);
		glUniformMatrix4fv(uniforms.mvp, 1, GL_FALSE, &mvpWater[0][0]);
		glUniform2fv(uniforms.texOffset, 1, &texOffset[0]);
		
		glUniform1f(uniforms.lightPower, lightPower);
		glUniform3fv(uniforms.lightPos, 1, &mvpLightPos[0]);
		glUniform3fv(uniforms.lightColor, 1, &lightColor[0]);
		
		glUniform3fv(uniforms.lightPos, 1, &mvpEyePos[0]);
		
		glUniform3fv(uniforms.lightPosCorrect, 1, &mvpLightPosCorrect[0]);
		
		glUniform3fv(uniforms.specularColor, 1, &specularColor[0]);
		glUniform3fv(uniforms.reflectionColor, 1, &reflectionColor[0]);
		glUniform3fv(uniforms.diffuseColor, 1, &diffuseColor[0]);

		glUniform1f(uniforms.var1, var1);
		glUniform1f(uniforms.var2, var2);
		glUniform1f(uniforms.var3, var3);
		glUniform1f(uniforms.var4, var4);
		glUniform1f(uniforms.var5, var5);
		glUniform1f(uniforms.var6, var6);

		glDrawArraysInstanced(GL_PATCHES, 0, 4, 128*128);
		glBindTexture(GL_TEXTURE_2D, 0);
		
		glDepthMask(GL_TRUE);
		///End of Water
		

		
		//Render Wave
		glUseProgram(waveProgram);
		glUniform3fv(5, 1, &lightPos[0]);
		glUniform3fv(6, 1, &lightColor[0]);
		glUniform1f(7, lightPower);
		
		glUniformMatrix4fv(9, 1, GL_FALSE, &viewMatrix[0][0]);
		glUniform1f(10, bias);
		glUniform2fv(11, 1, &texOffset[0]);
		glUniform3fv(12, 1, &lightPos[0]);
		glUniform1fv(13, 1, &sphereControls[0]);
		glUniform1fv(14, 1, &sphereControls[1]);
		glUniform3fv(15, 1, &spherePosition[0]);
		glUniform3fv(16, 1, &skyColor[0]);

		glm::vec2 waveTextureOffset = glm::vec2(-time/2, 0.0);
		glUniform2fv(17, 1, &waveTextureOffset[0]);
		
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, depthMap);
		
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, waveTexture);

		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);
		
		Model hold = player.getModel();
		hold.setVP(perspectiveMatrix*viewMatrix);
		depthBiasMVP = depthBiasMatrix*depthProjectionMatrix*depthViewMatrix*hold.getMatrix();
		glUniformMatrix4fv(3, 1, GL_FALSE, &depthBiasMVP[0][0]);
		glUniformMatrix4fv(8, 1, GL_FALSE, &hold.getMatrix()[0][0]);
		hold.renderPLY();

      	glDisable(GL_CULL_FACE);

		///Render Marbles
		glUseProgram(plyUVProgram);
		glUniform3fv(5, 1, &lightPos[0]);
		glUniform3fv(6, 1, &lightColor[0]);
		glUniform1f(7, lightPower);
		
		glUniformMatrix4fv(9, 1, GL_FALSE, &viewMatrix[0][0]);
		glUniform1f(10, bias);
		
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, depthMap);
		
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, marbleTexture);

		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);
		
		float marbleSin = (sin(time*2)/40.0f);
		plyMarble.translate(glm::vec3(0.0f, marbleSin, 0.0f));

		//PLY Marble
		plyMarble.setVP(perspectiveMatrix*viewMatrix);
		depthBiasMVP = depthBiasMatrix*depthProjectionMatrix*depthViewMatrix*plyMarble.getMatrix();
		glUniformMatrix4fv(3, 1, GL_FALSE, &depthBiasMVP[0][0]);
		glUniformMatrix4fv(8, 1, GL_FALSE, &plyMarble.getMatrix()[0][0]);
		plyMarble.renderPLY();

		glDisable(GL_CULL_FACE);

		//Light
		glUseProgram(simpleProgram);

		glUniformMatrix4fv(0, 1, GL_FALSE, &mvpLight[0][0]);
		glUniform4fv(1, 1, &mvpLightPosCorrect[0]);
		glDrawArrays(GL_POINTS, 0, 1);

		//Draw Framebuffer Texture To Screen
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		glViewport(0, 0, 1280, 720);
		glClearBufferfv(GL_COLOR, 0, &clearColorValue[0]);
		glClearBufferfv(GL_DEPTH, 0, clearDepthValue);
		
		//Copy from one framebuffer to another
		glBindFramebuffer(GL_READ_FRAMEBUFFER, frameBufferName);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
		glBlitFramebuffer(0, 0, 1280, 720, 0, 0, 1280, 720, GL_COLOR_BUFFER_BIT, GL_NEAREST);

		//Copy from texture to the framebuffer
		//glUseProgram(texProgram);
		
		//glActiveTexture(GL_TEXTURE0);
		//glBindTexture(GL_TEXTURE_2D, worldDepthMap);

		//glDrawArrays(GL_QUADS, 0, 4);


		//Button
		if(drawButtons) {
			glUseProgram(buttonProgram);
			for(unsigned int i = 0; i < guiContainer->getComponents().size(); i++) {
				for(int j = guiContainer->getComponents()[i]->getRenderables().size()-1; j >= 0;j--) {
					DreamRenderable* renderable = guiContainer->getComponents()[i]->getRenderables()[j];
					glUniform2fv(0, 1, &renderable->getSize()[0]);
					glUniform2fv(1, 1, &renderable->getOffset()[0]);
					glUniform4fv(2, 1, &renderable->getColor()[0]);
					glDrawArrays(GL_QUADS, 0, 4);
				}
			}
		}

		glfwSwapBuffers(window);
	}

	glfwTerminate();
	return 0;
}

ImagePBM readNetpbmFile(GLchar * ppmFileName) {
	ImagePBM imageStruct;

	std::ifstream image;
	image.open(ppmFileName, std::ios::binary | std::ios::in);

	if(!image.is_open()) {
		fprintf(stderr, "Image could not be opened");
		terminated = true;
		imageStruct.data = NULL;
		return imageStruct;
	}

	std::string netpbmFileType;
	getline(image, netpbmFileType);

	std::string line = "";
	getline(image, line);
	imageStruct.width = atoi(line.c_str());

	line = "";
	getline(image, line);
	imageStruct.height = atoi(line.c_str());

	if(netpbmFileType.compare("P5") == 0)
		imageStruct.elements = 1;
	else if(netpbmFileType.compare("P6") == 0)
		imageStruct.elements = 3;
	else
		imageStruct.elements = 3;

	imageStruct.data = reinterpret_cast<GLubyte*>(malloc(sizeof(GLubyte)*imageStruct.width*imageStruct.height*imageStruct.elements));

	line = "";
	getline(image, line);

	//Read Elements
	image.read(reinterpret_cast<char*>(imageStruct.data), sizeof(GLubyte)*imageStruct.width*imageStruct.height*imageStruct.elements);

	image.close();

	return imageStruct;
}

GLuint createRTexture(GLchar * textureFileName) {
	ImagePBM colorMap = readNetpbmFile(textureFileName);

	GLuint colorMapID;
	glGenTextures(1, &colorMapID);
	glBindTexture(GL_TEXTURE_2D, colorMapID);

	glTexStorage2D(GL_TEXTURE_2D, 1, GL_R8, colorMap.width, colorMap.height);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, colorMap.width, colorMap.height, GL_RED, GL_UNSIGNED_BYTE, colorMap.data);

	delete [] colorMap.data;
	return colorMapID;
}

GLuint createRTextureClamped(GLchar * textureFileName) {
	ImagePBM colorMap = readNetpbmFile(textureFileName);

	GLuint colorMapID;
	glGenTextures(1, &colorMapID);
	glBindTexture(GL_TEXTURE_2D, colorMapID);

	glTexStorage2D(GL_TEXTURE_2D, 1, GL_R8, colorMap.width, colorMap.height);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, colorMap.width, colorMap.height, GL_RED, GL_UNSIGNED_BYTE, colorMap.data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	delete [] colorMap.data;
	return colorMapID;
}

GLuint createRGBTexture(GLchar * textureFileName) {
	ImagePBM colorMap = readNetpbmFile(textureFileName);

	GLuint colorMapID;
	glGenTextures(1, &colorMapID);
	glBindTexture(GL_TEXTURE_2D, colorMapID);

	glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGB8, colorMap.width, colorMap.height);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, colorMap.width, colorMap.height, GL_RGB, GL_UNSIGNED_BYTE, colorMap.data);

	delete [] colorMap.data;
	return colorMapID;
}

GLuint createRGBTextureMipMapped(GLchar * textureFileName) {
	ImagePBM colorMap = readNetpbmFile(textureFileName);

	GLuint colorMapID;
	glGenTextures(1, &colorMapID);
	glBindTexture(GL_TEXTURE_2D, colorMapID);

	glTexStorage2D(GL_TEXTURE_2D, 9, GL_RGB8, colorMap.width, colorMap.height);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, colorMap.width, colorMap.height, GL_RGB, GL_UNSIGNED_BYTE, colorMap.data);
	glGenerateMipmap(GL_TEXTURE_2D);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

	delete [] colorMap.data;
	return colorMapID;
}

GLFWwindow* openGLInit(GLint width, GLint height, GLchar* windowTitle) {
	if(!glfwInit()) {
		fprintf(stderr, "GLFW could not be initialised");
		exit(1);
	}

	glfwWindowHint(GLFW_SAMPLES, 4);
	GLFWwindow* window = glfwCreateWindow(1280, 720, "Normal Mapped Water", NULL, NULL);

	if(!window) {
		fprintf(stderr, "Window could not be created");
		glfwTerminate();
		exit(1);
	}

	glfwMakeContextCurrent(window);	
	glfwSetKeyCallback(window, keyCallback);
	glfwSetCursorPosCallback(window, mouseMoveCallback);
	glfwSetMouseButtonCallback(window, mouseButtonCallback);

	glewExperimental = GL_TRUE;
	glewInit();

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glEnable(GL_MULTISAMPLE);

	glClearColor(1.0, 1.0, 1.0, 1.0);

	//glEnable(GL_CULL_FACE);
	//glCullFace(GL_BACK);
	
	glEnable(GL_DEPTH_CLAMP);

	return window;
}

void keyCallback (GLFWwindow * window, int key, int scancode, int action, int mods)
{
	/*
	if (action == GLFW_PRESS)
	{
		if (key == GLFW_KEY_E || key == GLFW_KEY_RIGHT)
			camera.rotation.x -= ROTATIONSPEED;

		else if (key == GLFW_KEY_Q || key == GLFW_KEY_LEFT)
			camera.rotation.x += ROTATIONSPEED;

		else if (key == GLFW_KEY_W)
			camera.translation.z += MOVESPEED;
		
		else if (key == GLFW_KEY_A)
			camera.translation.x += MOVESPEED;
		
		else if (key == GLFW_KEY_S)
			camera.translation.z -= MOVESPEED;
		
		else if (key == GLFW_KEY_D)
			camera.translation.x -= MOVESPEED;
		
		else if (key == GLFW_KEY_X)
			camera.translation.y -= MOVESPEED;
		
		else if (key == GLFW_KEY_Z)
			camera.translation.y += MOVESPEED;

		else if (key == GLFW_KEY_V)
			texOffset.x += 0.1f;

		else if (key == GLFW_KEY_C)
			texOffset.x -= 0.1f;

		else if (key == GLFW_KEY_B)
			drawButtons = !drawButtons;
		
		if (key ==  GLFW_KEY_ESCAPE)
		{
			glfwTerminate();
			terminated = true;
		}
	}

	
	if (action == GLFW_RELEASE)
	{
		if (key == GLFW_KEY_E || key == GLFW_KEY_RIGHT)
			camera.rotation.x = 0.00f;

		else if (key == GLFW_KEY_Q || key == GLFW_KEY_LEFT)
			camera.rotation.x = 0.00f;

		else if (key == GLFW_KEY_W)
			camera.translation.z = 0.0f;
		
		else if (key == GLFW_KEY_A)
			camera.translation.x = 0.0f;
		
		else if (key == GLFW_KEY_S)
			camera.translation.z = 0.0f;
		
		else if (key == GLFW_KEY_D)
			camera.translation.x = 0.0f;
		
		else if (key == GLFW_KEY_X)
			camera.translation.y = 0.0f;
		
		else if (key == GLFW_KEY_Z)
			camera.translation.y = 0.0f;

		else if (key == GLFW_KEY_V)
			texOffset.x += 0.1f;

		else if (key == GLFW_KEY_C)
			texOffset.x -= 0.1f;

		else if (key == GLFW_KEY_B)
			drawButtons = !drawButtons;
		
		if (key ==  GLFW_KEY_ESCAPE)
		{
			glfwTerminate();
			terminated = true;
		}
	}*/
}

void mouseMoveCallback(GLFWwindow * window, double mouseX, double mouseY) {
	int windowSize[2];
	glfwGetWindowSize(window, &windowSize[0], &windowSize[1]);
	checkForHover(mouseX*2/windowSize[0], mouseY*2/windowSize[1]);
}

void checkForHover(double mouseX, double mouseY) {
	if(buttonPressed != NULL && mousePressed) {
		buttonPressed->mouseDown(mouseX, mouseY);
	}
	else {
		for(unsigned int i=0; i<buttonList.size(); i++) {
			if(buttonList[i]->checkAndSetHovering(mouseX, mouseY)) {
				if(mousePressed) {
					buttonList[i]->mouseDown(mouseX, mouseY);
				}
			}
		}

		for(unsigned int i = 0; i < guiContainer->getComponents().size(); i++) {
			if(((DreamClickable*)(guiContainer->getComponents()[i]))->checkAndSetHovering(mouseX, mouseY)) {
				if(mousePressed) {
					((DreamClickable*)(guiContainer->getComponents()[i]))->mouseDown(mouseX, mouseY);
				}
			}
		}
	}
}

void mouseButtonCallback(GLFWwindow * window, int button, int action, int mods) {
	int windowSize[2];
	double mouseX, mouseY;

	glfwGetCursorPos(window, &mouseX, &mouseY);
	glfwGetWindowSize(window, &windowSize[0], &windowSize[1]);

	mouseX /= windowSize[0]/2.0;
	mouseY /= windowSize[1]/2.0;
	
	
	if(button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE) {
		mousePressed = false;
		buttonPressed = NULL;
	}
	else {
		for(unsigned int i=0; i<buttonList.size(); i++) {
			if(button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
				if(buttonList[i]->pointInAndExecute(mouseX, mouseY)) {
					mousePressed = true;
					buttonPressed = buttonList[i];
					break;
				}
			}
		}		

		for(unsigned int i = 0; i < guiContainer->getComponents().size(); i++) {
			if(button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
				if(((DreamClickable*)(guiContainer->getComponents()[i]))->pointInAndExecute(mouseX, mouseY)) {
					mousePressed = true;
					buttonPressed = (DreamClickable*)guiContainer->getComponents()[i];
					break;
				}
			}
		}
	}
}

void printFunction() {
	printf("I'm a button\n");
	for(unsigned int i = 0; i<guiContainer->getComponents().size(); i++) {
		((DreamClickable*)(guiContainer->getComponents()[i]))->print();
	}
}

GLuint createProgram(GLchar * vert_shader, GLchar * geom_shader, GLchar * tes_control_shader, GLchar * tes_eval_shader, GLchar * frag_shader)
{
	//Compile Program
	GLuint program = glCreateProgram();
	
	//Vertex Shader
	shaderFromFileAndLink(GL_VERTEX_SHADER, vert_shader, program);

	shaderFromFileAndLink(GL_GEOMETRY_SHADER, geom_shader, program);

	shaderFromFileAndLink(GL_TESS_CONTROL_SHADER, tes_control_shader, program);

	shaderFromFileAndLink(GL_TESS_EVALUATION_SHADER, tes_eval_shader, program);

	shaderFromFileAndLink(GL_FRAGMENT_SHADER, frag_shader, program);

	glLinkProgram(program);

	GLint program_log_length;
	glGetProgramiv(program, GL_INFO_LOG_LENGTH, &program_log_length);

	if (program_log_length > 1) {
		GLchar* program_log = new char[program_log_length + 1];
		glGetProgramInfoLog(program, program_log_length, NULL, program_log);
		printf("%s", program_log);
		glfwTerminate();
		exit(1);
	}

	return program;
}

void shaderFromFileAndLink(GLenum shaderType, GLchar * fileName, GLuint program)
{
	if (fileName != NULL) {
		std::string code = readFile (fileName);

		GLuint shader = createShaderFromCode(shaderType, code);

		glAttachShader (program, shader);

		glDeleteShader(shader);
	}

	return;
}

std::string readFile (GLchar * fileName)
{
	std::string fileData;
	std::ifstream file(fileName);

	if (file.is_open()) {
		std::string line = "";

		while (getline (file, line))
			fileData += "\n" + line;

		file.close();
	} else {
		fprintf(stderr, "ERROR: %s could not be opened", fileName);
		glfwTerminate();
	}

	return fileData;
}

GLuint createShaderFromCode (GLenum shaderType, std::string code)
{
	char const* source_code_pointer = code.c_str();
	GLuint shaderInt = glCreateShader(shaderType);
	glShaderSource(shaderInt, 1, &source_code_pointer, NULL);
	glCompileShader(shaderInt);

	GLint shaderLogLength;
	glGetShaderiv(shaderInt, GL_INFO_LOG_LENGTH, &shaderLogLength);

	if (shaderLogLength > 1) {
		GLchar* vs_log = new char[shaderLogLength + 1];
		glGetShaderInfoLog(shaderInt, shaderLogLength, NULL, vs_log);
		printf("Shader\n" + shaderType);
		printf("%s", vs_log);
		glfwTerminate();
		exit(1);
	}

	return shaderInt;
}

void renderSplashScreen(GLuint program, GLuint texture, GLFWwindow* window) {
	GLfloat* depth = new GLfloat(1.0f);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glViewport(0, 0, 1280, 720);
	glClearBufferfv(GL_COLOR, 0, &glm::vec4(0.0, 0.0, 0.0, 1.0)[0]);
	glClearBufferfv(GL_DEPTH, 0, depth);

	glUseProgram(program);
		
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);

	glDrawArrays(GL_QUADS, 0, 4);
	
	glfwSwapBuffers(window);
    
}
void computeMatricesFromInputs(GLFWwindow * window, glm::mat4 * cameraView, Player* player)
{
	glm::mat4 ViewMatrix = *cameraView;

	// glfwGetTime is called only once, the first time this function is called
	static double lastTime = glfwGetTime();
	// Initial horizontal angle : toward -Z
	static float horizontalAngle = 3.14f;
	// Initial vertical angle : none
	static float verticalAngle = 0.0f;
	// Initial Field of View
	static float initialFoV = 45.0f;
	static float speed = 7.0f; // 3 units / second
	static float rotateSpeed = 2.00f;
	static float mouseSpeed = 0.005f;
	player->setSpeed(1.0f);
	// Compute time difference between current and last frame
	double currentTime = glfwGetTime();
	float deltaTime = float(currentTime - lastTime);
	player->timeUpdate(deltaTime);
	// Get mouse position
	double xpos, ypos;
	glfwGetCursorPos(window, &xpos, &ypos);

	// Reset mouse position for next frame
	int width;
	int height;
	glfwGetWindowSize(window, &width, &height);
	glfwSetCursorPos(window, width / 2, height / 2);

	if (!locked)
	{
		// Compute new orientation
		horizontalAngle += mouseSpeed * float(width / 2 - xpos);
		verticalAngle += 0;//mouseSpeed * float(height / 2 - ypos);
	}
	else
	{
		if (player->getDirection().z < 0.0f && player->getDirection().x < 0.0f)
		{
			horizontalAngle = -asin(player->getDirection().x / cos(verticalAngle));
		}
		else if (player->getDirection().z < 0.0f)
		{
			horizontalAngle = acos(player->getDirection().z / cos(verticalAngle)) + PI;
		}
		else
		{
			horizontalAngle = asin(player->getDirection().x / cos(verticalAngle)) + PI;
		}	
	}
						   // Direction : Spherical coordinates to Cartesian coordinates conversion
	glm::vec3 direction(
		cos(verticalAngle) * sin(horizontalAngle),
		sin(verticalAngle),
		cos(verticalAngle) * cos(horizontalAngle)
		);
	
	// Right vector
	glm::vec3 right = glm::vec3(sin(horizontalAngle - PI / 2.0f), 0, cos(horizontalAngle - PI / 2.0f));

	// Up vector
	glm::vec3 up = glm::cross(-right, direction);


	// Move forward
	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
		camera.translation += direction * deltaTime * speed;
	}
	// Move backward
	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
		camera.translation -= direction * deltaTime * speed;
	}
	// Strafe right
	if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
		camera.translation += right * deltaTime * speed;
	}
	// Strafe left
	if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
		camera.translation -= right * deltaTime * speed;
	}
	//Player Controls
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
		player->setSpeed(1.4f);
	}
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_RELEASE) {
		player->resetSpeed(1.4f);
	}
	// Move backward
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		player->setSpeed(0.7f);
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_RELEASE) {
		player->resetSpeed(0.7f);
	}
	// Strafe right
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
		player->rotatePlayer(-rotateSpeed * deltaTime);
	}
	// Strafe left
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
		player->rotatePlayer(+rotateSpeed * deltaTime);
	}
	if (glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS) {
		player->scalePlayerUp();
	}
	if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS) {
		sounds.stop();
	}
	if (glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS)
	{
		if (!muteLock)
		{
			muteLock = true;
			//Mute Sounds Here
		}
		
	}
	if (glfwGetKey(window, GLFW_KEY_M) == GLFW_RELEASE)
	{
		muteLock = false;
	}
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
		if (!lockLock)
		{
			lockLock = true;
			if (locked)
			{
				locked = false;
			}
			else
			{
				locked = true;
				horizontalAngle = PI;
				direction = -player->getDirection();
			}
		}
		
	}
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_RELEASE)
	{
		lockLock = false;
	}
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwTerminate();
		terminated = true;
	}

	if (locked)
	{
		float angle = acos(((currentDirection.x * direction.x) + (currentDirection.z * direction.z) + (currentDirection.y * direction.y)) / (sqrt(pow(currentDirection.x, 2) + pow(currentDirection.z, 2) + pow(currentDirection.y, 2)) * sqrt(pow(direction.x, 2) + pow(direction.z, 2) + pow(direction.y, 2))));
		if (angle > 0.1f)
		{
			float ratio = 0.1f / angle;
			glm::vec3 directionMove = (direction - currentDirection) * ratio;
			currentDirection = currentDirection + directionMove;
						glm::normalize(currentDirection);
		}
		else
		{
			currentDirection = direction;
		}
	}
	else
	{
		currentDirection = direction;
	}

	player->translate(player->getDirection() * deltaTime * player->getSpeed());
	camera.translation = (player->getPosition() + ((-currentDirection * 6.0f))) + glm::vec3(0.0f, 3.0f, 0.0f);

	float FoV = initialFoV;// - 5 * glfwGetMouseWheel(); // Now GLFW 3 requires setting up a callback for this. It's a bit too complicated for this beginner's tutorial, so it's disabled instead.

						   // Camera matrix
	ViewMatrix = glm::lookAt(
		camera.translation,           // Camera is here
		camera.translation + currentDirection, // and looks here : at the same position, plus "direction"
		up                  // Head is up (set to 0,-1,0 to look upside-down)
		);

	*cameraView = ViewMatrix;

	// For the next frame, the "last time" will be "now"
	lastTime = currentTime;
}
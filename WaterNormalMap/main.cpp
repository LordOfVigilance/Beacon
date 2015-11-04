#include "GL/GLEW.h"
#define GLFW_DLL
#include "GLFW/glfw3.h"

#include <stdio.h>
#include <string>
#include <fstream>

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include <vector>

#include "DreamButton.h"
#include "DreamSlider.h"
#include "DreamClickable.h"
#include "DreamContainer.h"

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
} uniforms;

struct ImagePBM {
	GLint width;
	GLint height;
	GLubyte* data;
};

struct {
	glm::vec2 rotation;
	glm::vec3 translation;
} camera;

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
ImagePBM readPBMFile(GLchar *);
GLuint createTexture(GLchar *);
bool loadObjIndexed(const char *, std::vector<GLfloat>&, std::vector<GLfloat>&, std::vector<GLushort>&);

void printFunction();

bool terminated = false;
DreamClickable* buttonPressed;
bool mousePressed = false;
DreamContainer* guiContainer;

int main(void) {

	GLFWwindow* window = openGLInit(1280, 720, "Water Normal Mapped");

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

	GLuint simpleProgram = createProgram("simplePass.vert", NULL, NULL, NULL, "simplePass.frag");
	GLuint rainProgram = createProgram("rain.vert", "rain.geom", NULL, NULL, "rain.frag");
	GLuint buttonProgram = createProgram("button.vert", NULL, NULL, NULL, "button.frag");
	
	GLuint texProgram = createProgram("tex.vert", NULL, NULL, NULL, "tex.frag");

	glPatchParameteri(GL_PATCH_VERTICES, 4);
	glPointSize(16);

	GLuint frameBufferName;
	glGenFramebuffers(1, &frameBufferName);
	glBindFramebuffer(GL_FRAMEBUFFER, frameBufferName);

	GLuint frameBufferColorTextureName;
	glGenTextures(1, &frameBufferColorTextureName);
	glBindTexture(GL_TEXTURE_2D, frameBufferColorTextureName);
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGB8, 1280, 720);

	glTextureParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTextureParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	GLuint frameBufferDepthTextureName;
	glGenTextures(1, &frameBufferDepthTextureName);
	glBindTexture(GL_TEXTURE_2D, frameBufferDepthTextureName);
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_DEPTH_COMPONENT32F, 1280, 720);//GL_DEPTH_COMPONENT16

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);

	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, frameBufferColorTextureName, 0);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, frameBufferDepthTextureName, 0);

	static const GLenum drawBuffers[] = {GL_COLOR_ATTACHMENT0};
	glDrawBuffers(1, drawBuffers);

	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	GLuint waterTexture = createTexture("Textures/Water.pbm");
	GLuint fiberTexture = createTexture("Textures/colorMap.pbm");
	
	std::vector<GLfloat> monkey_vertices;
	std::vector<GLfloat> monkey_normals;
	std::vector<GLushort> monkey_indices;

	loadObjIndexed("Models/Land.obj", monkey_vertices, monkey_normals, monkey_indices);	
	
	//Create a vertex attribute object
	GLuint rainVao = 0;
	glGenVertexArrays(1, &rainVao);
	glBindVertexArray(rainVao);

	//Create a buffer from openGL
	GLuint rainVbo = 1;
	glGenBuffers(1, &rainVbo);
	glBindBuffer(GL_ARRAY_BUFFER, rainVbo);

	//Allocate space from openGL for the rain attributes
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * monkey_vertices.size(), NULL, GL_STATIC_DRAW);

	GLuint offset = 0;
	//Fill each attribue with data
	glBufferSubData(GL_ARRAY_BUFFER, offset, sizeof(GLfloat) * monkey_vertices.size(), monkey_vertices.data());

	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);

	glEnableVertexAttribArray(0);

	GLuint vertex_indices_buffer;
	glGenBuffers(1, &vertex_indices_buffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vertex_indices_buffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLushort) * monkey_indices.size(), monkey_indices.data(), GL_STATIC_DRAW);


	camera.rotation = glm::vec2();
	camera.translation = glm::vec3();
	
	glm::vec4 lightPos(0.0, 100.0, -500.0, 1.0);
	glm::vec3 lightColor(0.6, 0.6, 0.9);
	GLfloat lightPower(1.0);

	glm::vec3 eye(0.0, 8.0, 3.0);
	glm::vec3 center(0.0, 8.0, 0.0);
	glm::vec3 up(0.0, -1.0, 0.0);
	glm::mat4 modelMatrixLight(1.0f);
	glm::mat4 modelMatrixWater(1.0f);
	glm::mat4 modelMatrixLand(1.0f);
	modelMatrixLand = glm::scale(modelMatrixLand, glm::vec3(17.0, 17.0, 17.0));
	modelMatrixLand = glm::translate(modelMatrixLand, glm::vec3(0.0, -0.4, 0.0));
	glm::mat4 viewMatrix = glm::lookAt(eye, center, up);
	glm::mat4 perspectiveMatrix = glm::perspective(30.0f, 16.0f/9.0f, 1.0f, 500.0f);
	glm::mat4 mvpWater = perspectiveMatrix * viewMatrix * modelMatrixWater;
	glm::mat4 mvpLand = perspectiveMatrix * viewMatrix * modelMatrixLand;
	glm::mat4 mvpLight = perspectiveMatrix * viewMatrix * modelMatrixLight;

	
	
	glm::vec3 reflectionColor(1.0f, 1.0f, 1.0f);
	glm::vec3 specularColor(1.0f, 1.0f, 1.0f);
	glm::vec3 diffuseColor(1.0f, 1.0f, 1.0f);

	GLfloat var1 = 10.0f;
	GLfloat var2 = 25.0f;
	GLfloat var3 = 1.0f;

	GLfloat dmapDepth = 6.0f;

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
	guiContainer->addComponent(&var1, 0.0f, 10.0f);
	guiContainer->addComponent(&var2, 0.0f, 10.0f);
	guiContainer->addComponent(&var3, 0.0f, 100.0f);
	guiContainer->addComponent(&dmapDepth, -10.0f, 10.0f);

	glm::vec4 clearColorValue = glm::vec4(1.0f, 1.0f, 1.0f, 1.0);
	float* clearDepthValue = new float(1.0f);

	while(!glfwWindowShouldClose(window) && !terminated) {
		glfwPollEvents();

		//Draw to FrameBuffer
		glBindFramebuffer(GL_FRAMEBUFFER, frameBufferName);

		glViewport(0, 0, 1280, 720);
		glClearBufferfv(GL_COLOR, 0, &clearColorValue[0]);
		glClearBufferfv(GL_DEPTH, 0, clearDepthValue);

		double time = glfwGetTime();
		texOffset.x = -time/20;
		texOffset.y = time/40;

		float sinValue = (sin(glfwGetTime())/4.0 + 1.25);

		viewMatrix = glm::rotate(viewMatrix, camera.rotation.x, glm::vec3(0.0, 1.0, 0.0));
		viewMatrix = glm::translate(viewMatrix, camera.translation);
		mvpLight = perspectiveMatrix*viewMatrix*modelMatrixLight;
		mvpLand = perspectiveMatrix*viewMatrix*modelMatrixLand;
		mvpWater = perspectiveMatrix*viewMatrix*modelMatrixWater;

		glm::vec4 mvpLightPos = lightPos*mvpLight;
		glm::vec4 mvpEyePos = glm::vec4(eye, 1.0)*mvpLight;
		glm::vec4 mvpLightPosCorrect = mvpLight*lightPos;

		//Land
		glUseProgram (rainProgram);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vertex_indices_buffer);
		glBindVertexArray (rainVao);

		glUniformMatrix4fv(4, 1, GL_FALSE, &mvpLand[0][0]);
		glUniform3fv(5, 1, &mvpLightPosCorrect[0]);
		glUniform3fv(6, 1, &lightColor[0]);
		glUniform1f(7, lightPower);

		glDrawElements(GL_TRIANGLES, monkey_indices.size(), GL_UNSIGNED_SHORT, (void *) 0);

		//Water
		glDepthMask(GL_FALSE);
		glUseProgram(program);
		
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, waterTexture);
		
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, frameBufferDepthTextureName);

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

		glDrawArraysInstanced(GL_PATCHES, 0, 4, 256*256);
		glBindTexture(GL_TEXTURE_2D, 0);
		
		glDepthMask(GL_TRUE);

		//Light
		glUseProgram(simpleProgram);

		glUniformMatrix4fv(0, 1, GL_FALSE, &mvpLight[0][0]);
		glUniform4fv(1, 1, &mvpLightPosCorrect[0]);
		glDrawArrays(GL_POINTS, 0, 1);

		//Button
		glUseProgram(buttonProgram);
		for(int i = 0; i<buttonList.size(); i++) {
			for(int j = buttonList[i]->getRenderables().size()-1; j>=0; j--) {
				DreamRenderable* renderable = buttonList[i]->getRenderables()[j];
				glUniform2fv(0, 1, &renderable->getSize()[0]);
				glUniform2fv(1, 1, &renderable->getOffset()[0]);
				glUniform4fv(2, 1, &renderable->getColor()[0]);
				glDrawArrays(GL_QUADS, 0, 4);
			}
		}

		for(int i = 0; i < guiContainer->getComponents().size(); i++) {
			for(int j = guiContainer->getComponents()[i]->getRenderables().size()-1; j >= 0;j--) {
				DreamRenderable* renderable = guiContainer->getComponents()[i]->getRenderables()[j];
				glUniform2fv(0, 1, &renderable->getSize()[0]);
				glUniform2fv(1, 1, &renderable->getOffset()[0]);
				glUniform4fv(2, 1, &renderable->getColor()[0]);
				glDrawArrays(GL_QUADS, 0, 4);
			}
		}

		//Draw Framebuffer Texture To Screen
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		glViewport(0, 0, 1280, 720);
		glClearBufferfv(GL_COLOR, 0, &clearColorValue[0]);
		glClearBufferfv(GL_DEPTH, 0, clearDepthValue);
		
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, frameBufferColorTextureName);

		glUseProgram(texProgram);

		glDrawArrays(GL_QUADS, 0, 4);

		glBindTexture(GL_TEXTURE_2D, 0);


		glfwSwapBuffers(window);
	}

	glfwTerminate();
	return 0;
}

ImagePBM readPBMFile(GLchar * ppmFileName) {
	ImagePBM imageStruct;

	std::ifstream image(ppmFileName);

	if(!image.is_open()) {
		fprintf(stderr, "Image could not be opened");
		terminated = true;
		imageStruct.data = NULL;
		return imageStruct;
	}

	std::string ppmFileType;
	getline(image, ppmFileType);

	std::string line = "";
	getline(image, line);
	imageStruct.width = atoi(line.c_str());

	line = "";
	getline(image, line);
	imageStruct.height = atoi(line.c_str());

	imageStruct.data = new GLubyte[imageStruct.width*imageStruct.height];

	line = "";
	getline(image, line);

	for(int i=0; i<imageStruct.width*imageStruct.height; i++)
		imageStruct.data[i] = (GLubyte)image.get();

	return imageStruct;
}

GLuint createTexture(GLchar * textureFileName) {
	ImagePBM colorMap = readPBMFile(textureFileName);

	GLuint colorMapID;
	glGenTextures(1, &colorMapID);
	glBindTexture(GL_TEXTURE_2D, colorMapID);

	glTexStorage2D(GL_TEXTURE_2D, 1, GL_R8, colorMap.width, colorMap.height);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, colorMap.width, colorMap.height, GL_RED, GL_UNSIGNED_BYTE, colorMap.data);

	delete [] colorMap.data;
	return colorMapID;
}

GLFWwindow* openGLInit(GLint width, GLint height, GLchar* windowTitle) {
	if(!glfwInit()) {
		fprintf(stderr, "GLFW could not be initialised");
		exit(1);
	}

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
	glDepthFunc(GL_LESS);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glClearColor(1.0, 1.0, 1.0, 1.0);

	//glEnable(GL_CULL_FACE);
	//glCullFace(GL_BACK);

	return window;
}

void keyCallback (GLFWwindow * window, int key, int scancode, int action, int mods)
{
	if (action == GLFW_PRESS)
	{
		if (key == GLFW_KEY_E || key == GLFW_KEY_RIGHT)
			camera.rotation.x -= 0.01f;

		else if (key == GLFW_KEY_Q || key == GLFW_KEY_LEFT)
			camera.rotation.x += 0.01f;

		else if (key == GLFW_KEY_W)
			camera.translation.z += 0.1f;
		
		else if (key == GLFW_KEY_A)
			camera.translation.x += 0.1f;
		
		else if (key == GLFW_KEY_S)
			camera.translation.z -= 0.1f;
		
		else if (key == GLFW_KEY_D)
			camera.translation.x -= 0.1f;
		
		else if (key == GLFW_KEY_X)
			camera.translation.y -= 0.1f;
		
		else if (key == GLFW_KEY_Z)
			camera.translation.y += 0.1f;

		else if (key == GLFW_KEY_V)
			texOffset.x += 0.1f;

		else if (key == GLFW_KEY_C)
			texOffset.x -= 0.1f;
		
		if (key ==  GLFW_KEY_ESCAPE)
		{
			glfwTerminate();
			terminated = true;
		}
	}
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

		for(int i = 0; i < guiContainer->getComponents().size(); i++) {
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

		for(int i = 0; i < guiContainer->getComponents().size(); i++) {
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
	for(int i = 0; i<guiContainer->getComponents().size(); i++) {
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

GLuint createShaderFromCode (GLenum shaderType, std::string code) {
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

bool loadObjIndexed(const char * obj_file_name, std::vector<GLfloat>& vertex_position, std::vector<GLfloat>& vertex_normal, std::vector<GLushort>& vertex_indices) {
	
	FILE * obj_file = fopen (obj_file_name, "r");
	
	//File not opened
	if ( obj_file == NULL ) {
		fprintf(stderr, "ERROR: %s could not be opened", obj_file_name);

		return false;
	}
	//File opened
	else {
		while ( 1 ) {

			char lineHeader[128];
			int res = fscanf ( obj_file, "%s", lineHeader );
			if ( res == EOF )
				break;

			if ( strcmp (lineHeader, "o") == 0 ) {			//new object
				printf("object\n");
				continue;
			}

			else if ( strcmp (lineHeader, "v") == 0 ) {	//new vertex
				GLfloat x, y, z;
				fscanf(obj_file, "%f %f %f\n", &x, &y, &z);
				vertex_position.push_back(x);
				vertex_position.push_back(y);
				vertex_position.push_back(z);
				vertex_position.push_back(1.0f);
			}

			else if ( strcmp (lineHeader, "vn") == 0 ) { //new vertex normal
				GLfloat x, y, z;
				fscanf(obj_file, "%f %f %f\n", &x, &y, &z);
				vertex_normal.push_back(x);
				vertex_normal.push_back(y);
				vertex_normal.push_back(z);
				vertex_normal.push_back(1.0f);
			}
			
			else if ( strcmp (lineHeader, "s") == 0 ) {
				char comment_buffer[128];
				fgets(comment_buffer, 128, obj_file);
			}

			else if ( strcmp (lineHeader, "f") == 0 ) {
				GLuint fv1, fn1, fv2, fn2, fv3, fn3;
				fscanf (obj_file, "%u//%u %u//%u %u//%u\n", &fv1, &fn1, &fv2, &fn2, &fv3, &fn3);
				vertex_indices.push_back((short)fv1 - 1);
				vertex_indices.push_back((short)fv2 - 1);
				vertex_indices.push_back((short)fv3 - 1);
			} else {
				char comment_buffer[128];
				fgets(comment_buffer, 128, obj_file);
			}
		}
	}

	return true;
}
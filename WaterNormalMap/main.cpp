#include "GL/GLEW.h"
#define GLFW_DLL
#include "GLFW/glfw3.h"

#include <stdio.h>
#include <string>
#include <fstream>

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

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


glm::vec2 texOffset(1.5, 0.0);

GLFWwindow* openGLInit(GLint, GLint, GLchar*);
void keyCallback (GLFWwindow*, int, int, int, int);
GLuint createProgram (GLchar *, GLchar *, GLchar *, GLchar *, GLchar *);
void shaderFromFileAndLink (GLenum, GLchar *, GLuint);
std::string readFile (GLchar *);
GLuint createShaderFromCode (GLenum, std::string);
ImagePBM readPBMFile(GLchar *);

bool terminated = false;

int main(void) {

	GLFWwindow* window = openGLInit(1280, 720, "Water Normal Mapped");

	GLuint program = createProgram("waterNM.vert", "waterNM.geom", "waterNM.tcs", "waterNM.tes", "waterNM.frag");
	uniforms.dmapDepth = glGetUniformLocation(program, "dmap_depth");
	uniforms.mvp = glGetUniformLocation(program, "mvp");
	uniforms.texDepthMap = glGetUniformLocation(program, "texDisplacement");
	uniforms.texOffset = glGetUniformLocation(program, "texOffset");
	uniforms.sinValue = glGetUniformLocation(program, "sinValue");
	uniforms.lightPower = glGetUniformLocation(program, "lightPower");
	uniforms.lightColor = glGetUniformLocation(program, "lightColor");
	uniforms.lightPos = glGetUniformLocation(program, "lightPos");
	uniforms.eyePos = glGetUniformLocation(program, "eyePos");

	GLuint simpleProgram = createProgram("simplePass.vert", NULL, NULL, NULL, "simplePass.frag");

	glPatchParameteri(GL_PATCH_VERTICES, 4);
	glPointSize(16);

	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	ImagePBM depthMap = readPBMFile("Water.pbm");
	GLuint depthMapID;
	glActiveTexture(GL_TEXTURE0);
	glGenTextures(1, &depthMapID);
	glBindTexture(GL_TEXTURE_2D, depthMapID);
	// Allocate storage for the texture data
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_R8, depthMap.width, depthMap.height);
	// Specify the data for the texture
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, depthMap.width, depthMap.height, GL_RED, GL_UNSIGNED_BYTE, depthMap.data);
	delete [] depthMap.data;
	
	ImagePBM colorMap = readPBMFile("colorMap.pbm");
	GLuint colorMapID;
	glActiveTexture(GL_TEXTURE1);
	glGenTextures(1, &colorMapID);
	glBindTexture(GL_TEXTURE_2D, colorMapID);
	// Allocate storage for the texture data
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_R8, colorMap.width, colorMap.height);
	// Specify the data for the texture
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, colorMap.width, colorMap.height, GL_RED, GL_UNSIGNED_BYTE, colorMap.data);
	delete [] colorMap.data;

	camera.rotation = glm::vec2();
	camera.translation = glm::vec3();
	
	glm::vec4 lightPos(0.0, 0.0, 100000.0, 1.0);
	glm::vec3 lightColor(1.0, 0.3, 0.2);
	GLfloat lightPower(30.0);

	glm::vec3 eye(0.0, 0.0, 3.0);
	glm::vec3 center(0.0, 0.0, 0.0);
	glm::vec3 up(0.0, 1.0, 0.0);
	glm::mat4 modelMatrix(1.0f);
	glm::mat4 viewMatrix = glm::lookAt(eye, center, up);
	glm::mat4 perspectiveMatrix = glm::perspective(30.0f, 16.0f/9.0f, 0.1f, 1000.0f);
	glm::mat4 mvp = perspectiveMatrix * viewMatrix * modelMatrix;
	
	GLfloat dmapDepth = 2;

	while(!glfwWindowShouldClose(window) && !terminated) {

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		double time = glfwGetTime();
		texOffset.x = -time/20;
		texOffset.y = time/40;

		float sinValue = (sin(glfwGetTime())/4.0 + 1.25);

		viewMatrix = glm::rotate(viewMatrix, camera.rotation.x, glm::vec3(0.0, 1.0, 0.0));
		viewMatrix = glm::translate(viewMatrix, camera.translation);
		mvp = perspectiveMatrix*viewMatrix*modelMatrix;

		glUseProgram(program);
		glBindVertexArray(vao);

		glUniform1f(uniforms.dmapDepth, dmapDepth);
		glUniform1f(uniforms.sinValue, sinValue);
		glUniformMatrix4fv(uniforms.mvp, 1, GL_FALSE, &mvp[0][0]);
		glUniform2fv(uniforms.texOffset, 1, &texOffset[0]);
		
		glm::vec4 mvpLightPos = lightPos*mvp;
		glm::vec4 mvpEyePos = glm::vec4(eye, 1.0)*mvp;
		glUniform1f(uniforms.lightPower, lightPower);
		glUniform3fv(uniforms.lightPos, 1, &mvpLightPos[0]);
		glUniform3fv(uniforms.lightColor, 1, &lightColor[0]);
		
		glUniform3fv(uniforms.lightPos, 1, &mvpEyePos[0]);

		//glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

		glDrawArraysInstanced(GL_PATCHES, 0, 4, 64*64);

		glUseProgram(simpleProgram);

		glUniformMatrix4fv(0, 1, GL_FALSE, &mvp[0][0]);
		glUniform4fv(1, 1, &mvpLightPos[0]);
		glDrawArrays(GL_POINTS, 0, 1);

		glfwPollEvents();
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

	glewExperimental = GL_TRUE;
	glewInit();

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	//glEnable(GL_CULL_FACE);
	//glCullFace(GL_BACK);

	return window;
}

void keyCallback (GLFWwindow * window, int key, int scancode, int action, int mods)
{
	if (action == GLFW_PRESS)
	{
		if (key == GLFW_KEY_E || key == GLFW_KEY_RIGHT)
			camera.rotation.x -= 0.01;

		else if (key == GLFW_KEY_Q || key == GLFW_KEY_LEFT)
			camera.rotation.x += 0.01;

		else if (key == GLFW_KEY_W)
			camera.translation.z += 0.1;
		
		else if (key == GLFW_KEY_A)
			camera.translation.x -= 0.1;
		
		else if (key == GLFW_KEY_S)
			camera.translation.z -= 0.1;
		
		else if (key == GLFW_KEY_D)
			camera.translation.x += 0.1;
		
		else if (key == GLFW_KEY_X)
			camera.translation.y += 0.1;
		
		else if (key == GLFW_KEY_Z)
			camera.translation.y -= 0.1;

		else if (key == GLFW_KEY_V)
			texOffset.x += 0.1;

		else if (key == GLFW_KEY_C)
			texOffset.x -= 0.1;
		
		if (key ==  GLFW_KEY_ESCAPE)
		{
			glfwTerminate();
			terminated = true;
		}
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
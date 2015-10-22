#include "GL/GLEW.h"
#define GLFW_DLL
#include "GLFW/glfw3.h"

#include <stdio.h>
#include <string>
#include <fstream>

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include <vector>

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
bool loadObjIndexed(const char *, std::vector<GLfloat>&, std::vector<GLfloat>&, std::vector<GLushort>&);

bool terminated = false;

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

	GLuint simpleProgram = createProgram("simplePass.vert", NULL, NULL, NULL, "simplePass.frag");
	GLuint rainProgram = createProgram("rain.vert", "rain.geom", NULL, NULL, "rain.frag");
	GLuint buttonProgram = createProgram("button.vert", NULL, NULL, NULL, "button.frag");

	glPatchParameteri(GL_PATCH_VERTICES, 4);
	glPointSize(16);

	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	ImagePBM depthMap = readPBMFile("Textures/Water.pbm");
	GLuint depthMapID;
	glActiveTexture(GL_TEXTURE0);
	glGenTextures(1, &depthMapID);
	glBindTexture(GL_TEXTURE_2D, depthMapID);
	// Allocate storage for the texture data
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_R8, depthMap.width, depthMap.height);
	// Specify the data for the texture
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, depthMap.width, depthMap.height, GL_RED, GL_UNSIGNED_BYTE, depthMap.data);
	delete [] depthMap.data;
	
	ImagePBM colorMap = readPBMFile("Textures/colorMap.pbm");
	GLuint colorMapID;
	glActiveTexture(GL_TEXTURE1);
	glGenTextures(1, &colorMapID);
	glBindTexture(GL_TEXTURE_2D, colorMapID);
	// Allocate storage for the texture data
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_R8, colorMap.width, colorMap.height);
	// Specify the data for the texture
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, colorMap.width, colorMap.height, GL_RED, GL_UNSIGNED_BYTE, colorMap.data);
	delete [] colorMap.data;

	

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
	glm::vec3 lightColor(1.0, 0.3, 0.2);
	GLfloat lightPower(300.0);

	glm::vec3 eye(0.0, 8.0, 3.0);
	glm::vec3 center(0.0, 8.0, 0.0);
	glm::vec3 up(0.0, -1.0, 0.0);
	glm::mat4 modelMatrixLight(1.0f);
	glm::mat4 modelMatrixWater(1.0f);
	glm::mat4 modelMatrixLand(1.0f);
	modelMatrixLand = glm::scale(modelMatrixLand, glm::vec3(17.0, 17.0, 17.0));
	modelMatrixLand = glm::translate(modelMatrixLand, glm::vec3(0.0, -0.2, 0.0));
	glm::mat4 viewMatrix = glm::lookAt(eye, center, up);
	glm::mat4 perspectiveMatrix = glm::perspective(30.0f, 16.0f/9.0f, 0.1f, 1000.0f);
	glm::mat4 mvpWater = perspectiveMatrix * viewMatrix * modelMatrixWater;
	glm::mat4 mvpLand = perspectiveMatrix * viewMatrix * modelMatrixLand;
	glm::mat4 mvpLight = perspectiveMatrix * viewMatrix * modelMatrixLight;
	
	GLfloat dmapDepth = 1.5;

	int windowWidth, windowHeight;
	glfwGetWindowSize(window, &windowWidth, &windowHeight);

	glm::vec2 buttonSize = glm::vec2(100.0f/windowWidth, 100.0f/windowHeight);
	glm::vec2 buttonOffset = glm::vec2(100.0f/windowWidth, 100.0f/windowHeight);

	while(!glfwWindowShouldClose(window) && !terminated) {

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

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
		glUniform4fv(5, 1, &mvpLightPosCorrect[0]);
		glUniform3fv(6, 1, &lightColor[0]);
		glUniform1f(7, lightPower);

		glDrawElements(GL_TRIANGLES, monkey_indices.size(), GL_UNSIGNED_SHORT, (void *) 0);

		//Water
		glUseProgram(program);
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

		glDrawArraysInstanced(GL_PATCHES, 0, 4, 256*256);

		//Light
		glUseProgram(simpleProgram);

		glUniformMatrix4fv(0, 1, GL_FALSE, &mvpLight[0][0]);
		glUniform4fv(1, 1, &mvpLightPosCorrect[0]);
		glDrawArrays(GL_POINTS, 0, 1);

		//Button
		glUseProgram(buttonProgram);
		glUniform2fv(0, 1, &buttonSize[0]);
		glUniform2fv(1, 1, &buttonOffset[0]);
		glDrawArrays(GL_QUADS, 0, 4);

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
			camera.rotation.x -= 0.01;

		else if (key == GLFW_KEY_Q || key == GLFW_KEY_LEFT)
			camera.rotation.x += 0.01;

		else if (key == GLFW_KEY_W)
			camera.translation.z += 0.1;
		
		else if (key == GLFW_KEY_A)
			camera.translation.x += 0.1;
		
		else if (key == GLFW_KEY_S)
			camera.translation.z -= 0.1;
		
		else if (key == GLFW_KEY_D)
			camera.translation.x -= 0.1;
		
		else if (key == GLFW_KEY_X)
			camera.translation.y -= 0.1;
		
		else if (key == GLFW_KEY_Z)
			camera.translation.y += 0.1;

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
#pragma once
#include <vector>
#include "GL/GLEW.h"
#include "glm\mat4x4.hpp"
#include "glm\gtx\transform.hpp"

class Model
{
public:
	Model(GLchar*);
	~Model(void);

	void scale(glm::vec3);
	void translate(glm::vec3);
	void rotate(GLfloat, glm::vec3);

	void render();

	GLuint getVAO();
	GLuint getVBO();
	GLuint getVIB();
	GLuint getVertexCount();
	glm::mat4x4 getMatrix();

	glm::mat4x4 getMVP();
	void setVP(glm::mat4x4);
	
	static bool loadObjIndexed(const char *, std::vector<float>&, std::vector<float>&, std::vector<unsigned short>&);

private:	
	GLuint vao;
	GLuint vbo;
	GLuint vertexIndicesBuffer;

	GLuint vertexCount;
	
	glm::mat4x4 matrix;
	glm::mat4x4 mvp;
};
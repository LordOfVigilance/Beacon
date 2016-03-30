#pragma once
#include <vector>
#include <fstream>
#include <sys/stat.h>

#include "GL/GLEW.h"
#include "glm\mat4x4.hpp"
#include "glm\gtx\transform.hpp"

#include "tinyxml2.h"

enum {
	COLLADAE,
	WAVEFORM,
	PLY,
	PLYMALLOC,
	PLYUVMALLOC,
	PLYUVMALLOCINDEXED
};

struct DaeGeom {
	void* data;
	int size;
	unsigned int stride;
};

class Model
{
public:
	Model(GLchar*);
	Model(GLchar*, int);
	Model(GLchar*, GLint, GLfloat*);
	~Model(void);

	void scale(glm::vec3);
	void translate(glm::vec3);
	void rotate(GLfloat, glm::vec3);

	void render();
	void renderPLY();
	void renderPLYInstanced(GLsizei);

	GLuint getVAO();
	GLuint getVBO();
	GLuint getInstanceBuffer();
	GLuint getVIB();
	GLuint getVertexCount();
	glm::mat4x4 getMatrix();

	glm::mat4x4 getMVP();
	void setVP(glm::mat4x4);
	
	static bool loadColladaeIndexed(const char*, DaeGeom*, DaeGeom*, DaeGeom*);
	static bool loadPlyIndexed(const char*, std::vector<GLfloat>&, std::vector<GLfloat>&, std::vector<GLuint>&, std::vector<GLfloat>&);
	static bool loadPlyIndexedMalloc(const char*, GLfloat**, GLfloat**, GLfloat**, GLuint**, int*, int*);
	static bool loadPlyUVIndexedMalloc(const char*, GLfloat**, GLfloat**, GLfloat**, GLuint**, int*, int*);
	static bool loadObjIndexed(const char *, std::vector<GLfloat>&, std::vector<GLfloat>&, std::vector<GLushort>&);

private:	
	GLuint vao;
	GLuint vbo;
	GLuint instanceBuffer;
	GLuint vertexIndicesBuffer;

	GLuint vertexCount;
	
	glm::mat4x4 matrix;
	glm::mat4x4 mvp;
};
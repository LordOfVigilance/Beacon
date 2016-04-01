#include "glm/glm.hpp"
#include "Model.h"

#pragma once
class Beacon
{
public:
	Beacon();
	Beacon(glm::vec3 positionIn, glm::vec2 rotationIn, Model modelIn);
	~Beacon();
	glm::vec3 getPosition();
	Model getModel();
private:
	glm::vec3 position;
	glm::vec2 rotation;
	Model model;
};


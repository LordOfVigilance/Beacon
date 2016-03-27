#pragma once

#include "Model.h"

class Collectible
{
public:
	Collectible();
	Collectible(glm::vec3 positionIn, float radiusIn, bool visibleIn, Model modelIn);
	~Collectible();

	glm::vec3 getPosition();
	float getRadius();
	bool getVisible();

	void setPostion(glm::vec3 positionIn);
	void collide();

private:
	Model model;
	glm::vec3 position;
	float radius;
	bool visible;
};

